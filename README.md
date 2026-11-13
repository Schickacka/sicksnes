# 🎮 sick snes — v2.0.0

![logo do sick snes](images/logo.jpg)

**Troca automática do chip de lockout (CIC) e reset em jogo para o Super Nintendo.**

Se o cartucho não passa no handshake do CIC, o mod desliga o lockout e reseta o
console sozinho — imports, homebrew e flashcarts funcionam sem nenhum switch.
Além disso você ganha reset curto e reset longo pelo controle, sem furar o case
e sem botão extra.

➡️ English version: [README.en.md](README.en.md)

---

## 🙌 Créditos — leia primeiro

**Este é um fork. O projeto original, a pesquisa de hardware e todo o firmware
v1.x são de [@lucaslealdev](https://github.com/lucaslealdev).**

> **Repositório original: [github.com/lucaslealdev/sicksnes](https://github.com/lucaslealdev/sicksnes)**
> YouTube: [Pixel que Byte](https://www.youtube.com/@PixelqueByte)

Tudo o que faz esse mod funcionar — levantar o pino 4 do CIC, soldar no reset,
ler o barramento do controle, o esquema dos LEDs, a ideia do reset em jogo — foi
projetado, montado e validado por ele (com crédito a
[JeffWDH/NES-In-Game-Reset](https://github.com/JeffWDH/NES-In-Game-Reset) pela
ideia original do reset). A v2.0.0 é uma versão de manutenção desse trabalho:
mantém o comportamento comprovado da v1.5.0 no caminho de boot e adiciona um LED
RGB único opcional, mais algumas correções de robustez.

Licença: **GPL-3.0**, herdada do projeto original. Veja [LICENSE](LICENSE).

---

## 📚 Índice

1. [Novidades da v2.0.0](#-novidades-da-v200)
2. [Qual firmware gravar?](#-qual-firmware-gravar)
3. [Fiação](#-fiação)
4. [Os LEDs](#-os-leds)
5. [Comandos](#️-comandos)
6. [Modo de configuração](#️-modo-de-configuração)
7. [Compatibilidade com SD2SNES / FXPak](#-compatibilidade-com-sd2snes--fxpak)
8. [Instalar sem os fios do controle](#-instalar-sem-os-fios-do-controle)
9. [Gravação](#-gravação)
10. [Configurar e compilar](#️-configurar-e-compilar)
11. [Notas de compatibilidade](#-notas-de-compatibilidade)
12. [Notas técnicas](#-notas-técnicas)
13. [Solução de problemas](#-solução-de-problemas)
14. [Changelog](#-changelog)

---

## ✨ Novidades da v2.0.0

| | |
|---|---|
| 🌈 **LED RGB único** | Um LED RGB de 4 pinos substitui os dois LEDs: vermelho = lockout ativo, verde = lockout desligado, azul = feedback. `D6` e `D7` continuam onde a v1.x deixou, então **dá para regravar a v1.5.0 sem ressoldar nada**. |
| 🧭 **Caminho de boot idêntico à v1.5.0** | Ordem do `setup()`, rotina de reset, pull-up do CIC e instante de amostragem são iguais aos da imagem que liga. `reset.cpp` é cópia linha por linha. |
| 🎮 **Pinos do controle realmente corrigidos** | A v1.x fazia `#define latch 19 / clock 20 / Sdata 21`, que no Nano são **A5/A6/A7**, enquanto o código lia **A0/A1/A2**. A v2.0.0 usa os `A0/A1/A2` de verdade. |
| ⏱️ **Timeout no barramento** | Controle ausente não trava mais o loop; leitura falha = todos os botões soltos. |
| ⏳ **Combo precisa de ~400 ms** | Uma única frame com ruído não reseta mais o console. |
| 💾 **EEPROM validada e em endereço novo** | Magic + versão + checksum + faixa, gravada no **endereço 64** — os combos da v1.5.0 (endereço 0) sobrevivem à ida e à volta. |
| 🕹️ **Novos combos padrão** | `L+R+X+Y` / `L+R+A+B` — não colidem com nenhum atalho do SD2SNES/FXPak. |
| 💡 **Piscada verde de "vivo"** | No build RGB, um pulso verde de 80 ms no `D3` no fim do `setup()` — canal de diagnóstico que a v1.5.0 não usa. |
| 🎛️ **Um único arquivo de configuração** | `sicksnes/config.h`: descomente a linha da sua montagem (LED RGB, sem controle, depuração) e compile. Sem editar código, sem decorar flags. |
| 📖 **Documentação** | Este README, tabelas de fiação, legenda do boot e a versão em inglês ([README.en.md](README.en.md)). |

---

## 🧭 Qual firmware gravar?

Duas coisas independentes definem o firmware: **como os LEDs estão ligados** e
**se `A0`/`A1`/`A2` estão soldados**.

| Firmware | LED | Controle | Grave quando |
|---|---|---|---|
| `firmware/sicksnes-v2.0.0-rgb-controller.hex` | RGB | sim | Você colocou um LED RGB **e** ligou `A0/A1/A2`. Build mais completo. |
| `firmware/sicksnes-v2.0.0-rgb-nocontroller.hex` | RGB | não | LED RGB, mas **sem** `A0/A1/A2`. Só bypass de CIC + LEDs. |
| `firmware/sicksnes-v2.0.0-2led-controller.hex` | 2 LEDs | sim | Fiação de LED da v1.x + fios do controle. O setup clássico. |
| `firmware/sicksnes-v2.0.0-2led-nocontroller.hex` | 2 LEDs | não | Fiação de LED da v1.x, sem fios do controle. Menor imagem. |

### Por que essas opções existem?

- **"Com controle" x "sem controle".** O bypass do CIC só precisa de `D4`, `D5`
  e `D8`. Muita instalação para por aí e nunca solda os três fios do controle,
  porque o mod funciona sem eles — você só perde o reset em jogo e a
  configuração de combos. A v1.x **não era segura** nesse cenário: os pinos
  ficavam flutuando sem timeout, e o firmware podia travar ou ler botões
  fantasmas. A v2.0.0 corrige o firmware **e** entrega um build com o código do
  controle removido (`HAS_CONTROLLER=0`): sem polling, sem chance de entrada
  fantasma e ~1,4 KB menos de flash.
- **"RGB" x "2 LEDs".** O build de 2 LEDs é o esquema original (ânodo em
  `D6`/`D7`, cátodo chaveado em `D9`/`D10`). O build RGB usa um pino por cor e
  mantém `D6`/`D7` nas mesmas duas cores da v1.x, com o verde novo em `D3`. Esse
  mapeamento específico é o que permite **regravar a v1.5.0 por cima sem
  ressoldar**: a v1.5.0 acende vermelho/azul sólidos nesse LED e simplesmente
  ignora o `D3`.

Gravar a combinação "errada" não danifica nada — os pinos são entradas — você só
fica com um recurso que não consegue usar, ou com feedback num canal que não
ligou.

| Build | Flash usada | Arquivo |
|---|---|---|
| RGB + controle | 4408 B | 12.417 B |
| RGB, sem controle | 3054 B | 8.604 B |
| 2 LEDs + controle | 4400 B | 12.388 B |
| 2 LEDs, sem controle | 3014 B | 8.498 B |

---

## 🔌 Fiação

### Chicote principal

| Pino do Arduino | Sinal | Ligação no SNES |
|---|---|---|
| `5V` | alimentação | VCC da placa |
| `GND` | alimentação | GND da placa |
| `D4` | `/RESET` (ativo em LOW) | Botão de reset / ponto de teste do reset |
| `D5` | dados do CIC | **CIC pino 1** |
| `D8` | `CONF` do CIC | **CIC pino 4** (precisa estar levantado) |
| `D6` | LED, 2ª cor | Ânodo do LED azul (RGB) / verde (2 LEDs) |
| `D7` | LED, vermelho | Ânodo do LED vermelho |
| `D3` | LED, verde | Ânodo do LED verde (**só no build RGB**) |
| `D9` | retorno de cátodo | Cátodo do LED verde (**só no build de 2 LEDs**) |
| `D10` | retorno de cátodo | Cátodo do LED vermelho (**só no build de 2 LEDs**) |
| `A0` | **latch** do controle | Porta do controle, pino 10 |
| `A1` | **clock** do controle | Porta do controle, pino 8 |
| `A2` | **data** do controle | Porta do controle, pino 6 |

> **O pino 4 do CIC precisa ser levantado** (desconectado da placa). Ele é o
> `CONF`, não terra: `+5 V` = lockout ativo, `GND` = lockout desligado (o chip
> tem pull-down interno). A v2.0.0 o dirige, então o mod liga e desliga o lockout
> em tempo de execução — diferente do mod clássico de "cortar e aterrar".

### ↻ Botão de reset (visto por baixo da placa)

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

`D5` → CIC pino 1 (dados), `D8` → CIC pino 4 (`CONF`, levantado).

### 🎮 Porta do controle SNES (vista por baixo da placa)

```
    10 _____________________________ 2
      |   A0   A1   A2   •    •     |
      |                             |
      | •    •    •    •    •    •  |
    11 ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾ 1
```

`A0` = latch, `A1` = clock, `A2` = data. O Arduino **só lê** essas linhas; ele
nunca as dirige, então não briga com o console, com um multitap ou com um
flashcart.

---

## 💡 Os LEDs

### Build de 2 LEDs (fiação da v1.x)

Vermelho = lockout ativo, verde = lockout desligado (piscando). O LED vermelho
original pode ser reaproveitado: desconecte o lado de 5 V do resistor e alimente
o resistor pelos dois, `D6` e `D7`.

![como ligar o LED original](images/stock-led.png)

Os cátodos voltam para o Arduino em `D9`/`D10`, e é isso que permite apagar cada
LED por completo — é por isso que ele pisca em vez de só trocar de cor.

### LED RGB único (novo na v2.0.0)

Use um LED RGB de **cátodo comum** (pernas R/G/B positivas + um único `GND`):

- vermelho → resistor → `D7`
- azul → resistor → `D6`
- verde → resistor → `D3`
- perna comum → `GND`
- `D9`/`D10` não são usados; deixe desconectados.

Um resistor por perna de cor (330 Ω no vermelho, 220 Ω no verde/azul é um bom
ponto de partida). **Não** coloque resistor na perna comum.

| Cor | Significado |
|---|---|
| 🔴 Vermelho | Lockout ativo (operação normal) |
| 🟢 Verde | Lockout desligado (bypass do CIC entrou) |
| 🔵 Azul | Feedback (overlay): pisca no modo de configuração, 2 piscadas = combos salvos / EEPROM restaurada, 3 piscadas = combo rejeitado, aceso durante o pulso de reset em jogo |

O azul é **overlay**: pode acender junto com o vermelho ou o verde (fica
magenta/ciano) e nunca esconde o estado do lockout.

**Legenda do boot (é um diagnóstico):**

1. **Vermelho** acende — igual à v1.5.0 (lockout ativo).
2. **Um pulso verde curto (~80 ms) no `D3`** — só a v2.0.0 faz isso. Se você viu,
   o Nano rodou o `setup()` novo até o fim. Se o console ficar preto **e** o
   pulso aparecer, o problema não é o firmware.
3. **Azul pisca 2×** — EEPROM nova/vazia, combos padrão gravados (só no primeiro
   boot).
4. **Vermelho fixo** — handshake do CIC ok. **Verde fixo** — o handshake falhou,
   o lockout foi desligado e o console foi resetado.

**Por que azul no `D6` e verde no `D3`:** a v1.5.0 usa `D7` para vermelho e `D6`
para a segunda cor de status. Mantendo o azul em `D6`, uma imagem v1.5.0
regravada por cima continua piscando vermelho/azul nesse LED. O `D3` não é usado
pela v1.5.0, então o canal verde é simplesmente ignorado lá. Prefere verde no
`D6`? Compile com `-DRGB_GREEN_ON_D6` (o azul vai para o `D3`).

---

## 🕹️ Comandos

| Ação | Combo |
|---|---|
| 🔄 **Reset curto** | `L + R + X + Y` (segurar ~0,4 s) |
| 🔄 **Reset longo** | `L + R + A + B` (segurar ~0,4 s) |
| ⚙️ **Modo de configuração** | segurar `L + R + X + Y` ao ligar |
| ♻️ **Restaurar combos padrão** | segurar `L + R + A + B` ao ligar |

Os ~400 ms de hold evitam que uma frame com ruído resete o console.

---

## ⚙️ Modo de configuração

1. Ligue o console segurando `L + R + X + Y` — o azul começa a piscar.
2. Enquanto pisca, aperte o combo de 4 botões para o **reset curto** → azul pisca
   **2×** (salvo) ou **3×** (rejeitado).
3. O azul pisca de novo; aperte o combo do **reset longo** → 2 piscadas = salvo.
4. Os dois combos precisam ser diferentes e os quatro botões precisam ser
   distintos. O novo combo não pode ser o outro.
5. Desligue e ligue para sair do modo de configuração (não existe saída
   automática).

*Para restaurar os padrões, segure `L + R + A + B` ao ligar.*

---

## 🎮 Compatibilidade com SD2SNES / FXPak

O mod apenas **escuta** o barramento do controle; latch e clock são gerados pelo
console. Não existe interferência elétrica com flashcart, multitap ou um segundo
controle. A única interferência possível é lógica: dois dispositivos reagindo aos
mesmos botões.

Os combos padrão da v2.0.0 foram escolhidos para evitar todos os atalhos
documentados dos flashcarts:

| Atalho SD2SNES / FXPak | Combo | Colide? |
|---|---|---|
| Reset do jogo | `L + R + Select + Start` | Não |
| Voltar ao menu | `L + R + Select + X` | Não |
| Menu em jogo (fork sd2snes+) | `L + R + Y + ←` | Não |
| Salvar state | `Start + R` | Não |
| Carregar state | `Start + L` | Não |
| Slot de savestate | `Select + direcional` | Não |
| Cheats on/off | `L + R + Start + A/B` | Não |
| Hooks off (10 s / permanente) | `L + R + Start + X/Y` | Não |

**Não volte para os padrões da v1.x** (`Start + L + R + Esquerda/Baixo`): eles
contêm `Start + L` e `Start + R`, que são os atalhos de **carregar** e **salvar
state** do flashcart — todo reset curto também dispararia um savestate.

> Com o menu do SD2SNES aberto o jogo fica pausado, mas o Arduino continua vendo
> o controle real. Evite apertar os combos do sick snes dentro desse menu.

---

## 🚫 Instalar sem os fios do controle

O bypass do CIC roda no `setup()` e usa só `D4`, `D5` e `D8`. Sem `A0/A1/A2`
você perde o reset em jogo e a configuração de combos — nada além disso.

Duas opções:

1. **Grave o build `-nocontroller`** (recomendado). O código do controle não é
   compilado: sem polling, sem timeout, sem entrada fantasma.
2. **Grave o build normal e simplesmente não ligue os fios.** É seguro na v2.0.0
   (entradas simples + timeouts), mas o firmware ainda acorda uma vez por frame
   para checar o barramento.

> A v1.x **não era segura** aqui: pinos flutuando sem timeout podiam travar o
> firmware ou gerar botões aleatórios.

---

## 💻 Gravação

As imagens prontas estão em [`firmware/`](firmware/): escolha a que casa com a
sua fiação (veja [a tabela acima](#-qual-firmware-gravar)). Grave o hex da
**aplicação**, não mexa em fuses e não grave arquivo `.eep`.

### 💽 Via USB (bootloader do Arduino)

1. Conecte o Nano/Uno no USB.
2. Envie o sketch (Arduino IDE) ou o `.hex` (AVRDUDESS com programador
   `Arduino`, 115200 baud).
3. Com o bootloader original, o `D8` fica flutuando por ~1,5–2 s enquanto a placa
   inicializa. Com o pull-up recomendado de 10 kΩ no pino 4 do CIC o lockout
   fica ativo nessa janela; sem ele o console liga com o lockout desligado e
   alguns jogos (principalmente *Street Fighter Alpha 2*) podem falhar.

### 🧰 Recomendado: USBasp (sem bootloader, boot mais rápido)

1. Instale o driver com o [Zadig](https://zadig.akeo.ie/): `Options > List All
   Devices`, escolha o `USBasp`, marque `libusb-win32` (ou `libusbK`) e clique em
   `Install Driver`.
2. Abra o **Extreme Burner - AVR**, selecione **ATmega328P**, `File > Open
   Flash`, carregue o `.hex` e clique em `Write All`.

> ⚠️ Aviso de "signature mismatch" pode ser ignorado em ATmega328PB (a assinatura
> é diferente do 328P, mas ele é compatível para este mod).

### ⚡ Regra prática com o mod já instalado

Nunca deixe duas fontes de 5 V no Arduino ao mesmo tempo:

- **Console ligado, Arduino alimentado pelo console:** desconecte só o fio de
  5 V que vem do SNES antes de plugar o programador, mantendo `GND` e todos os
  sinais.
- **Console desligado:** alimente o Arduino pelo programador (um diodo no fio de
  5 V, faixa voltada para o Arduino, impede o programador de alimentar o
  console).

Se o Arduino estiver sem energia e continuar ligado ao console, os diodos de
proteção dele podem grampear a linha de `/RESET` do SNES e o console parece
morto (LED de power aceso, sem vídeo). Não é dano: devolva os 5 V ao Arduino e
ele volta a ligar.

---

## 🛠️ Configurar e compilar

Toda a configuração fica em **um único arquivo: `sicksnes/config.h`**. Você não
precisa decorar flags, nem editar código: é só descomentar a linha da sua
montagem, como nos mods que você já conhece.

```cpp
// ---------------------------------------------------------------------------
//  1) LED
// ---------------------------------------------------------------------------
//  PADRAO: dois LEDs discretos (anodo em D6/D7, catodo chaveado em D9/D10).
//  Se a sua montagem e essa, nao descomente nada.
//
//  LED RGB unico de CATODO comum (a perna comum vai no GND):
//#define HAS_RGB_LED 1
//
//  LED RGB unico de ANODO comum (a perna comum vai no +5 V):
//  descomente as DUAS linhas abaixo.
//#define HAS_RGB_LED 1
//#define RGB_COMMON_ANODE 1
```

### Pelo Arduino IDE (o jeito fácil)

1. Abra `sicksnes/sicksnes.ino`.
2. Clique na aba **`config.h`** (fica ao lado do `.ino`).
3. Apague o `//` do começo das linhas que correspondem à sua montagem.
4. Compile e envie normalmente.

Não mexeu em nada? Então você está compilando a montagem padrão: dois LEDs em
`D6`/`D7` + os três fios do controle em `A0`/`A1`/`A2`.

### Pela linha de comando (arduino-cli)

```bash
arduino-cli compile --fqbn arduino:avr:nano ./sicksnes
```

As mesmas opções do `config.h` também funcionam como flags, sem editar arquivo
nenhum — útil para script ou CI:

```bash
# LED RGB + controle
arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DHAS_RGB_LED=1" ./sicksnes

# LED RGB, sem controle
arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DHAS_CONTROLLER=0 -DHAS_RGB_LED=1" ./sicksnes
```

### As opções

| Opção (em `config.h` ou `-D`) | Padrão | Efeito |
|---|---|---|
| `HAS_CONTROLLER` | `1` | `0` remove o controle/combos do build (sem `A0/A1/A2`). |
| `HAS_RGB_LED` | `0` | `1` usa um LED RGB em vez de dois LEDs. |
| `RGB_COMMON_ANODE` | `0` | `1` para LED RGB de ânodo comum (perna comum em +5 V). |
| `RGB_GREEN_ON_D6` | desligado | Troca verde/azul: verde no `D6`, azul no `D3`. |
| `SICKNES_DEBUG` | `0` | `1` liga o Monitor Serial (9600) com o resumo da configuração e os combos detectados. |

> `RGB_GREEN_ON_D6` vale pela **presença da linha** (`#define RGB_GREEN_ON_D6`,
> sem valor). Para desligar, apague a linha — não use `0`.
>
> Se você ligar uma opção que só existe no build RGB (`RGB_COMMON_ANODE` ou
> `RGB_GREEN_ON_D6`) sem `HAS_RGB_LED 1`, a compilação para com uma mensagem
> explicando o que faltou, em vez de gerar um firmware errado.

### Qual combinação gera cada imagem pronta?

| `HAS_RGB_LED` | `HAS_CONTROLLER` | Equivale a |
|---|---|---|
| `0` | `1` | `sicksnes-v2.0.0-2led-controller.hex` |
| `0` | `0` | `sicksnes-v2.0.0-2led-nocontroller.hex` |
| `1` | `1` | `sicksnes-v2.0.0-rgb-controller.hex` |
| `1` | `0` | `sicksnes-v2.0.0-rgb-nocontroller.hex` |

Testado: editar o `config.h` e compilar gera **exatamente** o mesmo `.hex` que
as flags de linha de comando — as duas formas são equivalentes.

---

## 🔗 Notas de compatibilidade

| Modelo de SNES | Com bootloader (USB) | Sem bootloader (USBasp) |
|---|---|---|
| SNS-CPU-1CHIP-02 | ✅ Completo | ✅ Completo |
| SNS-CPU-1CHIP-03 | ⚠️ Mitigado (veja nota do SFA2) | ✅ Completo |
| SNS-CPU-GPM-02 | não testado | ✅ Completo |
| SNN-CPU-01 (baby jr) | não testado | ✅ Completo |

Qualquer outra placa de SNES deve ser compatível.

### ⚠️ *Street Fighter Alpha 2*

Firmwares antigos podiam falhar ao bootar SFA2 quando o Arduino era gravado pelo
bootloader padrão, porque o `D8` fica flutuando durante o bootloader: o console
liga com o lockout desligado e o Arduino o ativa depois.

A v2.0.0 mitiga com o pull-up recomendado de 10 kΩ no pino 4 do CIC (mantém o
`CONF` ativo enquanto o Arduino inicializa). O workaround antigo — segurar o
**botão de reset** até o Arduino inicializar (~1,8 s após ligar) — continua
funcionando.

### ⚠️ Lockout regional não é conversão de região

Desligar o CIC remove só a checagem de **lockout**. Não muda o modo de vídeo, o
timing de 50/60 Hz nem os pinos `PALMODE`. Jogos PAL num console NTSC (ou o
contrário) ainda podem não bootar ou rodar na velocidade errada. Em placas
SHVC-CPU-01 isso exige trabalho extra de região/50 Hz, fora do escopo do mod.

### ⚠️ Jogos SA-1 e lockout desligado

Alguns títulos SA-1 (*Super Mario RPG*, *Kirby Super Star*) checam o estado do
CIC e podem se recusar a bootar com o lockout desligado. Cartuchos que passam no
handshake continuam com o lockout ativo, então o mod não os afeta; isso só
importa se o CIC foi desligado por outro motivo.

---

## 🧪 Notas técnicas

### O caminho de boot é o da v1.5.0, de propósito

O `reset.cpp` é **cópia linha por linha da v1.5.0**: no boot ele dirige o `D4`
**HIGH** por 500 ms e depois solta o pino (high-Z). Ele **não** puxa `/RESET`
para LOW.

Durante o desenvolvimento da v2.0.0 a rotina de reset foi "melhorada" para um
pulso LOW real de 10 ms. O console parou de ligar na unidade de teste. A lição é
simples: **neste mod o formato do reset faz parte do contrato de boot** — o
handshake do CIC acontece logo após a energização, e cutucar `/RESET` durante a
negociação pode deixar o CIC segurando o console em reset. A sequência
comprovada ficou.

O timing também casa: a v1.5.0 amostra o handshake do CIC **1700 ms** depois de a
linha de reset voltar (1500 ms de `delay` + 200 ms gastos dentro do
`redLedOn()`), e a v2.0.0 usa `CIC_CHECK_DELAY_MS = 1700` para o mesmo instante.

### Como a checagem do CIC funciona

O `D5` lê o pino 1 do CIC pelo pull-up interno. Depois do tempo de acomodação a
linha é amostrada uma vez:

- em `CIC_DATA_RUNNING_LEVEL` (`LOW`) → o handshake deu certo, lockout continua
  ativo (vermelho);
- caso contrário → desliga o lockout (`D8` LOW), reseta o console, mostra verde.

O nível é empírico (é o que a v1.5.0 usa em hardware real). Se a sua placa se
comportar ao contrário, inverta `CIC_DATA_RUNNING_LEVEL` no `pins.h`.

### EEPROM

Os combos ficam no **endereço 64** na v2.0.0, com validação de magic + versão +
checksum + faixa + duplicados. A v1.5.0 usa o endereço 0, então:

- gravar a v2.0.0 não destrói os combos da v1.5.0, e
- gravar a v1.5.0 de volta os restaura intactos (sem lixo, sem combos fantasmas).

### Pinos do controle

A v1.x definia `latch 19 / clock 20 / Sdata 21`, que no Nano são **A5/A6/A7**,
enquanto a leitura de `PINC` usava **PC0/PC1/PC2** (`A0/A1/A2`). Só funcionava
porque os seis pinos são entradas após o reset. A v2.0.0 configura os
`A0/A1/A2` reais como entradas simples e lê `PINC` direto dentro de uma fase de
clock.

---

## 🩺 Solução de problemas

| Sintoma | Verifique |
|---|---|
| Console morto (LED de power aceso, sem vídeo) | O Arduino está alimentado? Um Nano sem 5 V grampeia o `/RESET` pelos diodos de proteção. Reconecte os 5 V (ou solte o fio do `D4` para confirmar). |
| Build RGB: sem piscada verde no boot | O Nano não chegou ao fim do `setup()`. Confira a gravação e os 5 V. |
| Verde fixo e o console resetando em loop | O handshake está sendo lido como falha. Confirme que o `D8` → pino 4 do CIC está realmente levantado. |
| Azul nunca acende | O azul é feedback, não status: só aparece na inicialização da EEPROM (primeiro boot) e no modo de configuração. |
| Combos não fazem nada | `A0/A1/A2` ligados? Você gravou um build `-nocontroller`? |
| O reset em jogo também mexe no flashcart | Você está com os combos da v1.x (`Start+L/R`). Restaure os padrões (`L+R+A+B` ao ligar). |
| `avrdude: Device signature = 00 00 00` / `FF FF FF` | Fiação ou energia do ISP, não o hex. Com **Arduino as ISP**: `D10` → `RESET` do alvo (pino 5 do ICSP, **não** o pino 5 do ICSP do Uno), `D11/D12/D13`, `GND`, uma única fonte de 5 V e um capacitor de 10 µF entre `RESET` e `GND` **no programador**. Nunca use `-F`. |

---

## 📝 Changelog

### Não lançado
- **`sicksnes/config.h`**: um único arquivo para escolher a montagem. Em vez de
  passar `-D` na linha de comando, você descomenta a linha da sua instalação no
  `config.h` e compila no Arduino IDE. As flags continuam funcionando e geram
  **exatamente** o mesmo binário (verificado byte a byte nas quatro variantes).
- **`SICKNES_DEBUG`**: depuração opcional pelo `config.h`, com resumo da
  configuração compilada no Monitor Serial. Desligada por padrão — o binário
  padrão não muda.
- **Checagem de configuração**: ligar `RGB_COMMON_ANODE` ou `RGB_GREEN_ON_D6`
  sem `HAS_RGB_LED 1` agora para a compilação com uma mensagem clara.

### v2.0.0
- **Caminho de boot é cópia linha por linha da v1.5.0** — ordem do `setup()`,
  rotina de reset (`D4` HIGH 500 ms → solta), pull-up do `D5`, pinos do controle
  como entradas simples e amostragem do CIC em 1700 ms. `reset.cpp` é verbatim.
- **LED RGB único opcional** (`-DHAS_RGB_LED=1`): vermelho/verde mantêm o status
  do CIC, azul faz o feedback. `D7` = vermelho, `D6` = azul, `D3` = verde, para
  regravar a v1.5.0 sem ressoldar.
- **Piscada verde de "vivo"** no `D3` no fim do `setup()` (só no build RGB).
- **Pinos do controle corrigidos**: `A0/A1/A2` reais em vez de `A5/A6/A7`.
- **Timeouts no barramento** em `readControllerFrame()`.
- **Hold de ~400 ms** antes de disparar o combo.
- **EEPROM validada** (magic/versão/checksum/faixa/duplicados) no endereço 64,
  independente do endereço 0 da v1.5.0.
- **Novos combos padrão** (`L+R+X+Y`, `L+R+A+B`) que evitam atalhos do
  SD2SNES/FXPak.
- **Gestos de configuração/factory reset** saíram de `Start`/`Select` para os
  combos de reset, então não abrem mais jogo nem lista de cheats no flashcart.
- **Blink não bloqueante** durante a configuração; o loop é paced por frame.
- **`CicGndPin` renomeado** para `PIN_CIC_DATA` (é o pino 1 do CIC, não terra) e
  o estado do CIC fica numa variável em vez de ser lido de uma saída.
- **`pins.h` único** com todos os pinos, flags e timings.
- **Documentação**: este README, as tabelas de fiação, a legenda do boot e a
  versão em inglês.

### v1.5.0 e anteriores
Veja o repositório original: [lucaslealdev/sicksnes](https://github.com/lucaslealdev/sicksnes).

---

## ✨ Créditos

- **[@lucaslealdev](https://github.com/lucaslealdev)** — projeto original, mod de
  hardware, firmware v1.x, esquema de LEDs, documentação e vídeos.
  ⭐ [github.com/lucaslealdev/sicksnes](https://github.com/lucaslealdev/sicksnes)
- **[JeffWDH/NES-In-Game-Reset](https://github.com/JeffWDH/NES-In-Game-Reset)** —
  a abordagem de reset que inspirou o mod.
- [Pixel que Byte](https://www.youtube.com/@PixelqueByte) — vídeos do projeto.
- Manutenção da v2.0.0 (LED RGB, restauração do caminho de boot, correções de
  robustez, documentação): este fork.

O mod é fornecido como está, sem garantias. Modifique e distribua por sua conta e
risco — e sob **GPL-3.0**, como o original.
