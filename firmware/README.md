# firmware/ — prebuilt images for v2.0.0

Pick the file that matches **your** wiring. If you are not sure, read
[“Which firmware should I flash?”](../README.md#-which-firmware-should-i-flash)
in the README.

| File | LED wiring | Controller wires (`A0/A1/A2`) | Sketch size |
|---|---|---|---|
| `sicksnes-v2.0.0-rgb-controller.hex` | one RGB LED | **yes** | 4408 B |
| `sicksnes-v2.0.0-rgb-nocontroller.hex` | one RGB LED | no | 3054 B |
| `sicksnes-v2.0.0-2led-controller.hex` | two discrete LEDs | **yes** | 4400 B |
| `sicksnes-v2.0.0-2led-nocontroller.hex` | two discrete LEDs | no | 3014 B |

These are **application-only** images: flash the `.hex`, do not touch the fuses,
and do not flash an `.eep` file.

## Checksums (SHA-256)

```
fa999cc447aadebc9a4c6ec19c1f382fa77d728861442c54755f5ddee86571fa  sicksnes-v2.0.0-2led-controller.hex
e54eed2ddd9f0da1d798a789d79b2d889a691d15d74e38cc6f8c21974bc81e38  sicksnes-v2.0.0-2led-nocontroller.hex
aa6332c820d8514b3eda17a66cd993d3054b961891a81dc04598b706c9fd7ea5  sicksnes-v2.0.0-rgb-controller.hex
0365e481c690a8cf1456f76b8c3fe968bd8deba46ee5994ff42a4f414ef91207  sicksnes-v2.0.0-rgb-nocontroller.hex
```

```powershell
Get-FileHash .\sicksnes-v2.0.0-rgb-controller.hex -Algorithm SHA256
```

```bash
sha256sum sicksnes-v2.0.0-rgb-controller.hex
```

## How these were built

```bash
arduino-cli compile --fqbn arduino:avr:nano \
  --output-dir out ./sicksnes

arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DHAS_RGB_LED=1" \
  --output-dir out ./sicksnes

arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DHAS_CONTROLLER=0" \
  --output-dir out ./sicksnes

arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DHAS_CONTROLLER=0 -DHAS_RGB_LED=1" \
  --output-dir out ./sicksnes
```

## Credits

The mod, the hardware design and the v1.x firmware are by
**[@lucaslealdev](https://github.com/lucaslealdev)** —
[github.com/lucaslealdev/sicksnes](https://github.com/lucaslealdev/sicksnes).
v2.0.0 is a maintenance fork under **GPL-3.0**.
