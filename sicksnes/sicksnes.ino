//////////////////////////////////////////////////////
//      _      _                                    //
//     (_)    | |                                   //
//  ___ _  ___| | __   ___ ____   ___  ___          //
// / __| |/ __| |/ /  / __| '_ \ / _ \/ __|         //
// \__ \ | (__|   <   \__ \ | | |  __/\__ \         //
// |___/_|\___|_|\_\  \___/_| |_|\___||___/  v2.1.0 //
//////////////////////////////////////////////////////
//
// Made by @lucaslealdev -- original project, hardware mod and the whole v1.x
// firmware: https://github.com/lucaslealdev/sicksnes
//
// The _sick snes_ provides automatic lockout chip toggling and in-game reset.
// If the cartridge does not pass the CIC check, the mod disables the lockout
// chip and resets the console automatically.
//
// v2.x is a maintenance release of that work (same GPL-3.0 license):
//  - reset + CIC boot sequence is byte-identical to v1.5.0 (proven to boot)
//  - three plain LED outputs: D7 = red, D6 = blue, D3 = green
//  - controller pins are configured correctly (A0/A1/A2)
//  - bus reads have timeouts; combos must be held before firing
//  - EEPROM is validated and stored at a separate address (v1.5.0 combos and
//    the v1.5.0 image keep working when you flash back and forth)
//  - default combos no longer collide with SD2SNES/FXPak shortcuts
//
// >>> CONFIGURACAO: nao edite este arquivo.
//     Abra a aba "config.h" e descomente a linha da sua instalacao. <<<

#include <Arduino.h>

#include "pins.h"
#include "buttons.h"
#include "combo.h"
#include "cic.h"
#include "reset.h"
#include "led.h"
#include "controller.h"

bool debug = SICKNES_DEBUG;

#if HAS_CONTROLLER
enum Mode : uint8_t {
    MODE_NORMAL = 0,
    MODE_CONFIG_QUICK,
    MODE_CONFIG_LONG
};

static Mode mode = MODE_NORMAL;
static bool bootGesturesDone = false;
static bool waitingRelease = true;
static bool fired = false;
static uint8_t heldKind = 0;
static uint32_t heldSince = 0;
static uint32_t lastBlink = 0;
static bool blinkOn = false;

static void startConfiguration() {
    mode = MODE_CONFIG_QUICK;
    waitingRelease = true;
    blinkOn = true;
    lastBlink = millis();
    blueOn();
    if (debug) Serial.println(F("Config: press the quick-reset combo"));
}

// Short feedback on the blue channel (or on the status LED in the 2-LED build).
static void blueFeedback(uint8_t times) {
    blueBlink(times, 90);
    lastBlink = millis();
    blinkOn = false;
}

static void handleBootGestures() {
    if (countPressed() == COMBO_LEN) {
        uint8_t pressed[COMBO_LEN];
        getPressedCombo(pressed, COMBO_LEN);

        if (isSameCombo(pressed, combos.resetCombo, COMBO_LEN)) {
            startConfiguration();
            bootGesturesDone = true;
            return;
        }
        if (isSameCombo(pressed, combos.longResetCombo, COMBO_LEN)) {
            if (debug) Serial.println(F("Factory reset: restoring default combos"));
            saveDefaultCombos();
            blinkActiveLed();
            blinkActiveLed();
            blinkActiveLed();
            bootGesturesDone = true;
            return;
        }
    }

    if (millis() > BOOT_WINDOW_MS) bootGesturesDone = true;
}

static void handleConfiguration() {
    // Non-blocking blink while we wait for the user.
    if (millis() - lastBlink >= 250) {
        lastBlink = millis();
        blinkOn = !blinkOn;
        if (blinkOn) {
            blueOn();
        } else {
            blueOff();
        }
    }

    // Ignore the combo that started the mode, and require a release between
    // the two steps.
    if (waitingRelease) {
        if (countPressed() == 0) waitingRelease = false;
        return;
    }

    if (countPressed() != COMBO_LEN) return;

    uint8_t pressed[COMBO_LEN];
    getPressedCombo(pressed, COMBO_LEN);

    if (mode == MODE_CONFIG_QUICK) {
        if (saveNewResetCombo(pressed, COMBO_LEN)) {
            mode = MODE_CONFIG_LONG;
            waitingRelease = true;
            if (debug) Serial.println(F("Quick-reset combo stored"));
            blueFeedback(2);
        } else {
            if (debug) Serial.println(F("Rejected: four different buttons, not the long-reset combo"));
            waitingRelease = true;
            blueFeedback(3);
        }
        return;
    }

    if (saveNewLongResetCombo(pressed, COMBO_LEN)) {
        mode = MODE_NORMAL;
        waitingRelease = true;
        blinkOn = true;
        if (debug) Serial.println(F("Long-reset combo stored"));
        blueFeedback(2);
        updateLED();
    } else {
        if (debug) Serial.println(F("Rejected: four different buttons, not the quick-reset combo"));
        waitingRelease = true;
        blueFeedback(3);
    }
}

static void handleInGame() {
    if (countPressed() == 0) {
        fired = false;
        heldKind = 0;
        return;
    }
    if (fired) return;

    uint8_t kind = 0;
    if (isComboPressed(combos.resetCombo, COMBO_LEN)) {
        kind = 1;
    } else if (isComboPressed(combos.longResetCombo, COMBO_LEN)) {
        kind = 2;
    } else {
        heldKind = 0;
        return;
    }

    if (kind != heldKind) {
        heldKind = kind;
        heldSince = millis();
        return;
    }
    if (millis() - heldSince < COMBO_HOLD_MS) return;

    fired = true;
    blueOn();
    if (kind == 1) {
        if (debug) Serial.println(F("Soft reset"));
        triggerReset();
    } else {
        if (debug) Serial.println(F("Long reset"));
        triggerLongReset();
    }
    blueOff();
}
#endif  // HAS_CONTROLLER

// The boot sequence below is a line-by-line port of v1.5.0's setup(). It is the
// sequence that is known to boot on the modded console, so it is kept intact:
//   1. CIC CONF (D8) driven HIGH  -> lockout enabled
//   2. reset helper               -> D4 HIGH 500 ms, then released (high-Z)
//   3. D13 output + status LED    -> red with the lockout enabled
//   4. CIC data (D5) pull-up, controller pins as plain inputs
//   5. 1700 ms settle (= v1.5.0), then the CIC handshake is sampled
void setup() {
    if (debug) Serial.begin(9600);
#if SICKNES_DEBUG
    // Banner with the compiled options (only when debug is enabled in config.h).
    Serial.println(F("sick snes v2.1.0 - config:"));
  #if HAS_CONTROLLER
    Serial.println(F("  pads: ligados (A0/A1/A2)"));
  #else
    Serial.println(F("  pads: DESLIGADOS (sem reset em jogo, sem combos)"));
  #endif
    Serial.println(F("  LEDs: D7=vermelho D6=azul D3=verde (positivos)"));
#endif

    // 1. Lockout chip enabled by default.
    setCicEnabled(true);

    // 2. Reset helper (identical to v1.5.0).
    triggerMediumReset();

    // 3. Onboard LED and status LED.
    pinMode(PIN_DEBUG_LED, OUTPUT);
    ledInit();      // legacy v1.x cathodes (D9/D10) held LOW
    blueOff();
    updateLED();

    // 4. CIC pin 1 is only read once, through the internal pull-up (as in v1.5.0).
    pinMode(PIN_CIC_DATA, INPUT_PULLUP);

#if HAS_CONTROLLER
    // Plain inputs, like v1.5.0. The console drives latch/clock; a floating
    // data line is handled by the bus timeouts in readControllerFrame().
    pinMode(PIN_LATCH, INPUT);
    pinMode(PIN_CLOCK, INPUT);
    pinMode(PIN_DATA,  INPUT);
#endif

    // 5. Let the CIC handshake run and check whether the cartridge passed.
    delay(CIC_CHECK_DELAY_MS);

    handleCICUnlockIfNeeded();

    // "Firmware reached the end of setup" cue: one short green pulse on D3.
    // D3 is not used by v1.5.0, so this pin is a free diagnostic channel.
    if (isCicOn()) greenPulse(BOOT_PULSE_MS);

    if (loadCombos()) {
        // Blank/old EEPROM: the defaults were just written.
        blueBlink(2, 100);
    }
}

void loop() {
#if HAS_CONTROLLER
    if (!readControllerFrame()) return;

    if (!bootGesturesDone) {
        handleBootGestures();
        return;
    }

    if (mode != MODE_NORMAL) {
        handleConfiguration();
        return;
    }

    handleInGame();
#endif
}
