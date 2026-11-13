#ifndef COMBO_H
#define COMBO_H

#include <Arduino.h>
#include <EEPROM.h>
#include "buttons.h"

#define COMBO_LEN 4

struct ButtonCombos {
    uint16_t magic;
    uint16_t checksum;
    uint8_t  version;
    uint8_t  resetCombo[COMBO_LEN];
    uint8_t  longResetCombo[COMBO_LEN];
};

extern ButtonCombos combos;

// Returns true when the stored data was invalid and the defaults were written.
bool loadCombos();
void saveDefaultCombos();
bool saveNewResetCombo(const uint8_t* newCombo, size_t size);
bool saveNewLongResetCombo(const uint8_t* newCombo, size_t size);
bool comboValid(const uint8_t* combo, size_t size);

#endif
