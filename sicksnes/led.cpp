#include <Arduino.h>
#include "led.h"
#include "pins.h"
#include "cic.h"

// Which status colour is currently selected (red = CIC on, green = CIC off).
static bool redActive = false;

#if HAS_RGB_LED
// Guard the documented mapping so a future edit cannot silently break the
// "flash v1.5.0 back without rewiring" property (D7 = red, D6 = status 2).
#if defined(RGB_GREEN_ON_D6)
static_assert(PIN_LED_GREEN == 6 && PIN_LED_BLUE == 3,
              "RGB_GREEN_ON_D6 expects green on D6 and blue on D3");
#else
static_assert(PIN_LED_RED == 7 && PIN_LED_BLUE == 6 && PIN_LED_GREEN == 3,
              "Default RGB mapping must be D7=red, D6=blue, D3=green");
#endif

// One pin per colour, no switched cathode: HIGH = on (common cathode),
// LOW = on (common anode).
static inline void ledWrite(uint8_t pin, bool on) {
#if RGB_COMMON_ANODE
    digitalWrite(pin, on ? LOW : HIGH);
#else
    digitalWrite(pin, on ? HIGH : LOW);
#endif
    pinMode(pin, OUTPUT);
}

void greenLedOn() {
    ledWrite(PIN_LED_RED, false);
    ledWrite(PIN_LED_GREEN, true);
    redActive = false;
}

void redLedOn() {
    ledWrite(PIN_LED_GREEN, false);
    ledWrite(PIN_LED_RED, true);
    redActive = true;
}

void activeLedOn() {
    ledWrite(redActive ? PIN_LED_RED : PIN_LED_GREEN, true);
}

void activeLedOff() {
    ledWrite(redActive ? PIN_LED_RED : PIN_LED_GREEN, false);
}

void blueOn() {
    ledWrite(PIN_LED_BLUE, true);
}

void blueOff() {
    ledWrite(PIN_LED_BLUE, false);
}

#else  // two discrete LEDs: anode on D6/D7, switched cathode on D9/D10

void greenLedOn() {
    digitalWrite(PIN_LED_RED, LOW);
    pinMode(PIN_LED_RED, INPUT);
    pinMode(PIN_LED_GND_R, INPUT);

    digitalWrite(PIN_LED_GREEN, HIGH);
    pinMode(PIN_LED_GREEN, OUTPUT);
    digitalWrite(PIN_LED_GND_G, LOW);
    pinMode(PIN_LED_GND_G, OUTPUT);

    redActive = false;
}

void redLedOn() {
    digitalWrite(PIN_LED_GREEN, LOW);
    pinMode(PIN_LED_GREEN, INPUT);
    pinMode(PIN_LED_GND_G, INPUT);

    digitalWrite(PIN_LED_RED, HIGH);
    pinMode(PIN_LED_RED, OUTPUT);
    digitalWrite(PIN_LED_GND_R, LOW);
    pinMode(PIN_LED_GND_R, OUTPUT);

    redActive = true;
}

void activeLedOn() {
    if (redActive) {
        digitalWrite(PIN_LED_GND_R, LOW);
        pinMode(PIN_LED_GND_R, OUTPUT);
    } else {
        digitalWrite(PIN_LED_GND_G, LOW);
        pinMode(PIN_LED_GND_G, OUTPUT);
    }
}

void activeLedOff() {
    if (redActive) {
        pinMode(PIN_LED_GND_R, INPUT);
    } else {
        pinMode(PIN_LED_GND_G, INPUT);
    }
}

// No dedicated blue channel: reuse the active status LED for feedback.
void blueOn()  { activeLedOn(); }
void blueOff() { activeLedOff(); }

#endif

// One short green flash, then back to the CIC status colour. Used as a
// "setup() finished" marker on the RGB build (D3 is free in v1.5.0).
void greenPulse(uint16_t ms) {
    bool wasRed = redActive;
    greenLedOn();
    delay(ms);
    if (wasRed) redLedOn();  // otherwise green was already the status colour
}

void blueBlink(uint8_t times, uint16_t ms) {
    for (uint8_t i = 0; i < times; i++) {
        blueOn();
        delay(ms);
        blueOff();
        delay(ms);
    }
}

bool isRedLEDActive() {
    return redActive;
}

void blinkActiveLed() {
    activeLedOff();
    delay(300);
    activeLedOn();
    delay(300);
}

void updateLED() {
    if (isCicOn()) {
        redLedOn();
    } else {
        greenLedOn();
        blinkActiveLed();
        blinkActiveLed();
        blinkActiveLed();
    }
}
