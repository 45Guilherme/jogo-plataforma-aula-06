/**
 * @file InimigoPeixe.h
 * @brief Declarações das funções do Inimigo (Peixe).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo inimigo (peixe).
 */
InimigoPeixe *criarInimigoPeixe( Rectangle ret, Color cor, float limiteEsquerda, float limiteDireita, float alturaMinima, float alturaMaxima );

/**
 * @brief Destroi um inimigo (peixe).
 */
void destruirInimigoPeixe( InimigoPeixe *inimigo );

/**
 * @brief Atualiza um inimigo (peixe).
 */
void atualizarInimigoPeixe( InimigoPeixe *inimigo, GameWorld *gw, float delta );

/**
 * @brief Desenha um inimigo (peixe).
 */
void desenharInimigoPeixe( InimigoPeixe *inimigo );

/**
 * @brief Obtém o quadro de animação atual de um inimigo (peixe).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoPeixe( InimigoPeixe *inimigo );
