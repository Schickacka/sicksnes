#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// User-editable options (pads on/off, debug). The defaults below only apply
// when config.h leaves them commented out.
#include "config.h"

// ---------------------------------------------------------------------------
// Feature flags
// ---------------------------------------------------------------------------
// PADS_OFF / PADS_ON in config.h choose whether the controller port
// (A0/A1/A2) is read. The automatic CIC bypass always works.
#if defined(PADS_ON) && defined(PADS_OFF)
#error "config.h: descomente PADS_ON ou PADS_OFF, nao os dois."
#endif
#if defined(PADS_OFF)
#undef HAS_CONTROLLER
#define HAS_CONTROLLER 0
#elif defined(PADS_ON)
#undef HAS_CONTROLLER
#define HAS_CONTROLLER 1
#elif !defined(HAS_CONTROLLER)
#define HAS_CONTROLLER 1
#endif

// Serial debug output (config.h: SICKNES_DEBUG 1). Off by default.
#ifndef SICKNES_DEBUG
#define SICKNES_DEBUG 0
#endif

// ---------------------------------------------------------------------------
// Pins (Arduino Nano / Uno, ATmega328P)
// ---------------------------------------------------------------------------
// The three LED pins are plain positive outputs: anode to the pin through a
// resistor, cathode to GND. One LED per pin, or one common-cathode RGB LED.
#define PIN_RESET        4    // SNES /RESET   (active LOW, open-drain)
#define PIN_CIC_DATA     5    // CIC pin 1     (data I/O, bidirectional)
#define PIN_LED_BLUE     6    // blue LED      (feedback)
#define PIN_LED_RED      7    // red LED       (lockout state)
#define PIN_CIC_CONF     8    // CIC pin 4     (CONF, must be lifted)
#define PIN_LED_GREEN    3    // green LED     (lockout off + boot pulse)
#define PIN_LED_CATHODE_G 9   // legacy v1.x wiring: green LED cathode return
#define PIN_LED_CATHODE_R 10  // legacy v1.x wiring: red LED cathode return
#define PIN_LATCH       A0    // controller port latch  (PC0)
#define PIN_CLOCK       A1    // controller port clock  (PC1)
#define PIN_DATA        A2    // controller port data   (PC2)
#define PIN_DEBUG_LED   13    // onboard LED

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
#define BOOT_PULSE_MS          80  // green "setup() finished" pulse

#endif
