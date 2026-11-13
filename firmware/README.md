# firmware/ — imagens prontas da v2.1.0

São duas imagens, e a única diferença é se os fios dos pads (`A0`/`A1`/`A2`)
estão soldados. Os LEDs não entram na escolha: `D3`, `D6` e `D7` são saídas
positivas e funcionam do mesmo jeito em qualquer montagem.

| Arquivo | Pads (`A0`/`A1`/`A2`) | Tamanho do sketch |
|---|---|---|
| `sicksnes-v2.1.0.hex` | **sim** | 4440 B |
| `sicksnes-v2.1.0-nopads.hex` | não | 3086 B |

Se estiver em dúvida, leia ["Qual firmware gravar?"](../README.md#-qual-firmware-gravar)
no README.

São imagens **só da aplicação**: grave o `.hex`, não mexa nos fuses e não grave
arquivo `.eep`.

## Checksums (SHA-256)

```
a3c8461c03f25a7e29eb3e1e737ec9d2bd3d9c70e95b1489e09bde7517c2486e  sicksnes-v2.1.0.hex
4f7ac3ad50e6b4a8a2dfcf3d415d5ca3a007c397085c3e14892ed29422bbb93b  sicksnes-v2.1.0-nopads.hex
```

```powershell
Get-FileHash .\sicksnes-v2.1.0.hex -Algorithm SHA256
```

```bash
sha256sum sicksnes-v2.1.0.hex
```

## Como essas imagens foram geradas

As duas saem do mesmo código, mudando só a escolha dos pads. No Arduino IDE, o
equivalente é descomentar a linha no `sicksnes/config.h`
(veja ["Configurar e compilar"](../README.md#️-configurar-e-compilar)):

```bash
# pads ligados (padrao)
arduino-cli compile --fqbn arduino:avr:nano \
  --output-dir out ./sicksnes

# pads desligados
arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DHAS_CONTROLLER=0" \
  --output-dir out ./sicksnes
```

## Versões anteriores

As quatro imagens da **v2.0.0** (com/sem controle, LED RGB ou 2 LEDs) continuam
disponíveis na [release v2.0.0](https://github.com/Schickacka/sicksnes/releases/tag/v2.0.0),
para quem já instalou naquele esquema. A v2.1.0 cobre os dois casos com duas
imagens porque o firmware agora sempre acende os LEDs pelos pinos `D3`/`D6`/`D7`
e mantém `D9`/`D10` em `LOW` para a fiação antiga.

## Créditos

O mod, o projeto de hardware e o firmware v1.x são de
**[@lucaslealdev](https://github.com/lucaslealdev)** —
[github.com/lucaslealdev/sicksnes](https://github.com/lucaslealdev/sicksnes).
A v2.x é um fork de manutenção, sob **GPL-3.0**.
