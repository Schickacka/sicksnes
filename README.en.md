# 🎮 sick snes — v2.0.0

![sick snes logo](images/logo.jpg)

🇧🇷 **Português:** [README.md](README.md)

**Automatic lockout chip (CIC) toggle and in-game reset for the Super Nintendo.**

If a cartridge fails the CIC handshake, the mod disables the lockout chip and
resets the console automatically, so imports, homebrew and flashcarts boot
without any switch. On top of that it gives you soft/long reset from the
controller, with no case mod and no extra button.

---

## 🙌 Credits — read this first

**This is a fork. The original project, the hardware research and the entire
v1.x firmware are by [@lucaslealdev](https://github.com/lucaslealdev).**

> **Original repository: [github.com/lucaslealdev/sicksnes](https://github.com/lucaslealdev/sicksnes)**
> YouTube: [Pixel que Byte](https://www.youtube.com/@PixelqueByte)

Everything that actually makes this mod work — lifting CIC pin 4, tapping the
reset line, reading the controller bus, the LED scheme, the in-game reset idea —
was designed, built and validated by him (with a nod to
[JeffWDH/NES-In-Game-Reset](https://github.com/JeffWDH/NES-In-Game-Reset) for
the reset idea). v2.0.0 is a maintenance release of that work: it keeps the
proven v1.5.0 behaviour on the boot path and adds an optional single RGB LED
plus a handful of robustness fixes.

License: **GPL-3.0**, inherited from upstream. See [LICENSE](LICENSE).

---

## 📚 Table of contents

1. [What's new in v2.0.0](#-whats-new-in-v200)
2. [Which firmware should I flash?](#-which-firmware-should-i-flash)
3. [Wiring](#-wiring)
4. [The LEDs](#-the-leds)
5. [Controls](#️-controls)
6. [Configuration mode](#️-configuration-mode)
7. [SD2SNES / FXPak compatibility](#-sd2snes--fxpak-compatibility)
8. [Installing without the controller wires](#-installing-without-the-controller-wires)
9. [Flashing](#-flashing)
10. [Building from source](#️-building-from-source)
11. [Compatibility](#-compatibility)
12. [Technical notes](#-technical-notes)
13. [Troubleshooting](#-troubleshooting)
14. [Changelog](#-changelog)

---

## ✨ What's new in v2.0.0

| | |
|---|---|
| 🌈 **Single RGB LED** | One 4-pin RGB LED replaces the two LEDs: red = lockout on, green = lockout off, blue = feedback. `D6` and `D7` stay where v1.x put them, so **v1.5.0 can be flashed back without rewiring**. |
| 🧭 **Boot path is v1.5.0 verbatim** | `setup()` order, reset helper, CIC pull-up and sample time are identical to the image known to boot. `reset.cpp` is a line-by-line port. |
| 🎮 **Controller pins actually fixed** | v1.x did `#define latch 19 / clock 20 / Sdata 21`, which configures **A5/A6/A7** on a Nano, while the code read **A0/A1/A2**. v2.0.0 uses the real `A0/A1/A2`. |
| ⏱️ **Bus timeouts** | A missing controller no longer hangs the loop; a failed read releases all buttons. |
| ⏳ **Combo hold time** | A combo must be held for ~400 ms, so one glitched frame never resets the console. |
| 💾 **Validated EEPROM at a new address** | Magic + version + checksum + range checks, stored at **address 64** so v1.5.0's combos (address 0) survive a round trip. |
| 🕹️ **New default combos** | `L+R+X+Y` / `L+R+A+B` — they do not collide with any SD2SNES/FXPak shortcut. |
| 💡 **Green "alive" pulse** | On the RGB build, one 80 ms green pulse on `D3` at the end of `setup()` — a free diagnostic channel that v1.5.0 never touches. |
| 📖 **Docs** | This README, wiring tables, boot legend and the Portuguese version ([README.md](README.md)). |

---

## 🧭 Which firmware should I flash?

Two independent things decide the firmware: **how the LEDs are wired** and
**whether `A0`/`A1`/`A2` are soldered**.

| Firmware | LED | Controller | Flash this when |
|---|---|---|---|
| `firmware/sicksnes-v2.0.0-rgb-controller.hex` | RGB | yes | You added a single RGB LED **and** wired `A0/A1/A2`. Most complete build. |
| `firmware/sicksnes-v2.0.0-rgb-nocontroller.hex` | RGB | no | You added a single RGB LED but did **not** wire `A0/A1/A2`. CIC bypass + LEDs only. |
| `firmware/sicksnes-v2.0.0-2led-controller.hex` | 2 LEDs | yes | Stock v1.x LED wiring + controller wires. The classic setup. |
| `firmware/sicksnes-v2.0.0-2led-nocontroller.hex` | 2 LEDs | no | Stock v1.x LED wiring, no controller wires. Smallest image. |

### Why do these four options exist?

- **"With controller" vs "without controller".** The CIC bypass only needs
  `D4`, `D5` and `D8`. Plenty of installs stop there and never solder the three
  controller taps, because the mod works fine without them — you just lose
  in-game reset and combo configuration. v1.x was *not* safe in that case: the
  pins floated with no timeout, so the firmware could hang or read phantom
  buttons. v2.0.0 fixes the firmware **and** ships a build with the controller
  code compiled out (`HAS_CONTROLLER=0`): no polling, no interrupts, no chance
  of phantom input, and ~1.4 KB less flash.
- **"RGB" vs "2 LEDs".** The 2-LED build is the original scheme (anode on
  `D6`/`D7`, switched cathode on `D9`/`D10`). The RGB build drives one pin per
  colour and uses `D6`/`D7` for the same two colours v1.x used, plus `D3` for
  green. That specific mapping is what lets you **flash v1.5.0 back on top
  without rewiring**: v1.5.0 shows solid red/blue on that LED and simply ignores
  `D3`.

You can always flash the wrong pairing — the pins are inputs, nothing is
damaged — you just get a feature you cannot use, or feedback on a channel you
did not wire.

| Build | Flash used | File |
|---|---|---|
| RGB + controller | 4408 B | 12.417 B |
| RGB, no controller | 3054 B | 8.604 B |
| 2 LEDs + controller | 4400 B | 12.388 B |
| 2 LEDs, no controller | 3014 B | 8.498 B |

---

## 🔌 Wiring

### Main harness

| Arduino pin | Signal | SNES connection |
|---|---|---|
| `5V` | power | Board VCC |
| `GND` | power | Board GND |
| `D4` | `/RESET` (active LOW) | Reset button / reset test point |
| `D5` | CIC data | **CIC pin 1** |
| `D8` | CIC `CONF` | **CIC pin 4** (must be lifted) |
| `D6` | LED, 2nd colour | Blue LED (RGB) / green LED (2-LED) anode |
| `D7` | LED, red | Red LED anode |
| `D3` | LED, green | Green LED anode (**RGB build only**) |
| `D9` | cathode return | Green LED cathode (**2-LED build only**) |
| `D10` | cathode return | Red LED cathode (**2-LED build only**) |
| `A0` | controller **latch** | Controller port pin 10 |
| `A1` | controller **clock** | Controller port pin 8 |
| `A2` | controller **data** | Controller port pin 6 |

> **CIC pin 4 must be lifted** (disconnected from the board). It is `CONF`, not
> ground: `+5 V` = lockout enabled, `GND` = lockout disabled (the chip has an
> internal pull-down). v2.0.0 drives it, so the mod can toggle it at runtime —
> unlike the classic "cut and ground" mod.

### ↻ Reset button (viewed from the PCB bottom)

```
         __________
        |  •    •  |
        |          |
        |          |
        | 5V    D4 |
         ‾‾‾‾‾‾‾‾‾‾
```

### 𓇲 CIC F411X / F413X

```
     9 ______________________________ 1
      |  •  •  •  •  •  D8  •  •  D5  |
      |                               |
      |                               |
      |                               |
      |                               |
      |                               |
      |                               |
      |                               |
      |  •  •  •  •  •   •   •  •  •  |
    10 ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾ 18
```

`D5` → CIC pin 1 (data), `D8` → CIC pin 4 (`CONF`, lifted).

### 🎮 SNES controller port (viewed from the PCB bottom)

```
    10 _____________________________ 2
      |   A0   A1   A2   •    •     |
      |                             |
      | •    •    •    •    •    •  |
    11 ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾ 1
```

`A0` = latch, `A1` = clock, `A2` = data. The Arduino **only reads** these
lines; it never drives them, so it cannot fight the console, a multitap or a
flashcart.

---

## 💡 The LEDs

### Two-LED build (v1.x wiring)

Red = lockout enabled, green = lockout disabled (blinking). The stock red LED
can be reused: disconnect the 5 V side of its resistor and feed the resistor
from both `D6` and `D7`.

![how to wire the stock RGB LED](images/stock-led.png)

The cathodes go back to the Arduino on `D9`/`D10`, which is how each LED is
switched fully off — that is why the LED blinks instead of just changing colour.

### Single RGB LED (new in v2.0.0)

Use a **common-cathode** 4-pin RGB LED (R/G/B positive legs + one `GND`):

- red → resistor → `D7`
- blue → resistor → `D6`
- green → resistor → `D3`
- common leg → `GND`
- `D9`/`D10` are unused; leave them disconnected.

One resistor per colour leg (330 Ω for red, 220 Ω for green/blue is a good
starting point). **Do not** put a resistor on the common leg.

| Colour | Meaning |
|---|---|
| 🔴 Red | Lockout enabled (normal operation) |
| 🟢 Green | Lockout disabled (CIC bypass kicked in) |
| 🔵 Blue | Feedback overlay: blinks in configuration mode, 2 blinks = combos stored / EEPROM restored, 3 blinks = combo rejected, lit during the in-game reset pulse |

Blue is an **overlay**: it can be on together with red or green (so it looks
magenta/cyan) and never hides the lockout state.

**Boot legend (it is a diagnostic):**

1. **Red** turns on — same as v1.5.0 (lockout enabled).
2. **One short green pulse (~80 ms) on `D3`** — only v2.0.0 does this. If you
   see it, the Nano ran the new `setup()` to the end. If the console stays black
   *and* the pulse appears, the firmware is not the problem.
3. **Blue blinks 2×** — fresh/blank EEPROM, default combos written (first boot
   only).
4. **Red steady** — CIC handshake OK. **Green steady** — handshake failed, the
   lockout was disabled and the console was reset.

**Why blue on `D6` and green on `D3`:** v1.5.0 uses `D7` for red and `D6` for
its second status colour. Keeping blue on `D6` means a v1.5.0 image flashed back
on top still blinks red/blue on this LED. `D3` is unused by v1.5.0, so the green
channel is simply ignored there. Prefer green on `D6`? Compile with
`-DRGB_GREEN_ON_D6` (then blue moves to `D3`).

---

## 🕹️ Controls

| Action | Combo |
|---|---|
| 🔄 **Soft reset** | `L + R + X + Y` (hold ~0.4 s) |
| 🔄 **Long reset** | `L + R + A + B` (hold ~0.4 s) |
| ⚙️ **Configuration mode** | hold `L + R + X + Y` while powering on |
| ♻️ **Restore default combos** | hold `L + R + A + B` while powering on |

The ~400 ms hold means a single accidental frame never resets the console.

---

## ⚙️ Configuration mode

1. Power on the console while holding `L + R + X + Y` — the blue channel starts
   blinking.
2. While it blinks, press the 4-button combo you want for **soft reset** →
   blue blinks **2×** (stored) or **3×** (rejected).
3. Blue blinks again; press the combo for **long reset** → 2 blinks = stored.
4. Both combos must be different and all four buttons must be distinct. The new
   combo cannot be the other one.
5. Power-cycle to leave configuration mode (there is no automatic exit).

*To restore the defaults, hold `L + R + A + B` while powering on.*

---

## 🎮 SD2SNES / FXPak compatibility

The mod only **listens** to the controller bus; the console drives latch and
clock. There is no electrical interference with a flashcart, a multitap or a
second controller. The only possible interference is logical: two devices
reacting to the same buttons.

The v2.0.0 defaults were chosen to avoid every documented flashcart shortcut:

| SD2SNES / FXPak shortcut | Combo | Collides? |
|---|---|---|
| Reset game | `L + R + Select + Start` | No |
| Reset to menu | `L + R + Select + X` | No |
| In-game menu (sd2snes+ fork) | `L + R + Y + ←` | No |
| Save state | `Start + R` | No |
| Load state | `Start + L` | No |
| Savestate slot | `Select + D-pad` | No |
| Cheats on/off | `L + R + Start + A/B` | No |
| Hooks off (10 s / permanent) | `L + R + Start + X/Y` | No |

**Do not go back to the v1.x defaults** (`Start + L + R + Left/Down`): they
contain `Start + L` and `Start + R`, which are the flashcart **load state** and
**save state** shortcuts — every soft reset would also hit the savestate.

> While the SD2SNES in-game menu is open the game is paused, but the Arduino
> still sees the real pad. Avoid pressing the sick snes combos inside that menu.

---

## 🚫 Installing without the controller wires

The CIC bypass runs in `setup()` and only uses `D4`, `D5` and `D8`. Without
`A0/A1/A2` you lose in-game reset and combo configuration — nothing else.

Two options:

1. **Flash the `-nocontroller` build** (recommended). The controller code is
   not compiled at all: no bus polling, no timeouts, no phantom input.
2. **Flash the normal build and just don't wire them.** Safe in v2.0.0 (plain
   inputs + timeouts), but the firmware still wakes up once per frame to check
   the bus.

> v1.x was **not** safe in this scenario: floating pins with no timeout could
> hang the firmware or produce random buttons.

---

## 💻 Flashing

Prebuilt images are in [`firmware/`](firmware/): pick the one that matches your
wiring (see [the table above](#-which-firmware-should-i-flash)). Flash the
**application** hex, leave the fuses alone, and do not flash an `.eep` file.

### 💽 Via USB (Arduino bootloader)

1. Connect the Nano/Uno over USB.
2. Upload the sketch (Arduino IDE) or the `.hex` (AVRDUDESS with programmer
   `Arduino`, 115200 baud).
3. With the stock bootloader, `D8` floats for ~1.5–2 s while the board boots.
   With the recommended 10 kΩ pull-up on CIC pin 4 the lockout stays enabled
   during that window; without it the console boots with the lockout disabled
   and a few games (notably *Street Fighter Alpha 2*) may misbehave.

### 🧰 Recommended: USBasp (no bootloader, faster startup)

1. Install the driver with [Zadig](https://zadig.akeo.ie/): `Options > List All
   Devices`, pick `USBasp`, choose `libusb-win32` (or `libusbK`), `Install
   Driver`.
2. Open **Extreme Burner - AVR**, select **ATmega328P**, `File > Open Flash`,
   load the `.hex`, then `Write All`.

> ⚠️ A "signature mismatch" warning is safe to ignore for an ATmega328PB (it has
> a different signature from the 328P but is compatible for this mod).

### ⚡ Practical rule for an installed mod

Never have two 5 V sources on the Arduino at the same time:

- **Console powered, Arduino powered by the console:** disconnect only the 5 V
  wire coming from the SNES before plugging the programmer, keep `GND` and all
  signals.
- **Console off:** power the Arduino from the programmer instead (a diode on the
  5 V line, band towards the Arduino, keeps the programmer from feeding the
  console).

If the Arduino is unpowered while wired to the console, its protection diodes
can clamp the SNES `/RESET` line and the console will look dead (power LED on,
no video). It is not damage — restore the Arduino's 5 V and it boots again.

---

## 🛠️ Building from source

```bash
arduino-cli compile --fqbn arduino:avr:nano ./sicksnes
```

Build flags (all optional, defined in `sicksnes/pins.h` or passed with
`--build-property "compiler.cpp.extra_flags=-D..."`):

| Flag | Default | Effect |
|---|---|---|
| `HAS_CONTROLLER` | `1` | `0` compiles the controller/combos out (no `A0/A1/A2`). |
| `HAS_RGB_LED` | `0` | `1` uses one RGB LED instead of two discrete LEDs. |
| `RGB_COMMON_ANODE` | `0` | `1` for a common-anode RGB LED (common leg to +5 V). |
| `RGB_GREEN_ON_D6` | off | Swaps green/blue: green on `D6`, blue on `D3`. |

Examples:

```bash
# RGB LED + controller
arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DHAS_RGB_LED=1" ./sicksnes

# RGB LED, no controller
arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DHAS_CONTROLLER=0 -DHAS_RGB_LED=1" ./sicksnes
```

---

## 🔗 Compatibility

| SNES model | With bootloader (USB) | No bootloader (USBasp) |
|---|---|---|
| SNS-CPU-1CHIP-02 | ✅ Full | ✅ Full |
| SNS-CPU-1CHIP-03 | ⚠️ Mitigated (see SFA2 note) | ✅ Full |
| SNS-CPU-GPM-02 | not tested | ✅ Full |
| SNN-CPU-01 (baby jr) | not tested | ✅ Full |

Every other SNES board should be compatible.

### ⚠️ *Street Fighter Alpha 2*

Older firmware could fail to boot SFA2 when the Arduino was flashed through the
default bootloader, because `D8` floats during the bootloader: the console boots
with the lockout disabled and the Arduino enables it afterwards.

v2.0.0 mitigates this with the recommended 10 kΩ pull-up on CIC pin 4 (keeps
`CONF` enabled while the Arduino boots). The old workaround — hold the **reset
button** until the Arduino initializes (~1.8 s after power-up) — still works.

### ⚠️ Regional lockout is not region conversion

Disabling the CIC only removes the **lockout** check. It does not change the
video mode, the 50/60 Hz timing or the `PALMODE` pins. PAL games on an NTSC
console (or the other way around) may still fail to boot or run at the wrong
speed. On SHVC-CPU-01 boards this needs additional region/50 Hz work, which is
outside the scope of this mod.

### ⚠️ SA-1 games and a disabled lockout

A few SA-1 titles (*Super Mario RPG*, *Kirby Super Star*) check the CIC state
and may refuse to boot when the lockout is disabled. Carts that pass the
handshake keep the lockout enabled, so the mod does not affect them; this only
matters if the CIC was disabled for another reason.

---

## 🧪 Technical notes

### The boot path is v1.5.0, on purpose

`reset.cpp` is a **line-by-line port of v1.5.0**: at boot it drives `D4` **HIGH**
for 500 ms and then releases the pin (high-Z). It does **not** pull `/RESET` low.

During v2.0.0 development the reset helper was "improved" into a real 10 ms LOW
pulse. The console stopped booting on the test unit. The community lesson is
simple: **on this mod the reset shape is part of the boot contract** — the SNES
CIC handshake happens right after power-on, and poking `/RESET` while it is
negotiating can leave the CIC holding the console in reset. So the proven
sequence stayed.

Timing is matched too: v1.5.0 samples the CIC handshake **1700 ms** after the
reset line returns (1500 ms `delay` + 200 ms spent inside `redLedOn()`), and
v2.0.0 uses `CIC_CHECK_DELAY_MS = 1700` for the same sample point.

### How the CIC check works

`D5` reads CIC pin 1 through the internal pull-up. After the settle time the
line is sampled once:

- at `CIC_DATA_RUNNING_LEVEL` (`LOW`) → the handshake succeeded, keep the
  lockout enabled (red);
- otherwise → disable the lockout (`D8` LOW), reset the console, show green.

The level is empirical (it is what v1.5.0 uses on real hardware). If your board
behaves backwards, flip `CIC_DATA_RUNNING_LEVEL` in `pins.h`.

### EEPROM

Combos live at **address 64** in v2.0.0, with magic + version + checksum + range
and duplicate validation. v1.5.0 uses address 0, so:

- flashing v2.0.0 does not destroy the v1.5.0 combos, and
- flashing v1.5.0 back restores them untouched (no garbage, no phantom combos).

### Controller pins

v1.x defined `latch 19 / clock 20 / Sdata 21`, which on a Nano are **A5/A6/A7**,
while `PINC` bit reads used **PC0/PC1/PC2** (`A0/A1/A2`). It worked only because
all six pins default to inputs after reset. v2.0.0 configures the real
`A0/A1/A2` as plain inputs and reads `PINC` directly inside one clock phase.

---

## 🩺 Troubleshooting

| Symptom | Check |
|---|---|
| Console dead (power LED on, no video) | Is the Arduino powered? An unpowered Nano clamps `/RESET` through its protection diodes. Reconnect the 5 V (or remove the `D4` wire to confirm). |
| RGB build: no green pulse at boot | The Nano did not reach the end of `setup()`. Check the flash and the 5 V. |
| Green LED steady, console resets in a loop | The CIC handshake is being read as failed. Confirm `D8` → CIC pin 4 is really lifted. |
| Blue never lights | The only "always visible" blue is the EEPROM blink on first boot and the config mode — blue is feedback, not a status light. |
| Combos do nothing | `A0/A1/A2` wired? Did you flash a `-nocontroller` build? |
| In-game reset also hits the flashcart | You are using v1.x combos (`Start+L/R`). Restore the defaults (`L+R+A+B` at power-on). |
| `avrdude: Device signature = 00 00 00` / `FF FF FF` | ISP wiring or power, not the hex. With **Arduino as ISP**: `D10` → target `RESET` (ICSP pin 5, **not** the Uno's own ICSP pin 5), `D11/D12/D13`, `GND`, one 5 V source only, and a 10 µF cap between `RESET` and `GND` **on the programmer**. Never use `-F`. |

---

## 📝 Changelog

### v2.0.0
- **Boot path is a line-by-line port of v1.5.0** — `setup()` order, reset helper
  (`D4` HIGH 500 ms → release), `D5` pull-up, plain-input controller pins and the
  1700 ms CIC sample point. `reset.cpp` is verbatim v1.5.0.
- **Optional single RGB LED** (`-DHAS_RGB_LED=1`): red/green keep the CIC status,
  blue adds feedback. `D7` = red, `D6` = blue, `D3` = green, so v1.5.0 can be
  flashed back without rewiring.
- **Green "alive" pulse** on `D3` at the end of `setup()` (RGB build only).
- **Controller pins fixed**: real `A0/A1/A2` instead of `A5/A6/A7`.
- **Bus timeouts** in `readControllerFrame()` — no more hanging on a missing pad.
- **~400 ms combo hold** before acting.
- **EEPROM validation** (magic/version/checksum/range/duplicates) at address 64,
  independent from v1.5.0's address 0.
- **New default combos** (`L+R+X+Y`, `L+R+A+B`) that avoid SD2SNES/FXPak shortcuts.
- **Config/factory-reset gestures** moved from `Start`/`Select` to the reset
  combos, so they no longer launch a game or open the cheat list on a flashcart.
- **Non-blocking LED blink** during configuration; the loop is frame-paced.
- **`CicGndPin` renamed** to `PIN_CIC_DATA` (it is CIC pin 1, not ground) and the
  CIC state is kept in a variable instead of read back from an output.
- **Single `pins.h`** with every pin, flag and timing.
- **Documentation**: this README, the wiring tables, the boot legend and
  [README.md](README.md).

### v1.5.0 and earlier
See the original repository: [lucaslealdev/sicksnes](https://github.com/lucaslealdev/sicksnes).

---

## ✨ Credits

- **[@lucaslealdev](https://github.com/lucaslealdev)** — original project,
  hardware mod, v1.x firmware, LED scheme, documentation, videos.
  ⭐ [github.com/lucaslealdev/sicksnes](https://github.com/lucaslealdev/sicksnes)
- **[JeffWDH/NES-In-Game-Reset](https://github.com/JeffWDH/NES-In-Game-Reset)** —
  the reset approach that inspired this mod.
- [Pixel que Byte](https://www.youtube.com/@PixelqueByte) — project videos.
- v2.0.0 maintenance (RGB LED, boot-path restoration, robustness fixes, docs):
  this fork.

The mod is provided as-is, with no guarantees. Modify and distribute at your own
risk — and under **GPL-3.0**, like the original.
