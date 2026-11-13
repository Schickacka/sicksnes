// ===========================================================================
//  sick snes v2.0.0  --  CONFIGURACAO
// ---------------------------------------------------------------------------
//  Este e o unico arquivo que voce precisa editar.
//
//  Para LIGAR uma opcao: apague as duas barras (//) do comeco da linha.
//  Para DESLIGAR: coloque as duas barras de volta.
//
//  Se voce usa a montagem padrao -- dois LEDs em D6/D7 e os tres fios do
//  controle em A0/A1/A2 -- nao precisa mexer em nada aqui: basta compilar.
//
//  Depois de editar, compile/grave normalmente. As mesmas opcoes tambem
//  funcionam por linha de comando (veja "Compilar do codigo" no README).
// ===========================================================================

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
//
//  AVANCADO -- ordem dos canais:
//  por padrao o vermelho fica no D7, o azul no D6 e o verde no D3. E' esse
//  mapeamento que permite regravar a v1.5.0 por cima sem ressoldar nada.
//  Se o seu LED esta montado ao contrario (verde no D6, azul no D3),
//  descomente a linha abaixo junto com HAS_RGB_LED.
//  Para desligar, apague a linha -- nao use 0.
//#define RGB_GREEN_ON_D6

// ---------------------------------------------------------------------------
//  2) CONTROLE
// ---------------------------------------------------------------------------
//  PADRAO: os tres fios do controle (A0/A1/A2) estao soldados.
//  Se a sua montagem e essa, nao descomente nada.
//
//  Sem os fios do controle (so o bypass do CIC):
//  voce perde o reset em jogo e o modo de configuracao de combos -- nada mais.
//#define HAS_CONTROLLER 0

// ---------------------------------------------------------------------------
//  3) DEPURACAO (opcional)
// ---------------------------------------------------------------------------
//  Mensagens no Monitor Serial a 9600 baud. Nao interfere no console.
//#define SICKNES_DEBUG 1

// ===========================================================================
//  Daqui para baixo nao ha nada para editar.
// ===========================================================================

// Valor padrao da depuracao: desligada.
#ifndef SICKNES_DEBUG
#define SICKNES_DEBUG 0
#endif

// As opcoes que faltarem continuam com o padrao definido em pins.h:
//   HAS_CONTROLLER   1  (fios do controle ligados)
//   HAS_RGB_LED      0  (dois LEDs)
//   RGB_COMMON_ANODE 0  (LED RGB de catodo comum)
// Assim, quem compila passando -D pela linha de comando continua funcionando.
