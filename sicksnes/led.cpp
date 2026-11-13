#include <Arduino.h>
#include "led.h"
#include "pins.h"
#include "cic.h"

// Which status colour is currently selected (red = CIC on, green = CIC off).
static bool redActive = false;

// The v1.x wiring switched each LED on its cathode (D9/D10) and fed the anode
// from D6/D7. v2.1.0 drives the anodes only, so holding the cathode returns LOW
// keeps those installs working while a cathode-to-GND LED behaves the same.
void ledInit() {
    digitalWrite(PIN_LED_CATHODE_G, LOW);
    pinMode(PIN_LED_CATHODE_G, OUTPUT);
    digitalWrite(PIN_LED_CATHODE_R, LOW);
    pinMode(PIN_LED_CATHODE_R, OUTPUT);
}

// One pin per colour, all positive: HIGH = on.
static inline void ledWrite(uint8_t pin, bool on) {
    digitalWrite(pin, on ? HIGH : LOW);
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

// One short green flash, then back to the CIC status colour.
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
