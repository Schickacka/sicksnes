# 🎮 sick snes — v2.1.0

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
the reset idea). v2.x is a maintenance release of that work: it keeps the
proven v1.5.0 behaviour on the boot path, fixes a handful of robustness details
and puts the whole configuration in a single file.

License: **GPL-3.0**, inherited from upstream. See [LICENSE](LICENSE).

---

## 📚 Table of contents

1. [What's new in v2.1.0](#-whats-new-in-v210)
2. [Which firmware should I flash?](#-which-firmware-should-i-flash)
3. [Wiring](#-wiring)
4. [The LEDs](#-the-leds)
5. [Controls](#️-controls)
6. [Configuration mode](#️-configuration-mode)
7. [SD2SNES / FXPak compatibility](#-sd2snes--fxpak-compatibility)
8. [Installing without the pads](#-installing-without-the-pads)
9. [Flashing](#-flashing)
10. [Configuring and building](#️-configuring-and-building)
11. [Compatibility](#-compatibility)
12. [Technical notes](#-technical-notes)
13. [Troubleshooting](#-troubleshooting)
14. [Changelog](#-changelog)

---

## ✨ What's new in v2.1.0

| | |
|---|---|
| 💡 **Three LEDs, three pins** | `D7` = red (lockout on), `D6` = blue (feedback), `D3` = green (lockout off + boot pulse). They are plain **positive** outputs: anode to the pin, cathode to GND. One LED per pin or one common-cathode RGB LED. |
| 🧭 **Boot path is v1.5.0 verbatim** | `setup()` order, reset helper, CIC pull-up and sample time are identical to the image known to boot. `reset.cpp` is a line-by-line port. |
| 🎛️ **`config.h`, PsNee style** | One file, commented lines: *uncomment this to turn the pads off*, *uncomment this to turn them on*. No flags, no code edits. |
| 🎮 **Controller pins actually fixed** | v1.x did `#define latch 19 / clock 20 / Sdata 21`, which configures **A5/A6/A7** on a Nano, while the code read **A0/A1/A2**. v2.x uses the real `A0/A1/A2`. |
| ⏱️ **Bus timeouts** | A missing controller no longer hangs the loop; a failed read releases all buttons. |
| ⏳ **Combo hold time** | A combo must be held for ~400 ms, so one glitched frame never resets the console. |
| 💾 **Validated EEPROM at a new address** | Magic + version + checksum + range checks, stored at **address 64** so v1.5.0's combos (address 0) survive a round trip. |
| 🕹️ **New default combos** | `L+R+X+Y` / `L+R+A+B` — they do not collide with any SD2SNES/FXPak shortcut. |
| 📖 **Docs** | This README, wiring tables, boot legend and the Portuguese version ([README.md](README.md)). |

---

## 🧭 Which firmware should I flash?

There are **two images**, and the only difference is whether the pads
(`A0`/`A1`/`A2`) are soldered:

| Firmware | Pads | Flash this when |
|---|---|---|
| `firmware/sicksnes-v2.1.0.hex` | **yes** | You wired `A0`/`A1`/`A2`. You get in-game reset and combo configuration. Most complete build. |
| `firmware/sicksnes-v2.1.0-nopads.hex` | no | CIC bypass only, no pad wires. Smallest image, no controller polling at all. |

The LEDs are **not** part of this choice: `D3`, `D6` and `D7` are positive
outputs and the same firmware drives a single RGB LED or separate LEDs. If you
leave one of them unconnected, you simply do not see that colour.

### Why two?

The CIC bypass only needs `D4`, `D5` and `D8`. Plenty of installs stop there and
never solder the three pad wires, because the mod works fine without them — you
just lose in-game reset and combo configuration.

v1.x was *not* safe in that case: the pins floated with no timeout, so the
firmware could hang or read phantom buttons. v2.x fixes the firmware **and**
ships an image with the pad code compiled out: no polling, no phantom input and
~1.3 KB less flash. It is exactly what `PADS_OFF` in `config.h` does.

| Build | Flash used | `.hex` file |
|---|---|---|
| Pads on | 4440 B | ~12.5 KB |
| Pads off | 3086 B | ~8.7 KB |

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
| `D6` | LED blue (positive) | Blue LED anode, resistor, cathode to GND |
| `D7` | LED red (positive) | Red LED anode, resistor, cathode to GND |
| `D3` | LED green (positive) | Green LED anode, resistor, cathode to GND |
| `D9` | cathode (old wiring) | Only for the v1.x wiring; the firmware holds it LOW |
| `D10` | cathode (old wiring) | Only for the v1.x wiring; the firmware holds it LOW |
| `A0` | controller **latch** | Controller port pin 10 |
| `A1` | controller **clock** | Controller port pin 8 |
| `A2` | controller **data** | Controller port pin 6 |

> **CIC pin 4 must be lifted** (disconnected from the board). It is `CONF`, not
> ground: `+5 V` = lockout enabled, `GND` = lockout disabled (the chip has an
> internal pull-down). v2.x drives it, so the mod can toggle it at runtime —
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

There is nothing to configure: **`D3`, `D6` and `D7` are plain positive LED
outputs**. Put the anode on the pin, a resistor in series and the cathode on
`GND`. Use one LED per pin or a single common-cathode RGB LED (the three colour
legs on the pins, the common leg on `GND`).

| Pin | Colour | What it shows |
|---|---|---|
| `D7` | 🔴 Red | **Lockout state.** On = CIC enabled (the cartridge passed the handshake). Off = the mod disabled the lockout. |
| `D6` | 🔵 Blue | **Feedback.** Blinks in configuration mode; 2 blinks = combo stored / EEPROM restored; 3 blinks = combo rejected; lit during the in-game reset pulse. |
| `D3` | 🟢 Green | **Lockout disabled** (CIC bypassed) and the 80 ms pulse at boot. |

Blue is an **overlay**: it can be on together with red or green (so it looks
magenta/cyan) and never hides the lockout state.

One resistor per colour (330 Ω for red, 220 Ω for green/blue is a good starting
point). **Do not** put a resistor on the common leg.

**Boot legend (it is a diagnostic):**

1. **Red** turns on — same as v1.5.0 (lockout enabled).
2. **One short green pulse (~80 ms) on `D3`** — the Nano ran `setup()` to the
   end. If you see it and the console stays black, the firmware is not the
   problem.
3. **Blue blinks 2×** — fresh/blank EEPROM, default combos written (first boot
   only).
4. **Red steady** — CIC handshake OK. **Green steady** — handshake failed, the
   lockout was disabled and the console was reset.

### Already have the v1.x two-LED wiring?

No rewiring needed. v1.x drove the anode from `D6`/`D7` and switched the cathode
on `D9`/`D10`; v2.1.0 holds `D9`/`D10` `LOW` and lights the LEDs from the anode,
exactly the same way. The only channel missing on that wiring is green (`D3`,
which v1.x never used) — the status stays red/blue, as it always did.

![how to wire the stock RGB LED](images/stock-led.png)

The way back works too: `D7` and `D6` are exactly the pins v1.5.0 uses, so you
can **flash v1.5.0 back on top without rewiring**. It shows solid red/blue on
that LED and simply ignores `D3`.

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

The v2.x defaults were chosen to avoid every documented flashcart shortcut:

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

## 🚫 Installing without the pads

The CIC bypass runs in `setup()` and only uses `D4`, `D5` and `D8`. Without
`A0/A1/A2` you lose in-game reset and combo configuration — nothing else.

Two options:

1. **Flash `sicksnes-v2.1.0-nopads.hex`** (recommended). The pad code is not
   compiled at all: no bus polling, no timeouts, no phantom input.
2. **Flash the normal build and just don't wire them.** Safe in v2.1.0 (plain
   inputs + timeouts), but the firmware still wakes up once per frame to check
   the bus.

Building from source? That is the choice `config.h` gives you: uncomment
`PADS_OFF` (or `PADS_ON` to make it explicit that the pads are wired).

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

## 🛠️ Configuring and building

The configuration is **one file: `sicksnes/config.h`**. Two choices, the way the
classic Arduino mods work: delete the `//` from the line you want.

```cpp
// ---------------------------------------------------------------------------
//  PADS  --  leitura dos controles na porta do SNES (fios A0/A1/A2)
// ---------------------------------------------------------------------------
//  PADS_ON  : reset em jogo + modo de configuracao de combos.  (padrao)
//  PADS_OFF : so o bypass do CIC. Menos codigo, nada de leitura de controle.
//
//  Descomente UMA das duas linhas abaixo. Se nao descomentar nenhuma,
//  fica PADS_ON.

//#define PADS_ON
//#define PADS_OFF

// ---------------------------------------------------------------------------
//  DEPURACAO  --  opcional
// ---------------------------------------------------------------------------
//#define SICKNES_DEBUG 1
```

The LEDs are deliberately not here: `D3`, `D6` and `D7` are positive outputs
that behave the same in every build — see [The LEDs](#-the-leds).

### With the Arduino IDE

1. Open `sicksnes/sicksnes.ino`.
2. Click the **`config.h`** tab (next to the `.ino`).
3. Delete the `//` from the line for your wiring.
4. Compile and upload as usual.

Changed nothing? Then you get the default: pads on (`A0`/`A1`/`A2`) and LEDs on
`D3`/`D6`/`D7`.

### From the command line (arduino-cli)

```bash
arduino-cli compile --fqbn arduino:avr:nano ./sicksnes
```

The same choices exist as build flags, without editing any file:

```bash
# pads on (default)
arduino-cli compile --fqbn arduino:avr:nano ./sicksnes

# pads off
arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DHAS_CONTROLLER=0" ./sicksnes

# with serial debug
arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DSICKNES_DEBUG=1" ./sicksnes
```

### The options

| Option | Default | Effect |
|---|---|---|
| `PADS_ON` | on | Pads read on `A0`/`A1`/`A2`: in-game reset + combos. This is the default. |
| `PADS_OFF` | — | Compiles the pad code out: CIC bypass only. |
| `SICKNES_DEBUG` | `0` | `1` turns on the Serial Monitor (9600) with the compiled config and the combos it sees. |

On the command line those become `-DHAS_CONTROLLER=0` (same as `PADS_OFF`) and
`-DSICKNES_DEBUG=1` — they are literally the defines `config.h` sets.

### Which choice matches each prebuilt image?

| `config.h` | Same as |
|---|---|
| everything commented (default) | `firmware/sicksnes-v2.1.0.hex` |
| `PADS_OFF` | `firmware/sicksnes-v2.1.0-nopads.hex` |

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

v2.x mitigates this with the recommended 10 kΩ pull-up on CIC pin 4 (keeps
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
the firmware uses `CIC_CHECK_DELAY_MS = 1700` for the same sample point.

### How the CIC check works

`D5` reads CIC pin 1 through the internal pull-up. After the settle time the
line is sampled once:

- at `CIC_DATA_RUNNING_LEVEL` (`LOW`) → the handshake succeeded, keep the
  lockout enabled (red);
- otherwise → disable the lockout (`D8` LOW), reset the console, show green.

The level is empirical (it is what v1.5.0 uses on real hardware). If your board
behaves backwards, flip `CIC_DATA_RUNNING_LEVEL` in `pins.h`.

### EEPROM

Combos live at **address 64** in v2.x, with magic + version + checksum + range
and duplicate validation. v1.5.0 uses address 0, so:

- flashing v2.x does not destroy the v1.5.0 combos, and
- flashing v1.5.0 back restores them untouched (no garbage, no phantom combos).

### Controller pins

v1.x defined `latch 19 / clock 20 / Sdata 21`, which on a Nano are **A5/A6/A7**,
while `PINC` bit reads used **PC0/PC1/PC2** (`A0/A1/A2`). It worked only because
all six pins default to inputs after reset. v2.x configures the real
`A0/A1/A2` as plain inputs and reads `PINC` directly inside one clock phase.

---

## 🩺 Troubleshooting

| Symptom | Check |
|---|---|
| Console dead (power LED on, no video) | Is the Arduino powered? An unpowered Nano clamps `/RESET` through its protection diodes. Reconnect the 5 V (or remove the `D4` wire to confirm). |
| No green pulse at boot | The Nano did not reach the end of `setup()`. Check the flash and the 5 V. |
| Green LED steady, console resets in a loop | The CIC handshake is being read as failed. Confirm `D8` → CIC pin 4 is really lifted. |
| Blue never lights | The only "always visible" blue is the EEPROM blink on first boot and the config mode — blue is feedback, not a status light. |
| Combos do nothing | `A0/A1/A2` wired? Did you flash the `-nopads` build? |
| In-game reset also hits the flashcart | You are using v1.x combos (`Start+L/R`). Restore the defaults (`L+R+A+B` at power-on). |
| `avrdude: Device signature = 00 00 00` / `FF FF FF` | ISP wiring or power, not the hex. With **Arduino as ISP**: `D10` → target `RESET` (ICSP pin 5, **not** the Uno's own ICSP pin 5), `D11/D12/D13`, `GND`, one 5 V source only, and a 10 µF cap between `RESET` and `GND` **on the programmer**. Never use `-F`. |

---

## 📝 Changelog

### v2.1.0
- **`sicksnes/config.h`**: a single file with the choices, PsNee style. `PADS_ON`
  / `PADS_OFF` for the pads and `SICKNES_DEBUG` for the serial port. No flags,
  no code edits. The equivalent flags (`-DHAS_CONTROLLER=0`,
  `-DSICKNES_DEBUG=1`) still work and produce the same binary.
- **Simplified LEDs**: `D7`, `D6` and `D3` are three positive outputs, one per
  colour, with no common-anode/cathode option and no channel swapping. Less
  code, less documentation, same result.
- **The old two-LED wiring keeps working**: `D9`/`D10` are held `LOW`, so an
  existing v1.x install needs no rewiring.
- **Two prebuilt images** instead of four: pads and no pads.
- `SICKNES_DEBUG` prints the compiled configuration on the Serial Monitor.

### v2.0.0
- **Boot path is a line-by-line port of v1.5.0** — `setup()` order, reset helper
  (`D4` HIGH 500 ms → release), `D5` pull-up, plain-input controller pins and the
  1700 ms CIC sample point. `reset.cpp` is verbatim v1.5.0.
- **Green "alive" pulse** on `D3` at the end of `setup()`.
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
- v2.x maintenance (single config file, boot-path restoration, robustness fixes, docs):
  this fork.

The mod is provided as-is, with no guarantees. Modify and distribute at your own
risk — and under **GPL-3.0**, like the original.
