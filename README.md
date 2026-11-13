# 🎮 sick snes — v2.1.0

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
ideia original do reset). A v2.x é uma versão de manutenção desse trabalho:
mantém o comportamento comprovado da v1.5.0 no caminho de boot, corrige alguns
detalhes de robustez e organiza a configuração num único arquivo.

Licença: **GPL-3.0**, herdada do projeto original. Veja [LICENSE](LICENSE).

---

## 📚 Índice

1. [Novidades da v2.1.0](#-novidades-da-v210)
2. [Qual firmware gravar?](#-qual-firmware-gravar)
3. [Fiação](#-fiação)
4. [Os LEDs](#-os-leds)
5. [Comandos](#️-comandos)
6. [Modo de configuração](#️-modo-de-configuração)
7. [Compatibilidade com SD2SNES / FXPak](#-compatibilidade-com-sd2snes--fxpak)
8. [Instalar sem os pads](#-instalar-sem-os-pads)
9. [Gravação](#-gravação)
10. [Configurar e compilar](#️-configurar-e-compilar)
11. [Notas de compatibilidade](#-notas-de-compatibilidade)
12. [Notas técnicas](#-notas-técnicas)
13. [Solução de problemas](#-solução-de-problemas)
14. [Changelog](#-changelog)

---

## ✨ Novidades da v2.1.0

| | |
|---|---|
| 💡 **Três LEDs, três pinos** | `D7` = vermelho (lockout ativo), `D6` = azul (feedback), `D3` = verde (lockout desligado + piscada de boot). São saídas **positivas**: ânodo no pino, cátodo no GND. Um LED por pino ou um LED RGB de cátodo comum. |
| 🧭 **Caminho de boot idêntico à v1.5.0** | Ordem do `setup()`, rotina de reset, pull-up do CIC e instante de amostragem são iguais aos da imagem que liga. `reset.cpp` é cópia linha por linha. |
| 🎛️ **`config.h` no estilo PsNee** | Um arquivo, linhas comentadas: *descomente esta para desligar os pads*, *descomente esta para ligar*. Sem flags, sem editar código. |
| 🎮 **Pinos do controle realmente corrigidos** | A v1.x fazia `#define latch 19 / clock 20 / Sdata 21`, que no Nano são **A5/A6/A7**, enquanto o código lia **A0/A1/A2**. A v2.x usa os `A0/A1/A2` de verdade. |
| ⏱️ **Timeout no barramento** | Controle ausente não trava mais o loop; leitura falha = todos os botões soltos. |
| ⏳ **Combo precisa de ~400 ms** | Uma única frame com ruído não reseta mais o console. |
| 💾 **EEPROM validada e em endereço novo** | Magic + versão + checksum + faixa, gravada no **endereço 64** — os combos da v1.5.0 (endereço 0) sobrevivem à ida e à volta. |
| 🕹️ **Novos combos padrão** | `L+R+X+Y` / `L+R+A+B` — não colidem com nenhum atalho do SD2SNES/FXPak. |
| 📖 **Documentação** | Este README, tabelas de fiação, legenda do boot e a versão em inglês ([README.en.md](README.en.md)). |

---

## 🧭 Qual firmware gravar?

São **duas imagens**, e a única diferença é se os fios dos pads (`A0`/`A1`/`A2`)
estão soldados:

| Firmware | Pads | Grave quando |
|---|---|---|
| `firmware/sicksnes-v2.1.0.hex` | **sim** | Você soldou `A0`/`A1`/`A2`. Tem reset em jogo e configuração de combos. Build completo. |
| `firmware/sicksnes-v2.1.0-nopads.hex` | não | Só o bypass do CIC, sem os fios dos pads. Menor imagem e sem nenhuma leitura de controle. |

Os LEDs **não** entram nessa escolha: `D3`, `D6` e `D7` são saídas positivas e o
mesmo firmware serve para um LED RGB único ou para LEDs separados. Se você não
ligar algum deles, simplesmente não vê aquela cor.

### Por que existem duas?

O bypass do CIC só precisa de `D4`, `D5` e `D8`. Muita instalação para por aí e
nunca solda os três fios dos pads, porque o mod funciona sem eles — você só
perde o reset em jogo e a configuração de combos.

A v1.x **não era segura** nesse cenário: os pinos ficavam flutuando, sem timeout,
e o firmware podia travar ou ler botões fantasmas. A v2.x corrige o firmware
**e** entrega uma imagem com o código dos pads removido: sem polling, sem chance
de entrada fantasma e ~1,3 KB menos de flash. É o que o `config.h` faz quando
você descomenta `PADS_OFF`.

| Build | Flash usada | Arquivo `.hex` |
|---|---|---|
| Pads ligados | 4440 B | ~12,5 KB |
| Pads desligados | 3086 B | ~8,7 KB |

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
| `D6` | LED azul (positivo) | Ânodo do LED azul, resistor, cátodo no GND |
| `D7` | LED vermelho (positivo) | Ânodo do LED vermelho, resistor, cátodo no GND |
| `D3` | LED verde (positivo) | Ânodo do LED verde, resistor, cátodo no GND |
| `D9` | cátodo (fiação antiga) | Só para quem já tem a fiação da v1.x; o firmware segura em LOW |
| `D10` | cátodo (fiação antiga) | Só para quem já tem a fiação da v1.x; o firmware segura em LOW |
| `A0` | **latch** do controle | Porta do controle, pino 10 |
| `A1` | **clock** do controle | Porta do controle, pino 8 |
| `A2` | **data** do controle | Porta do controle, pino 6 |

> **O pino 4 do CIC precisa ser levantado** (desconectado da placa). Ele é o
> `CONF`, não terra: `+5 V` = lockout ativo, `GND` = lockout desligado (o chip
> tem pull-down interno). A v2.x o dirige, então o mod liga e desliga o lockout
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

Não tem nada para configurar: **`D3`, `D6` e `D7` são saídas positivas de LED**.
Ligue o ânodo no pino, um resistor em série e o cátodo no `GND`. Use um LED por
pino ou um único LED RGB de cátodo comum (as três pernas de cor nos pinos e a
perna comum no `GND`).

| Pino | Cor | O que mostra |
|---|---|---|
| `D7` | 🔴 Vermelho | **Estado do lockout.** Aceso = CIC ativo (jogo passou no handshake). Apagado = o mod desligou o lockout. |
| `D6` | 🔵 Azul | **Feedback.** Pisca no modo de configuração; 2 piscadas = combo salvo / EEPROM restaurada; 3 piscadas = combo recusado; fica aceso durante o pulso de reset em jogo. |
| `D3` | 🟢 Verde | **Lockout desligado** (CIC liberado) e a piscada de 80 ms no boot. |

O azul é **overlay**: pode acender junto com o vermelho ou o verde (fica
magenta/ciano) e nunca esconde o estado do lockout.

Um resistor por cor (330 Ω no vermelho, 220 Ω no verde/azul é um bom ponto de
partida). **Não** coloque resistor na perna comum.

**Legenda do boot (é um diagnóstico):**

1. **Vermelho** acende — igual à v1.5.0 (lockout ativo).
2. **Um pulso verde curto (~80 ms) no `D3`** — o Nano rodou o `setup()` até o
   fim. Se o console ficar preto **e** o pulso aparecer, o problema não é o
   firmware.
3. **Azul pisca 2×** — EEPROM nova/vazia, combos padrão gravados (só no primeiro
   boot).
4. **Vermelho fixo** — handshake do CIC ok. **Verde fixo** — o handshake falhou,
   o lockout foi desligado e o console foi resetado.

### Já tem a fiação de 2 LEDs da v1.x?

Não precisa ressoldar. A v1.x ligava o ânodo em `D6`/`D7` e chaveava o cátodo em
`D9`/`D10`; a v2.1.0 mantém `D9`/`D10` em `LOW` e acende os LEDs pelo ânodo, do
mesmo jeito. O único canal que não existe nessa fiação é o verde (`D3`, que a
v1.x não usava) — o status continua vermelho/azul, como sempre foi.

![como ligar o LED original](images/stock-led.png)

E o caminho de volta também funciona: como `D7` e `D6` são exatamente os pinos
que a v1.5.0 usa, **dá para regravar a v1.5.0 por cima sem ressoldar nada**. Ela
acende vermelho/azul sólidos e ignora o `D3`.

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

Os combos padrão da v2.1.0 foram escolhidos para evitar todos os atalhos
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

## 🚫 Instalar sem os pads

O bypass do CIC roda no `setup()` e usa só `D4`, `D5` e `D8`. Sem `A0/A1/A2`
você perde o reset em jogo e a configuração de combos — nada além disso.

Duas opções:

1. **Grave o `sicksnes-v2.1.0-nopads.hex`** (recomendado). O código dos pads não
   é compilado: sem polling, sem timeout, sem entrada fantasma.
2. **Grave o build normal e simplesmente não ligue os fios.** É seguro na v2.1.0
   (entradas simples + timeouts), mas o firmware ainda acorda uma vez por frame
   para checar o barramento.

Se você compila do código, é a escolha que o `config.h` oferece: descomente
`PADS_OFF` (ou `PADS_ON` para deixar explícito que os pads estão ligados).

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

A configuração é **um arquivo só: `sicksnes/config.h`**. São duas escolhas, no
estilo dos mods clássicos de Arduino: apague o `//` da linha que você quer.

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

Os LEDs não aparecem aqui de propósito: `D3`, `D6` e `D7` são saídas positivas e
funcionam do mesmo jeito em qualquer montagem — veja [Os LEDs](#-os-leds).

### Pelo Arduino IDE

1. Abra `sicksnes/sicksnes.ino`.
2. Clique na aba **`config.h`** (fica ao lado do `.ino`).
3. Apague o `//` da linha da sua montagem.
4. Compile e envie normalmente.

Não mexeu em nada? Então sai o padrão: pads ligados (`A0`/`A1`/`A2`) e LEDs em
`D3`/`D6`/`D7`.

### Pela linha de comando (arduino-cli)

```bash
arduino-cli compile --fqbn arduino:avr:nano ./sicksnes
```

As mesmas escolhas existem como flags, sem editar arquivo nenhum:

```bash
# pads ligados (padrao)
arduino-cli compile --fqbn arduino:avr:nano ./sicksnes

# pads desligados
arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DHAS_CONTROLLER=0" ./sicksnes

# com depuracao serial
arduino-cli compile --fqbn arduino:avr:nano \
  --build-property "compiler.cpp.extra_flags=-DSICKNES_DEBUG=1" ./sicksnes
```

### As opções

| Opção | Padrão | Efeito |
|---|---|---|
| `PADS_ON` | ligado | Pads lidos em `A0`/`A1`/`A2`: reset em jogo + combos. É o padrão. |
| `PADS_OFF` | — | Remove o código dos pads: só o bypass do CIC. |
| `SICKNES_DEBUG` | `0` | `1` liga o Monitor Serial (9600) com a configuração compilada e os combos detectados. |

Na linha de comando os mesmos nomes viram `-DHAS_CONTROLLER=0` (equivalente a
`PADS_OFF`) e `-DSICKNES_DEBUG=1` — são exatamente as definições que o
`config.h` usa por baixo.

### Qual escolha gera cada imagem pronta?

| `config.h` | Equivale a |
|---|---|
| tudo comentado (padrão) | `firmware/sicksnes-v2.1.0.hex` |
| `PADS_OFF` | `firmware/sicksnes-v2.1.0-nopads.hex` |

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

A v2.x mitiga com o pull-up recomendado de 10 kΩ no pino 4 do CIC (mantém o
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
`redLedOn()`), e o firmware usa `CIC_CHECK_DELAY_MS = 1700` para o mesmo instante.

### Como a checagem do CIC funciona

O `D5` lê o pino 1 do CIC pelo pull-up interno. Depois do tempo de acomodação a
linha é amostrada uma vez:

- em `CIC_DATA_RUNNING_LEVEL` (`LOW`) → o handshake deu certo, lockout continua
  ativo (vermelho);
- caso contrário → desliga o lockout (`D8` LOW), reseta o console, mostra verde.

O nível é empírico (é o que a v1.5.0 usa em hardware real). Se a sua placa se
comportar ao contrário, inverta `CIC_DATA_RUNNING_LEVEL` no `pins.h`.

### EEPROM

Os combos ficam no **endereço 64** na v2.x, com validação de magic + versão +
checksum + faixa + duplicados. A v1.5.0 usa o endereço 0, então:

- gravar a v2.x não destrói os combos da v1.5.0, e
- gravar a v1.5.0 de volta os restaura intactos (sem lixo, sem combos fantasmas).

### Pinos do controle

A v1.x definia `latch 19 / clock 20 / Sdata 21`, que no Nano são **A5/A6/A7**,
enquanto a leitura de `PINC` usava **PC0/PC1/PC2** (`A0/A1/A2`). Só funcionava
porque os seis pinos são entradas após o reset. A v2.x configura os
`A0/A1/A2` reais como entradas simples e lê `PINC` direto dentro de uma fase de
clock.

---

## 🩺 Solução de problemas

| Sintoma | Verifique |
|---|---|
| Console morto (LED de power aceso, sem vídeo) | O Arduino está alimentado? Um Nano sem 5 V grampeia o `/RESET` pelos diodos de proteção. Reconecte os 5 V (ou solte o fio do `D4` para confirmar). |
| Sem piscada verde no boot | O Nano não chegou ao fim do `setup()`. Confira a gravação e os 5 V. |
| Verde fixo e o console resetando em loop | O handshake está sendo lido como falha. Confirme que o `D8` → pino 4 do CIC está realmente levantado. |
| Azul nunca acende | O azul é feedback, não status: só aparece na inicialização da EEPROM (primeiro boot) e no modo de configuração. |
| Combos não fazem nada | `A0/A1/A2` ligados? Você gravou o `-nopads`? |
| O reset em jogo também mexe no flashcart | Você está com os combos da v1.x (`Start+L/R`). Restaure os padrões (`L+R+A+B` ao ligar). |
| `avrdude: Device signature = 00 00 00` / `FF FF FF` | Fiação ou energia do ISP, não o hex. Com **Arduino as ISP**: `D10` → `RESET` do alvo (pino 5 do ICSP, **não** o pino 5 do ICSP do Uno), `D11/D12/D13`, `GND`, uma única fonte de 5 V e um capacitor de 10 µF entre `RESET` e `GND` **no programador**. Nunca use `-F`. |

---

## 📝 Changelog

### v2.1.0
- **`sicksnes/config.h`**: um único arquivo com as escolhas, no estilo PsNee.
  `PADS_ON` / `PADS_OFF` para os pads e `SICKNES_DEBUG` para o Serial. Sem
  flags, sem editar código. As flags equivalentes (`-DHAS_CONTROLLER=0`,
  `-DSICKNES_DEBUG=1`) continuam funcionando e geram o mesmo binário.
- **LEDs simplificados**: `D7`, `D6` e `D3` são três saídas positivas, uma por
  cor, sem opção de ânodo/cátodo comum nem troca de canais. Menos código, menos
  documentação, mesmo resultado.
- **Fiação antiga de 2 LEDs continua funcionando**: `D9`/`D10` ficam em `LOW`,
  então quem já montou no esquema da v1.x não precisa ressoldar.
- **Duas imagens prontas** em vez de quatro: com pads e sem pads.
- `SICKNES_DEBUG` mostra a configuração compilada no Monitor Serial.

### v2.0.0
- **Caminho de boot é cópia linha por linha da v1.5.0** — ordem do `setup()`,
  rotina de reset (`D4` HIGH 500 ms → solta), pull-up do `D5`, pinos do controle
  como entradas simples e amostragem do CIC em 1700 ms. `reset.cpp` é verbatim.
- **Piscada verde de "vivo"** no `D3` no fim do `setup()`.
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
- Manutenção da v2.x (configuração num arquivo, restauração do caminho de boot, correções de
  robustez, documentação): este fork.

O mod é fornecido como está, sem garantias. Modifique e distribua por sua conta e
risco — e sob **GPL-3.0**, como o original.
