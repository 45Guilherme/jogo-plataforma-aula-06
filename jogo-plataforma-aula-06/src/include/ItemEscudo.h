/**
 * @file ItemEscudo.h
 * @brief Declarações das funções do Item (Escudo de Água).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo item (escudo de água).
 */
ItemEscudo *criarItemEscudo( Rectangle ret, Color cor );

/**
 * @brief Destroi um item (escudo de água).
 */
void destruirItemEscudo( ItemEscudo *item );

/**
 * @brief Atualiza um item (escudo de água).
 */
void atualizarItemEscudo( ItemEscudo *item, float delta );

/**
 * @brief Desenha um item (escudo de água).
 */
void desenharItemEscudo( ItemEscudo *item );

/**
 * @brief Obtém o quadro de animação atual de um item (escudo de água).
 */
QuadroAnimacao *getQuadroAnimacaoAtualItemEscudo( ItemEscudo *item );
