#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include "buttons.h"

// 0 = pressed, 1 = released (the SNES data line is active LOW).
extern int buttonsState[BTN_COUNT];

// Reads one full frame from the controller bus.
// Returns false on timeout; in that case every button is marked released.
bool readControllerFrame();

bool isComboPressed(const uint8_t combo[], size_t size);
bool isSameCombo(const uint8_t a[], const uint8_t b[], size_t size);

int countPressed();
int getPressedCombo(uint8_t* outCombo, int maxSize);

#endif
