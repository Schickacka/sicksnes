#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

// SNES joypad shift order: B, Y, Select, Start, Up, Down, Left, Right, A, X, L, R.
enum : uint8_t {
    BTN_B = 0,
    BTN_Y,
    BTN_SELECT,
    BTN_START,
    BTN_UP,
    BTN_DOWN,
    BTN_LEFT,
    BTN_RIGHT,
    BTN_A,
    BTN_X,
    BTN_L,
    BTN_R,
    BTN_COUNT
};

#endif
