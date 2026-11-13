#include "controller.h"
#include "pins.h"

int buttonsState[BTN_COUNT];

// A0..A2 are PC0..PC2 on the ATmega328P. The code reads PINC directly so the
// sampling stays inside one clock phase; this only works with these pins.
static_assert(A0 == 14 && A1 == 15 && A2 == 16,
              "controller.cpp reads PINC directly; A0/A1/A2 must be PC0/PC1/PC2");

static inline bool latchLevel() { return (PINC & _BV(PC0)) != 0; }
static inline bool clockLevel() { return (PINC & _BV(PC1)) != 0; }
static inline bool dataLevel()  { return (PINC & _BV(PC2)) != 0; }

static bool waitLatch(bool level) {
    uint32_t start = micros();
    while (latchLevel() != level) {
        if ((uint32_t)(micros() - start) > BUS_TIMEOUT_US) return false;
    }
    return true;
}

static bool waitClock(bool level) {
    uint32_t start = micros();
    while (clockLevel() != level) {
        if ((uint32_t)(micros() - start) > BUS_TIMEOUT_US) return false;
    }
    return true;
}

static void releaseAll() {
    for (int i = 0; i < BTN_COUNT; i++) buttonsState[i] = 1;
}

bool readControllerFrame() {
    // The latch must be idle high first, then go low to start a frame.
    if (!waitLatch(true) || !waitLatch(false)) {
        releaseAll();
        return false;
    }

    for (int i = 0; i < BTN_COUNT; i++) {
        if (!waitClock(true) || !waitClock(false)) {
            releaseAll();
            return false;
        }
        // The line is stable during the clock's low phase. Sampling a couple
        // of microseconds after the falling edge keeps the read away from the
        // transition without leaving the valid window.
        delayMicroseconds(2);
        buttonsState[i] = dataLevel() ? 1 : 0;
    }
    return true;
}

bool isComboPressed(const uint8_t combo[], size_t size) {
    if (size == 0) return false;
    for (size_t i = 0; i < size; i++) {
        if (combo[i] >= BTN_COUNT) return false;
        if (buttonsState[combo[i]] != 0) return false;
    }
    return true;
}

// Order-independent comparison: both arrays hold distinct button ids.
bool isSameCombo(const uint8_t a[], const uint8_t b[], size_t size) {
    for (size_t i = 0; i < size; i++) {
        bool found = false;
        for (size_t j = 0; j < size; j++) {
            if (a[i] == b[j]) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

int countPressed() {
    int pressed = 0;
    for (int i = 0; i < BTN_COUNT; i++) {
        if (buttonsState[i] == 0) pressed++;
    }
    return pressed;
}

int getPressedCombo(uint8_t* outCombo, int maxSize) {
    int count = 0;
    for (int i = 0; i < BTN_COUNT && count < maxSize; i++) {
        if (buttonsState[i] == 0) {
            outCombo[count] = (uint8_t)i;
            count++;
        }
    }
    return count;
}
