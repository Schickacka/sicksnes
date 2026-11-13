#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// User-editable options (LED type, controller wires, debug). The defaults
// below only apply when config.h leaves them commented out.
#include "config.h"

// ---------------------------------------------------------------------------
// Feature flags
// ---------------------------------------------------------------------------
// Set to 0 when the controller port (A0/A1/A2) is NOT wired.
// The automatic CIC bypass still works; in-game reset and combo
// configuration are compiled out.
#ifndef HAS_CONTROLLER
#define HAS_CONTROLLER 1
#endif

// ---------------------------------------------------------------------------
// Pins (Arduino Nano / Uno, ATmega328P)
// ---------------------------------------------------------------------------
#define PIN_RESET        4    // SNES /RESET   (active LOW, open-drain)
#define PIN_CIC_DATA     5    // CIC pin 1     (data I/O, bidirectional)
#define PIN_LED_GREEN    6    // green LED anode
#define PIN_LED_RED      7    // red LED anode
#define PIN_CIC_CONF     8    // CIC pin 4     (CONF, must be lifted)
#define PIN_LED_GND_G    9    // green LED cathode / return
#define PIN_LED_GND_R   10    // red LED cathode / return
#define PIN_LATCH       A0    // controller port latch  (PC0)
#define PIN_CLOCK       A1    // controller port clock  (PC1)
#define PIN_DATA        A2    // controller port data   (PC2)
#define PIN_DEBUG_LED   13    // onboard LED

// ---------------------------------------------------------------------------
// LED options
// ---------------------------------------------------------------------------
// 0 = two discrete LEDs: anode on D6/D7, switched cathode on D9/D10
//     (the wiring described in the README, keeps the stock LED mod).
// 1 = one 4-pin RGB LED, common cathode (or common anode) tied to a rail.
//     Default mapping: red -> D7, blue -> D6, green -> D3.
//     D9/D10 are unused in this mode.
#ifndef HAS_RGB_LED
#define HAS_RGB_LED 0
#endif

// 0 = common cathode (common leg to GND), 1 = common anode (common leg to +5V).
#ifndef RGB_COMMON_ANODE
#define RGB_COMMON_ANODE 0
#endif

// RGB channel pins. D7 (red) and D6 (second status colour) are the same pins
// the v1.x firmware uses, so an old v1.5.0 image can be flashed back on top
// without rewiring. v1.5.0 toggles the anode, so it still blinks with the
// common leg on GND.
// Define RGB_GREEN_ON_D6 to put green on D6 and blue on D3 instead.
#if HAS_RGB_LED && defined(RGB_GREEN_ON_D6)
#undef PIN_LED_GREEN
#define PIN_LED_GREEN    6
#define PIN_LED_BLUE     3
#elif HAS_RGB_LED
#undef PIN_LED_GREEN
#define PIN_LED_GREEN    3
#define PIN_LED_BLUE     6
#endif

// ---------------------------------------------------------------------------
// Electrical conventions
// ---------------------------------------------------------------------------
// CIC pin 4 (CONF): +5V locks the console, GND disables the lockout.
#define CIC_CONF_ENABLED   HIGH
#define CIC_CONF_DISABLED  LOW

// Level seen on CIC pin 1 while the console runs normally.
// Flip this if the automatic detection works backwards on your board.
#define CIC_DATA_RUNNING_LEVEL LOW

// ---------------------------------------------------------------------------
// Timings
// ---------------------------------------------------------------------------
#define BUS_TIMEOUT_US      30000  // give up waiting for latch/clock (us)
#define COMBO_HOLD_MS         400  // combo must be held this long
#define BOOT_WINDOW_MS       4000  // boot gestures are read during this window
// Settle time before sampling the CIC handshake. v1.5.0 waits 1500 ms here,
// plus the 200 ms it spends inside its redLedOn() (the LEDs use a switched
// cathode there), so the sample happens 1700 ms after the reset line returns.
#define CIC_CHECK_DELAY_MS   1700
#define BOOT_RESET_MS         500  // reset held while the Arduino boots

// ---------------------------------------------------------------------------
// Sanity checks for config.h
// ---------------------------------------------------------------------------
// Catches the classic mistake of enabling an RGB-only option while the 2-LED
// build is still selected, instead of silently compiling the wrong thing.
#if defined(RGB_GREEN_ON_D6) && !HAS_RGB_LED
#error "config.h: RGB_GREEN_ON_D6 so vale junto com HAS_RGB_LED 1."
#endif
#if RGB_COMMON_ANODE && !HAS_RGB_LED
#error "config.h: RGB_COMMON_ANODE so vale junto com HAS_RGB_LED 1."
#endif

#endif
