#ifndef LED_H
#define LED_H

// LED model: D7 = red, D6 = blue, D3 = green. Each pin is a positive output
// (HIGH = LED on), so one LED per pin or a single common-cathode RGB LED both
// work. The v1.x switched cathodes on D9/D10 are held LOW by ledInit(), so an
// older two-LED install keeps working without rewiring.

// Sets up the legacy cathode returns (D9/D10). Call once, early in setup().
void ledInit();

void updateLED();
void blinkActiveLed();
void activeLedOn();
void activeLedOff();
bool isRedLEDActive();
void redLedOn();
void greenLedOn();

// Short green pulse used as a "setup finished" marker (D3 is unused by
// v1.5.0, so it is a free diagnostic channel). Restores the CIC status
// colour afterwards.
void greenPulse(uint16_t ms);

// Feedback channel on the blue LED (D6).
void blueOn();
void blueOff();
void blueBlink(uint8_t times, uint16_t ms);

#endif
