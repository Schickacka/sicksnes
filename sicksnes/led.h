#ifndef LED_H
#define LED_H

void updateLED();
void blinkActiveLed();
void activeLedOn();
void activeLedOff();
bool isRedLEDActive();
void redLedOn();
void greenLedOn();

// Short green pulse used as a "setup finished" marker (RGB build only: D3 is
// unused by v1.5.0, so it is a free diagnostic channel). Restores the CIC
// status colour afterwards.
void greenPulse(uint16_t ms);

// Feedback channel: the blue channel on an RGB LED, or the active status LED
// on the two-LED build (so the behaviour stays the same there).
void blueOn();
void blueOff();
void blueBlink(uint8_t times, uint16_t ms);

#endif
