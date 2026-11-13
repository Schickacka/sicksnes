// ===========================================================================
//  sick snes v2.1.0  --  CONFIGURACAO
// ---------------------------------------------------------------------------
//  E' isto que voce faz aqui: descomentar (apagar as duas barras //) a linha
//  da opcao que voce quer. Nada de flags, nada de editar o codigo.
//
//  Se nao mexer em nada, o firmware sai com a montagem padrao:
//  pads ligados (A0/A1/A2) e os LEDs em D3/D6/D7.
// ===========================================================================

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
//  Mensagens no Monitor Serial a 9600 baud (resumo da configuracao e os
//  combos que o mod reconhece). Nao atrapalha o console.
//#define SICKNES_DEBUG 1

// ===========================================================================
//  LEDs (D3, D6 e D7) -- NAO E' CONFIGURACAO, e' so' para consulta.
//
//  Os tres pinos sao saidas POSITIVAS de LED: ligue o anodo no pino, um
//  resistor em serie e o catodo no GND. Um LED por pino, ou um LED RGB de
//  catodo comum (as tres pernas de cor nos pinos e a perna comum no GND).
//
//    D7  ->  LED vermelho : estado do lockout (aceso = CIC ativo)
//    D6  ->  LED azul     : feedback (pisca no modo de configuracao,
//                           2 piscadas = combo salvo, 3 = combo recusado)
//    D3  ->  LED verde    : lockout desligado (CIC liberado) e a piscada
//                           de 80 ms no boot ("firmware chegou ao fim")
//
//  Ja tem a fiacao antiga da v1.x, com os catodos em D9/D10? Nao precisa
//  ressoldar: o firmware segura D9/D10 em LOW e os LEDs funcionam igual.
// ===========================================================================
