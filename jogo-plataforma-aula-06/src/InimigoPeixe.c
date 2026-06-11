/**
 * @file InimigoPeixe.c
 * @brief Implementação do Inimigo (Peixe / Buzz Bomber).
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoPeixe.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoInimigoPeixe( InimigoPeixe *inimigo, QuadroAnimacao *qa, Color tonalidade );
static void desenharQuadroAnimacaoInimigoPeixeMorrendo( InimigoPeixe *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade );
static Animacao *getAnimacaoAtualInimigoPeixe( InimigoPeixe *inimigo );

static const bool MOSTRAR_RETANGULOS = false;
static const float VEL_NADO = 120.0f;

/**
 * @brief Cria um novo Inimigo (peixe).
 */
InimigoPeixe *criarInimigoPeixe( Rectangle ret, Color cor, float limiteEsquerda, float limiteDireita, float alturaMinima, float alturaMaxima ) {

    InimigoPeixe *novoInimigo = (InimigoPeixe*) malloc( sizeof( InimigoPeixe ) );

    novoInimigo->ret = ret;
    novoInimigo->vel = (Vector2) { 0 };
    novoInimigo->cor = cor;

    novoInimigo->velocidadeNado = VEL_NADO;
    novoInimigo->limiteEsquerda = limiteEsquerda;
    novoInimigo->limiteDireita = limiteDireita;
    novoInimigo->alturaMinima = alturaMinima;
    novoInimigo->alturaMaxima = alturaMaxima;

    novoInimigo->estado = ESTADO_INIMIGO_PEIXE_NADANDO;
    novoInimigo->ativo = true;
    novoInimigo->olhandoParaDireita = true;

    int quantidadeAnimacoes = 0;

    novoInimigo->animacaoNadando.quantidadeQuadros = 4;
    novoInimigo->animacaoNadando.quadroAtual = 0;
    novoInimigo->animacaoNadando.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoNadando.pararNoUltimoQuadro = false;
    novoInimigo->animacaoNadando.executarUmaVez = false;
    novoInimigo->animacaoNadando.finalizada = false;
    criarQuadrosAnimacao( &novoInimigo->animacaoNadando, novoInimigo->animacaoNadando.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoInimigo->animacaoNadando.quadros,
        novoInimigo->animacaoNadando.quantidadeQuadros,
        120,
        664, 324,
        48, 48,
        4,
        false,
        (Rectangle) { 2, 2, 68, 58 }
    );

    novoInimigo->animacaoMorrendo.quantidadeQuadros = 4;
    novoInimigo->animacaoMorrendo.quadroAtual = 0;
    novoInimigo->animacaoMorrendo.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoMorrendo.pararNoUltimoQuadro = false;
    novoInimigo->animacaoMorrendo.executarUmaVez = true;
    novoInimigo->animacaoMorrendo.finalizada = false;
    criarQuadrosAnimacao( &novoInimigo->animacaoMorrendo, novoInimigo->animacaoMorrendo.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoInimigo->animacaoMorrendo.quadros,
        novoInimigo->animacaoMorrendo.quantidadeQuadros,
        100,
        169, 1,
        32, 32,
        1,
        false,
        (Rectangle) { 0 }
    );

    novoInimigo->animacoes[ESTADO_INIMIGO_PEIXE_NADANDO] = &novoInimigo->animacaoNadando; quantidadeAnimacoes++;
    novoInimigo->animacoes[ESTADO_INIMIGO_PEIXE_MORRENDO] = &novoInimigo->animacaoMorrendo; quantidadeAnimacoes++;
    novoInimigo->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoInimigo;

}

/**
 * @brief Destroi um inimigo (peixe).
 */
void destruirInimigoPeixe( InimigoPeixe *inimigo ) {
    if ( inimigo != NULL ) {
        for ( int i = 0; i < inimigo->quantidadeAnimacoes; i++ ) {
            destruirQuadrosAnimacao( inimigo->animacoes[i] );
        }
        free( inimigo );
    }
}

/**
 * @brief Atualiza um inimigo (peixe).
 */
void atualizarInimigoPeixe( InimigoPeixe *inimigo, GameWorld *gw, float delta ) {

    if ( !inimigo->ativo ) {
        return;
    }

    if ( inimigo->estado == ESTADO_INIMIGO_PEIXE_NADANDO ) {

        Animacao *animacaoAtual = getAnimacaoAtualInimigoPeixe( inimigo );
        atualizarAnimacao( animacaoAtual, delta );

        if ( inimigo->olhandoParaDireita ) {
            inimigo->vel.x = inimigo->velocidadeNado;
        } else {
            inimigo->vel.x = -inimigo->velocidadeNado;
        }

        inimigo->ret.x += inimigo->vel.x * delta;

        if ( inimigo->ret.x <= inimigo->limiteEsquerda ) {
            inimigo->ret.x = inimigo->limiteEsquerda;
            inimigo->olhandoParaDireita = true;
        }
        if ( inimigo->ret.x + inimigo->ret.width >= inimigo->limiteDireita ) {
            inimigo->ret.x = inimigo->limiteDireita - inimigo->ret.width;
            inimigo->olhandoParaDireita = false;
        }

        if ( inimigo->ret.y < inimigo->alturaMinima ) {
            inimigo->ret.y = inimigo->alturaMinima;
        }
        if ( inimigo->ret.y + inimigo->ret.height > inimigo->alturaMaxima ) {
            inimigo->ret.y = inimigo->alturaMaxima - inimigo->ret.height;
        }

    } else if ( inimigo->estado == ESTADO_INIMIGO_PEIXE_MORRENDO ) {

        atualizarAnimacao( &inimigo->animacaoMorrendo, delta );

        if ( inimigo->animacaoMorrendo.finalizada ) {
            inimigo->ativo = false;
        }

    }

}

/**
 * @brief Desenha um inimigo (peixe).
 */
void desenharInimigoPeixe( InimigoPeixe *inimigo ) {

    if ( !inimigo->ativo ) {
        return;
    }

    if ( inimigo->estado == ESTADO_INIMIGO_PEIXE_NADANDO ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualInimigoPeixe( inimigo );
        desenharQuadroAnimacaoInimigoPeixe( inimigo, qa, WHITE );

    } else if ( inimigo->estado == ESTADO_INIMIGO_PEIXE_MORRENDO ) {

        desenharQuadroAnimacaoInimigoPeixeMorrendo( inimigo, getQuadroAtualAnimacao( &inimigo->animacaoMorrendo ), 2.0f, WHITE );

    }

    if ( MOSTRAR_RETANGULOS ) {
        DrawRectangleRec( inimigo->ret, Fade( inimigo->cor, 0.5f ) );
        DrawRectangleLines( inimigo->ret.x, inimigo->ret.y, inimigo->ret.width, inimigo->ret.height, BLACK );
    }

}

/**
 * @brief Obtém o quadro de animação atual de um inimigo (peixe).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoPeixe( InimigoPeixe *inimigo ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualInimigoPeixe( inimigo ) );
}

static void desenharQuadroAnimacaoInimigoPeixe( InimigoPeixe *inimigo, QuadroAnimacao *qa, Color tonalidade ) {

    if ( qa != NULL ) {

        DrawTexturePro(
            rm.texturaBadniks,
            (Rectangle) {
                qa->fonte.x,
                qa->fonte.y,
                inimigo->olhandoParaDireita ? -qa->fonte.width : qa->fonte.width,
                qa->fonte.height
            },
            inimigo->ret,
            (Vector2) { 0 },
            0.0f,
            tonalidade
        );

        if ( MOSTRAR_RETANGULOS ) {
            float xDesenho = inimigo->olhandoParaDireita
                ? inimigo->ret.x + inimigo->ret.width - qa->retColisao.x - qa->retColisao.width
                : inimigo->ret.x + qa->retColisao.x;
            float yDesenho = inimigo->ret.y + qa->retColisao.y;
            DrawRectangle( xDesenho, yDesenho, qa->retColisao.width, qa->retColisao.height, Fade( GREEN, 0.5f ) );
        }

    }

}

static void desenharQuadroAnimacaoInimigoPeixeMorrendo( InimigoPeixe *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade ) {

    if ( qa != NULL ) {

        DrawTexturePro(
            rm.texturaBadniks,
            qa->fonte,
            (Rectangle) {
                inimigo->ret.x,
                inimigo->ret.y,
                qa->fonte.width * escala,
                qa->fonte.height * escala
            },
            (Vector2) { 0 },
            0.0f,
            tonalidade
        );

    }

}

static Animacao *getAnimacaoAtualInimigoPeixe( InimigoPeixe *inimigo ) {
    return inimigo->animacoes[inimigo->estado];
}
