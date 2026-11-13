#include <string.h>

#include "combo.h"
#include "controller.h"

extern bool debug;

// v1.5.0 stores its combos at address 0. v2.0.0 uses a different address so
// that flashing back to v1.5.0 restores the old combos untouched (and v1.5.0
// never reads garbage written by v2.0.0).
static const int      EEPROM_ADDR    = 64;
static const uint16_t COMBO_MAGIC    = 0x534E;  // "SN"
static const uint8_t  COMBO_VERSION  = 2;

// Safe defaults. They deliberately avoid Start and Select, so they never
// collide with the SD2SNES/FXPak shortcuts:
//   Start+L / Start+R      -> load / save state
//   Select+D-pad           -> save state slot
//   L+R+Select+Start / +X  -> reset game / reset to menu
//   L+R+Y+Left             -> sd2snes+ in-game menu
static const uint8_t DEFAULT_RESET[COMBO_LEN]      = { BTN_L, BTN_R, BTN_X, BTN_Y };
static const uint8_t DEFAULT_LONG_RESET[COMBO_LEN] = { BTN_L, BTN_R, BTN_A, BTN_B };

ButtonCombos combos;

static uint16_t computeChecksum(ButtonCombos c) {
    c.checksum = 0;
    const uint8_t* raw = reinterpret_cast<const uint8_t*>(&c);
    uint16_t sum = 0x5A5A;
    for (size_t i = 0; i < sizeof(c); i++) {
        sum = (uint16_t)((sum << 5) - sum + raw[i]);  // sum * 31 + byte
    }
    return sum;
}

bool comboValid(const uint8_t* combo, size_t size) {
    if (size != COMBO_LEN) return false;
    for (size_t i = 0; i < size; i++) {
        if (combo[i] >= BTN_COUNT) return false;
        for (size_t j = i + 1; j < size; j++) {
            if (combo[i] == combo[j]) return false;
        }
    }
    return true;
}

void saveDefaultCombos() {
    combos.magic    = COMBO_MAGIC;
    combos.version  = COMBO_VERSION;
    memcpy(combos.resetCombo, DEFAULT_RESET, COMBO_LEN);
    memcpy(combos.longResetCombo, DEFAULT_LONG_RESET, COMBO_LEN);
    combos.checksum = computeChecksum(combos);
    EEPROM.put(EEPROM_ADDR, combos);

    if (debug) Serial.println(F("Default combos stored."));
}

bool loadCombos() {
    EEPROM.get(EEPROM_ADDR, combos);

    bool valid = combos.magic == COMBO_MAGIC &&
                 combos.version == COMBO_VERSION &&
                 comboValid(combos.resetCombo, COMBO_LEN) &&
                 comboValid(combos.longResetCombo, COMBO_LEN) &&
                 !isSameCombo(combos.resetCombo, combos.longResetCombo, COMBO_LEN);

    if (valid) {
        // The checksum is checked after the range checks so a corrupted
        // struct never reaches isComboPressed().
        valid = combos.checksum == computeChecksum(combos);
    }

    if (!valid) {
        if (debug) Serial.println(F("Invalid EEPROM, restoring defaults."));
        saveDefaultCombos();
        return true;
    }
    return false;
}

static bool storeCombo() {
    combos.checksum = computeChecksum(combos);
    EEPROM.put(EEPROM_ADDR, combos);
    return true;
}

bool saveNewResetCombo(const uint8_t* newCombo, size_t size) {
    if (!comboValid(newCombo, size)) return false;
    if (isSameCombo(newCombo, combos.longResetCombo, COMBO_LEN)) return false;

    memcpy(combos.resetCombo, newCombo, COMBO_LEN);
    return storeCombo();
}

bool saveNewLongResetCombo(const uint8_t* newCombo, size_t size) {
    if (!comboValid(newCombo, size)) return false;
    if (isSameCombo(newCombo, combos.resetCombo, COMBO_LEN)) return false;

    memcpy(combos.longResetCombo, newCombo, COMBO_LEN);
    return storeCombo();
}
