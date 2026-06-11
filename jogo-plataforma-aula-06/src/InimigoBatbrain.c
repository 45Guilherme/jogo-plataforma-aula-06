/**
 * @file InimigoBatbrain.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Inimigo (Batbrain).
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoBatbrain.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoInimigoBatbrain( InimigoBatbrain *inimigo, QuadroAnimacao *qa, Color tonalidade );
static void desenharQuadroAnimacaoInimigoBatbrainMorrendo( InimigoBatbrain *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade );
static Animacao *getAnimacaoAtualInimigoBatbrain( InimigoBatbrain *inimigo );

static const bool MOSTRAR_RETANGULOS = false;

static const float VEL_VOANDO = 60.0f;

/**
 * @brief Cria um novo Inimigo (batbrain).
 */
InimigoBatbrain *criarInimigoBatbrain( Rectangle ret, Color cor ) {

    InimigoBatbrain *novoInimigo = (InimigoBatbrain*) malloc( sizeof( InimigoBatbrain ) );

    novoInimigo->ret = ret;
    novoInimigo->vel = (Vector2) { 0 };
    novoInimigo->cor = cor;

    novoInimigo->velAndando = VEL_VOANDO;
    novoInimigo->velMaxQueda = 80;

    novoInimigo->estado = ESTADO_INIMIGO_BATBRAIN_ANDANDO;
    novoInimigo->ativo = true;
    novoInimigo->olhandoParaDireita = false;

    int quantidadeAnimacoes = 0;

    novoInimigo->animacaoAndando.quantidadeQuadros = 5;
    novoInimigo->animacaoAndando.quadroAtual = 0;
    novoInimigo->animacaoAndando.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoAndando.pararNoUltimoQuadro = false;
    novoInimigo->animacaoAndando.executarUmaVez = false;
    novoInimigo->animacaoAndando.finalizada = false;
    criarQuadrosAnimacao( &novoInimigo->animacaoAndando, novoInimigo->animacaoAndando.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoInimigo->animacaoAndando.quadros,
        novoInimigo->animacaoAndando.quantidadeQuadros,
        160,
        1, 96,
        40, 32,
        1,
        false,
        (Rectangle) { 6, 6, 32, 24 }
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
        90,
        1, 160,
        32, 32,
        1,
        false,
        (Rectangle) { 0 }
    );

    novoInimigo->animacoes[ESTADO_INIMIGO_BATBRAIN_ANDANDO] = &novoInimigo->animacaoAndando; quantidadeAnimacoes++;
    novoInimigo->animacoes[ESTADO_INIMIGO_BATBRAIN_MORRENDO] = &novoInimigo->animacaoMorrendo; quantidadeAnimacoes++;
    novoInimigo->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoInimigo;

}

/**
 * @brief Destroi um inimigo (batbrain).
 */
void destruirInimigoBatbrain( InimigoBatbrain *inimigo ) {
    if ( inimigo != NULL ) {
        for ( int i = 0; i < inimigo->quantidadeAnimacoes; i++ ) {
            destruirQuadrosAnimacao( inimigo->animacoes[i] );
        }
        free( inimigo );
    }
}

static void atualizarVooBatbrain( InimigoBatbrain *inimigo, GameWorld *gw, float delta ) {

    if ( inimigo->olhandoParaDireita ) {
        inimigo->vel.x = VEL_VOANDO;
    } else {
        inimigo->vel.x = -VEL_VOANDO;
    }

    inimigo->vel.y = -VEL_VOANDO * 0.6f;

    inimigo->ret.x += inimigo->vel.x * delta;
    inimigo->ret.y += inimigo->vel.y * delta;

    float limiteInferior = -220.0f;
    if ( inimigo->ret.y < limiteInferior ) {
        inimigo->ret.y = limiteInferior;
    }

    float margem = 60.0f;
    float limiteDireito = (float) gw->mapa->colunas * 48.0f - margem;

    if ( inimigo->ret.x > limiteDireito ) {
        inimigo->ret.x = limiteDireito;
        inimigo->olhandoParaDireita = false;
    }

    if ( inimigo->ret.x < margem ) {
        inimigo->ret.x = margem;
        inimigo->olhandoParaDireita = true;
    }

}

/**
 * @brief Atualiza um inimigo (batbrain).
 */
void atualizarInimigoBatbrain( InimigoBatbrain *inimigo, GameWorld *gw, float delta ) {

    if ( inimigo->ativo ) {

        if ( inimigo->estado == ESTADO_INIMIGO_BATBRAIN_ANDANDO ) {

            Animacao *animacaoAtual = getAnimacaoAtualInimigoBatbrain( inimigo );
            atualizarAnimacao( animacaoAtual, delta );

            atualizarVooBatbrain( inimigo, gw, delta );

        } else if ( inimigo->estado == ESTADO_INIMIGO_BATBRAIN_MORRENDO ) {

            atualizarAnimacao( &inimigo->animacaoMorrendo, delta );

            if ( inimigo->animacaoMorrendo.finalizada ) {
                inimigo->ativo = false;
            }

        }

    }

}

/**
 * @brief Desenha um inimigo (batbrain).
 */
void desenharInimigoBatbrain( InimigoBatbrain *inimigo ) {

    if ( inimigo->ativo ) {

        if ( inimigo->estado == ESTADO_INIMIGO_BATBRAIN_ANDANDO ) {
            QuadroAnimacao *qa = getQuadroAnimacaoAtualInimigoBatbrain( inimigo );
            desenharQuadroAnimacaoInimigoBatbrain( inimigo, qa, WHITE );
        } else if ( inimigo->estado == ESTADO_INIMIGO_BATBRAIN_MORRENDO ) {
            desenharQuadroAnimacaoInimigoBatbrainMorrendo( inimigo, getQuadroAtualAnimacao( &inimigo->animacaoMorrendo ), 3.0f, WHITE );
        }

        if ( MOSTRAR_RETANGULOS ) {
            DrawRectangleRec( inimigo->ret, Fade( inimigo->cor, 0.5f ) );
            DrawRectangleLines( inimigo->ret.x, inimigo->ret.y, inimigo->ret.width, inimigo->ret.height, BLACK );
        }

    }

}

/**
 * @brief Obtém o quadro de animação atual de um inimigo (batbrain).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoBatbrain( InimigoBatbrain *inimigo ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualInimigoBatbrain( inimigo ) );
}

static void desenharQuadroAnimacaoInimigoBatbrain( InimigoBatbrain *inimigo, QuadroAnimacao *qa, Color tonalidade ) {

    if ( qa != NULL ) {

        Texture2D *tex = &rm.texturaBadniksBase;

        DrawTexturePro(
            *tex,
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

static void desenharQuadroAnimacaoInimigoBatbrainMorrendo( InimigoBatbrain *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade ) {

    if ( qa != NULL ) {

        Texture2D *tex = &rm.texturaBadniksBase;

        DrawTexturePro(
            *tex,
            qa->fonte,
            (Rectangle) {
                inimigo->ret.x + inimigo->ret.width * 0.5f - qa->fonte.width * escala * 0.5f,
                inimigo->ret.y + inimigo->ret.height * 0.5f - qa->fonte.height * escala * 0.5f,
                qa->fonte.width * escala,
                qa->fonte.height * escala
            },
            (Vector2) { qa->fonte.width * escala * 0.5f, qa->fonte.height * escala * 0.5f },
            0.0f,
            tonalidade
        );

    }

}

static Animacao *getAnimacaoAtualInimigoBatbrain( InimigoBatbrain *inimigo ) {
    return inimigo->animacoes[inimigo->estado];
}
