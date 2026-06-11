/**
 * @file ItemEscudo.c
 * @brief Implementação do Item (Escudo de Água).
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "ItemEscudo.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoItemEscudo( ItemEscudo *item, QuadroAnimacao *qa, Color tonalidade );
static Animacao *getAnimacaoAtualItemEscudo( ItemEscudo *item );

static const bool MOSTRAR_RETANGULOS = false;

ItemEscudo *criarItemEscudo( Rectangle ret, Color cor ) {

    ItemEscudo *novoItem = (ItemEscudo*) malloc( sizeof( ItemEscudo ) );

    novoItem->ret = ret;
    novoItem->cor = cor;
    novoItem->estado = ESTADO_ITEM_ESCUDO_PARADO;
    novoItem->ativo = true;

    int quantidadeAnimacoes = 0;

    novoItem->animacaoParado.quantidadeQuadros = 4;
    novoItem->animacaoParado.quadroAtual = 0;
    novoItem->animacaoParado.contadorTempoQuadro = 0.0f;
    novoItem->animacaoParado.pararNoUltimoQuadro = false;
    novoItem->animacaoParado.executarUmaVez = false;
    novoItem->animacaoParado.finalizada = false;
    criarQuadrosAnimacao( &novoItem->animacaoParado, novoItem->animacaoParado.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoItem->animacaoParado.quadros,
        novoItem->animacaoParado.quantidadeQuadros,
        120,
        1, 1,
        16, 16,
        1,
        false,
        (Rectangle) { 0, 0, 32, 32 }
    );

    novoItem->animacaoColetando.quantidadeQuadros = 4;
    novoItem->animacaoColetando.quadroAtual = 0;
    novoItem->animacaoColetando.contadorTempoQuadro = 0.0f;
    novoItem->animacaoColetando.pararNoUltimoQuadro = false;
    novoItem->animacaoColetando.executarUmaVez = true;
    novoItem->animacaoColetando.finalizada = false;
    criarQuadrosAnimacao( &novoItem->animacaoColetando, novoItem->animacaoColetando.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoItem->animacaoColetando.quadros,
        novoItem->animacaoColetando.quantidadeQuadros,
        80,
        1, 18,
        16, 16,
        1,
        false,
        (Rectangle) { 0 }
    );

    novoItem->animacoes[ESTADO_ITEM_ESCUDO_PARADO] = &novoItem->animacaoParado;
    quantidadeAnimacoes++;
    novoItem->animacoes[ESTADO_ITEM_ESCUDO_COLETADO] = &novoItem->animacaoColetando;
    quantidadeAnimacoes++;
    novoItem->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoItem;

}

void destruirItemEscudo( ItemEscudo *item ) {
    if ( item != NULL ) {
        for ( int i = 0; i < item->quantidadeAnimacoes; i++ ) {
            destruirQuadrosAnimacao( item->animacoes[i] );
        }
        free( item );
    }
}

void atualizarItemEscudo( ItemEscudo *item, float delta ) {
    if ( item->ativo ) {
        Animacao *animacaoAtual = getAnimacaoAtualItemEscudo( item );
        atualizarAnimacao( animacaoAtual, delta );
        if ( item->estado == ESTADO_ITEM_ESCUDO_COLETADO && animacaoAtual->finalizada ) {
            item->ativo = false;
        }
    }
}

void desenharItemEscudo( ItemEscudo *item ) {
    if ( !item->ativo ) {
        return;
    }

    QuadroAnimacao *qa = getQuadroAnimacaoAtualItemEscudo( item );
    if ( qa == NULL ) {
        return;
    }

    desenharQuadroAnimacaoItemEscudo( item, qa, WHITE );

    if ( MOSTRAR_RETANGULOS ) {
        DrawRectangleRec( item->ret, Fade( item->cor, 0.5f ) );
        DrawRectangleLines( item->ret.x, item->ret.y, item->ret.width, item->ret.height, BLACK );
    }
}

QuadroAnimacao *getQuadroAnimacaoAtualItemEscudo( ItemEscudo *item ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualItemEscudo( item ) );
}

static Animacao *getAnimacaoAtualItemEscudo( ItemEscudo *item ) {
    return item->animacoes[item->estado];
}

static void desenharQuadroAnimacaoItemEscudo( ItemEscudo *item, QuadroAnimacao *qa, Color tonalidade ) {

    if ( qa != NULL ) {
        DrawTexturePro(
            rm.texturaItens,
            qa->fonte,
            item->ret,
            (Vector2){ 0 },
            0.0f,
            tonalidade
        );
    }

}
