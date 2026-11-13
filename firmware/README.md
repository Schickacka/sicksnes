# firmware/ — imagens prontas da v2.0.0

Escolha o arquivo que combina com a **sua** fiação. Se estiver em dúvida, leia
["Qual firmware gravar?"](../README.md#-qual-firmware-gravar) no README.

| Arquivo | Fiação dos LEDs | Fios do controle (`A0/A1/A2`) | Tamanho do sketch |
|---|---|---|---|
| `sicksnes-v2.0.0-rgb-controller.hex` | um LED RGB | **sim** | 4408 B |
| `sicksnes-v2.0.0-rgb-nocontroller.hex` | um LED RGB | não | 3054 B |
| `sicksnes-v2.0.0-2led-controller.hex` | dois LEDs comuns | **sim** | 4400 B |
| `sicksnes-v2.0.0-2led-nocontroller.hex` | dois LEDs comuns | não | 3014 B |

São imagens **só da aplicação**: grave o `.hex`, não mexa nos fuses e não grave
arquivo `.eep`.

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

## Como essas imagens foram geradas

As quatro combinações saem do mesmo código, mudando só as opções de build. No
Arduino IDE, o equivalente é descomentar as linhas no `sicksnes/config.h`
(veja ["Configurar e compilar"](../README.md#️-configurar-e-compilar)):

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

## Créditos

O mod, o projeto de hardware e o firmware v1.x são de
**[@lucaslealdev](https://github.com/lucaslealdev)** —
[github.com/lucaslealdev/sicksnes](https://github.com/lucaslealdev/sicksnes).
A v2.0.0 é um fork de manutenção, sob **GPL-3.0**.
