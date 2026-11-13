#include <Arduino.h>
#include "reset.h"
#include "pins.h"

extern bool debug;

// ---------------------------------------------------------------------------
// IMPORTANT: this file is a verbatim port of v1.5.0's reset.cpp.
//
// v1.5.0 never PULLS the /RESET line low at boot: it drives it HIGH for 500 ms
// and then releases it. An earlier v2.0.0 draft replaced that with a real LOW
// pulse, and the console stopped booting on the test board. The reset shape is
// therefore kept exactly as in v1.5.0 until it can be validated on hardware.
// ---------------------------------------------------------------------------

void triggerReset() {
    if (debug) Serial.println(F("Quick resetting console"));
    pinMode(PIN_RESET, OUTPUT);
    digitalWrite(PIN_RESET, HIGH);
    delay(100);
    digitalWrite(PIN_RESET, LOW);
    delay(100);
    pinMode(PIN_RESET, INPUT);
}

void triggerMediumReset() {
    if (debug) Serial.println(F("Medium resetting console"));
    pinMode(PIN_RESET, OUTPUT);
    digitalWrite(PIN_RESET, HIGH);
    delay(500);
    digitalWrite(PIN_RESET, LOW);
    pinMode(PIN_RESET, INPUT);
}

void triggerLongReset() {
    if (debug) Serial.println(F("Long resetting console"));
    pinMode(PIN_RESET, OUTPUT);
    digitalWrite(PIN_RESET, HIGH);
    delay(2000);
    digitalWrite(PIN_RESET, LOW);
    delay(100);
    pinMode(PIN_RESET, INPUT);
}
