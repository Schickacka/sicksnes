#include <Arduino.h>
#include "cic.h"
#include "pins.h"
#include "reset.h"
#include "led.h"

extern bool debug;

// Mirrors what we are driving on the CIC pin 4. Never read the pin back:
// it is an output and its level depends on the CIC's internal pull-down.
static bool cicEnabled = true;

void setCicEnabled(bool enabled) {
    // Write the port before switching to output, so no glitch is generated.
    digitalWrite(PIN_CIC_CONF, enabled ? CIC_CONF_ENABLED : CIC_CONF_DISABLED);
    pinMode(PIN_CIC_CONF, OUTPUT);
    cicEnabled = enabled;
}

bool isCicOn() {
    return cicEnabled;
}

void handleCICUnlockIfNeeded() {
    // D5 reads CIC pin 1 through the internal pull-up. If the cartridge
    // passed the handshake the line settles at CIC_DATA_RUNNING_LEVEL.
    bool running = (digitalRead(PIN_CIC_DATA) == CIC_DATA_RUNNING_LEVEL);

    // We only listen to this line: release it (high-Z, no pull-up).
    pinMode(PIN_CIC_DATA, INPUT);
    digitalWrite(PIN_CIC_DATA, LOW);

    if (running) {
        if (debug) Serial.println(F("CIC handshake OK, keeping lockout on."));
        return;
    }

    if (debug) Serial.println(F("CIC handshake failed, disabling lockout."));
    digitalWrite(PIN_DEBUG_LED, HIGH);
    setCicEnabled(false);
    triggerReset();
    updateLED();
}
