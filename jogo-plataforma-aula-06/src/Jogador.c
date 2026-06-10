/**
 * @file Jogador.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Jogador.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoMotobug.h"
#include "InimigoSpikes.h"
#include "Item.h"
#include "ItemAnel.h"
#include "ItemAnelAzul.h"
#include "Macros.h"
#include "Jogador.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoJogador( Jogador *j, QuadroAnimacao *qa, Color tonalidade );
static QuadroAnimacao *getQuadroAnimacaoAtualJogador( Jogador *j );
static Animacao *getAnimacaoAtualJogador( Jogador *j );

static void resolverColisaoJogadorObstaculosMapaX( Jogador *j, Mapa *mapa );
static void resolverColisaoJogadorObstaculosMapaY( Jogador *j, Mapa *mapa );

static void resolverColisaoJogadorItensMapa( Jogador *j, Mapa *mapa );
static void resolverColisaoJogadorInimigosMapa( Jogador *j, Mapa *mapa );

static const bool MOSTRAR_RETANGULOS = false;

/**
 * @brief Cria uma instância alocada dinamicamente da struct Jogador.
 */
Jogador *criarJogador( float x, float y, float w, float h ) {

    Jogador *novoJogador = (Jogador*) malloc( sizeof( Jogador ) );

    novoJogador->ret.x = x;
    novoJogador->ret.y = y;
    novoJogador->ret.width = w;
    novoJogador->ret.height = h;
    novoJogador->vel = (Vector2) { 0 };

    novoJogador->cor = BLUE;

    novoJogador->velAndando = 200;
    novoJogador->velAndandoRapido = 400;
    novoJogador->velCorrendo = 800;
    novoJogador->velPulo = -550;
    novoJogador->velMaxQueda = 600;

    novoJogador->aceleracao = 200;
    novoJogador->desaceleracao = 400;
    novoJogador->frenagem = 1800;

    novoJogador->quantidadePulos = 0;
    novoJogador->quantidadeMaxPulos = 3;

    // ===== SISTEMA DE ANÉIS E VIDAS =====
    novoJogador->quantidadeAneis = 0;
    novoJogador->quantidadeVidas = 3;

    // ===== SISTEMA DE PONTUAÇÃO =====
    novoJogador->pontuacao = 0;
    novoJogador->proximoBonusVida = 50000;

    // ===== SISTEMA DE TEMPO =====
    novoJogador->tempoFase = 0.0f;
    novoJogador->minutos = 0;
    novoJogador->segundos = 0;

    novoJogador->invulneravel = false;
    novoJogador->tempoInvulnerabilidade = 3.0f;
    novoJogador->contadorTempoInvulnerabilidade = 0.0f;

    novoJogador->piscaPisca = false;
    novoJogador->tempoPiscaPisca = 0.05f;
    novoJogador->contadorTempoPiscaPisca = 0.0f;

    novoJogador->freando = false;

    novoJogador->estado = ESTADO_JOGADOR_PARADO;
    novoJogador->olhandoParaDireita = true;

    int quantidadeAnimacoes = 0;

    novoJogador->animacaoParado.quantidadeQuadros = 1;
    novoJogador->animacaoParado.quadroAtual = 0;
    novoJogador->animacaoParado.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoParado.pararNoUltimoQuadro = false;
    novoJogador->animacaoParado.executarUmaVez = false;
    novoJogador->animacaoParado.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoParado, novoJogador->animacaoParado.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoJogador->animacaoParado.quadros,
        novoJogador->animacaoParado.quantidadeQuadros,
        1000,
        24, 251,
        48, 48,
        4,
        false,
        (Rectangle) { 32, 20, 42, 76 }
    );

    novoJogador->animacaoAndando.quantidadeQuadros = 8;
    novoJogador->animacaoAndando.quadroAtual = 0;
    novoJogador->animacaoAndando.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoAndando.pararNoUltimoQuadro = false;
    novoJogador->animacaoAndando.executarUmaVez = false;
    novoJogador->animacaoAndando.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoAndando, novoJogador->animacaoAndando.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoJogador->animacaoAndando.quadros,
        novoJogador->animacaoAndando.quantidadeQuadros,
        80,
        664, 324,
        48, 48,
        4,
        false,
        (Rectangle) { 32, 20, 42, 76 }
    );

    novoJogador->animacaoAndandoRapido.quantidadeQuadros = 8;
    novoJogador->animacaoAndandoRapido.quadroAtual = 0;
    novoJogador->animacaoAndandoRapido.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoAndandoRapido.pararNoUltimoQuadro = false;
    novoJogador->animacaoAndandoRapido.executarUmaVez = false;
    novoJogador->animacaoAndandoRapido.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoAndandoRapido, novoJogador->animacaoAndandoRapido.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoJogador->animacaoAndandoRapido.quadros,
        novoJogador->animacaoAndandoRapido.quantidadeQuadros,
        40,
        664, 324,
        48, 48,
        4,
        false,
        (Rectangle) { 32, 20, 42, 76 }
    );

    novoJogador->animacaoCorrendo.quantidadeQuadros = 4;
    novoJogador->animacaoCorrendo.quadroAtual = 0;
    novoJogador->animacaoCorrendo.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoCorrendo.pararNoUltimoQuadro = false;
    novoJogador->animacaoCorrendo.executarUmaVez = false;
    novoJogador->animacaoCorrendo.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoCorrendo, novoJogador->animacaoCorrendo.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoJogador->animacaoCorrendo.quadros,
        novoJogador->animacaoCorrendo.quantidadeQuadros,
        20,
        24, 397,
        48, 48,
        4,
        false,
        (Rectangle) { 32, 20, 42, 76 }
    );

    novoJogador->animacaoPulando.quantidadeQuadros = 4;
    novoJogador->animacaoPulando.quadroAtual = 0;
    novoJogador->animacaoPulando.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulando.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulando.executarUmaVez = false;
    novoJogador->animacaoPulando.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoPulando, novoJogador->animacaoPulando.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoJogador->animacaoPulando.quadros,
        novoJogador->animacaoPulando.quantidadeQuadros,
        40,
        248, 397,
        48, 48,
        4,
        false,
        (Rectangle) { 32, 46, 42, 50 }
    );

    novoJogador->animacaoPulandoRapido.quantidadeQuadros = 4;
    novoJogador->animacaoPulandoRapido.quadroAtual = 0;
    novoJogador->animacaoPulandoRapido.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulandoRapido.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulandoRapido.executarUmaVez = false;
    novoJogador->animacaoPulandoRapido.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoPulandoRapido, novoJogador->animacaoPulandoRapido.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoJogador->animacaoPulandoRapido.quadros,
        novoJogador->animacaoPulandoRapido.quantidadeQuadros,
        25,
        248, 397,
        48, 48,
        4,
        false,
        (Rectangle) { 32, 46, 42, 50 }
    );

    novoJogador->animacaoPulandoCorrendo.quantidadeQuadros = 4;
    novoJogador->animacaoPulandoCorrendo.quadroAtual = 0;
    novoJogador->animacaoPulandoCorrendo.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulandoCorrendo.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulandoCorrendo.executarUmaVez = false;
    novoJogador->animacaoPulandoCorrendo.finalizada = false;
    criarQuadrosAnimacao( &novoJogador->animacaoPulandoCorrendo, novoJogador->animacaoPulandoCorrendo.quantidadeQuadros );
    inicializarQuadrosAnimacao(
        novoJogador->animacaoPulandoCorrendo.quadros,
        novoJogador->animacaoPulandoCorrendo.quantidadeQuadros,
        15,
        248, 397,
        48, 48,
        4,
        false,
        (Rectangle) { 32, 46, 42, 50 }
    );

    novoJogador->animacoes[ESTADO_JOGADOR_PARADO] = &novoJogador->animacaoParado; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_ANDANDO] = &novoJogador->animacaoAndando; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_ANDANDO_RAPIDO] = &novoJogador->animacaoAndandoRapido; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_CORRENDO] = &novoJogador->animacaoCorrendo; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO] = &novoJogador->animacaoPulando; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO_RAPIDO] = &novoJogador->animacaoPulandoRapido; quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO_CORRENDO] = &novoJogador->animacaoPulandoCorrendo; quantidadeAnimacoes++;
    novoJogador->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoJogador;
}

/**
 * @brief Destrói um objeto Jogador e libera seus recursos.
 */
void destruirJogador( Jogador *j ) {
    if ( j != NULL ) {
        for ( int i = 0; i < j->quantidadeAnimacoes; i++ ) {
            destruirQuadrosAnimacao( j->animacoes[i] );
        }
        free( j );
    }
}

/**
 * @brief Lê a entrada do usuário e atualiza as velocidades do jogador.
 */
void entradaJogador( Jogador *j, float delta ) {

    EstadoJogador estadoAnterior = j->estado;

    bool direitaDown  = IsKeyDown( KEY_RIGHT )     || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT ) );
    bool esquerdaDown = IsKeyDown( KEY_LEFT )      || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_LEFT_FACE_LEFT ) );
    bool puloPressed  = IsKeyPressed( KEY_SPACE )  || ( IsGamepadAvailable( 0 ) && IsGamepadButtonDown( 0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN ) );

    if ( direitaDown ) {
        if ( j->vel.x < 0 ) {
            j->vel.x += j->frenagem * delta;
            if ( !j->freando && j->estado == ESTADO_JOGADOR_CORRENDO ) {
                PlaySound( rm.somFrenagem );
                j->freando = true;
            }
            if ( j->vel.x > 0 ) {
                j->vel.x = 0;
                j->freando = false;
            }
        } else {
            j->vel.x += j->aceleracao * delta;
            if ( j->vel.x > j->velCorrendo ) {
                j->vel.x = j->velCorrendo;
            }
        }
        j->olhandoParaDireita = true;
    } else if ( esquerdaDown ) {
        if ( j->vel.x > 0 ) {
            j->vel.x -= j->frenagem * delta;
            if ( !j->freando && j->estado == ESTADO_JOGADOR_CORRENDO ) {
                PlaySound( rm.somFrenagem );
                j->freando = true;
            }
            if ( j->vel.x < 0 ) {
                j->vel.x = 0;
                j->freando = false;
            }
        } else {
            j->vel.x -= j->aceleracao * delta;
            if ( j->vel.x < -j->velCorrendo ) {
                j->vel.x = -j->velCorrendo;
            }
        }
        j->olhandoParaDireita = false;
    } else {
        if ( j->vel.x > 0 ) {
            j->vel.x -= j->desaceleracao * delta;
            if ( j->vel.x < 0 ) {
                j->vel.x = 0;
            }
        } else if ( j->vel.x < 0 ) {
            j->vel.x += j->desaceleracao * delta;
            if ( j->vel.x > 0 ) {
                j->vel.x = 0;
            }
        }
    }

    float absVelX = fabsf( j->vel.x );
    if ( j->quantidadePulos > 0 ) {
        if ( absVelX <= j->velAndando ) {
            j->estado = ESTADO_JOGADOR_PULANDO;
        } else if ( absVelX <= j->velAndandoRapido ) {
            j->estado = ESTADO_JOGADOR_PULANDO_RAPIDO;
        } else {
            j->estado = ESTADO_JOGADOR_PULANDO_CORRENDO;
        }
    } else if ( absVelX < 1.0f ) {
        j->estado = ESTADO_JOGADOR_PARADO;
    } else if ( absVelX <= j->velAndando ) {
        j->estado = ESTADO_JOGADOR_ANDANDO;
    } else if ( absVelX <= j->velAndandoRapido ) {
        j->estado = ESTADO_JOGADOR_ANDANDO_RAPIDO;
    } else {
        j->estado = ESTADO_JOGADOR_CORRENDO;
    }

    if ( puloPressed && j->quantidadePulos < j->quantidadeMaxPulos ) {
        j->vel.y = j->velPulo;
        j->quantidadePulos++;
        PlaySound( rm.somPulo );
    }

    // sincronização de animações andando e andando rápido
    if ( estadoAnterior == ESTADO_JOGADOR_ANDANDO && j->estado == ESTADO_JOGADOR_ANDANDO_RAPIDO ) {
        sincronizarAnimacao( &j->animacaoAndandoRapido, &j->animacaoAndando );
    } else if ( estadoAnterior == ESTADO_JOGADOR_ANDANDO_RAPIDO && j->estado == ESTADO_JOGADOR_ANDANDO ) {
        sincronizarAnimacao( &j->animacaoAndando, &j->animacaoAndandoRapido );
    }

}

/**
 * @brief Aplica física e resolve colisões do jogador com o mundo.
 */
void atualizarJogador( Jogador *j, GameWorld *gw, float delta ) {

    if ( j->invulneravel ) {

        j->contadorTempoPiscaPisca += delta;
        if ( j->contadorTempoPiscaPisca >= j->tempoPiscaPisca ) {
            j->contadorTempoPiscaPisca = 0.0f;
            j->piscaPisca = !j->piscaPisca;
        }

        j->contadorTempoInvulnerabilidade += delta;
        if ( j->contadorTempoInvulnerabilidade >= j->tempoInvulnerabilidade ) {
            j->contadorTempoInvulnerabilidade = 0.0f;
            j->invulneravel = false;
            j->contadorTempoPiscaPisca = 0.0f;
            j->piscaPisca = false;
        }

    }

    Animacao *animacaoAtual = getAnimacaoAtualJogador( j );
    atualizarAnimacao( animacaoAtual, delta );

    // fase X: move horizontalmente e resolve colisões laterais
    j->ret.x += j->vel.x * delta;
    resolverColisaoJogadorObstaculosMapaX( j, gw->mapa );

    // fase Y: aplica gravidade, move verticalmente e resolve colisões verticais
    j->vel.y += gw->gravidade * delta;
    if ( j->vel.y > j->velMaxQueda ) {
        j->vel.y = j->velMaxQueda;
    }
    j->ret.y += j->vel.y * delta;
    resolverColisaoJogadorObstaculosMapaY( j, gw->mapa );

    resolverColisaoJogadorItensMapa( j, gw->mapa );
    resolverColisaoJogadorInimigosMapa( j, gw->mapa );

}

/**
 * @brief Desenha o jogador.
 */
void desenharJogador( Jogador *j ) {

    if ( !j->piscaPisca ) {
        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );
        desenharQuadroAnimacaoJogador( j, qa, WHITE );
    }

    if ( MOSTRAR_RETANGULOS ) {
        DrawRectangleRec( j->ret, Fade( j->cor, 0.5f ) );
        DrawRectangleLines( j->ret.x, j->ret.y, j->ret.width, j->ret.height, BLACK );
    }

}

static void desenharQuadroAnimacaoJogador( Jogador *j, QuadroAnimacao *qa, Color tonalidade ) {

    if ( qa != NULL ) {

        DrawTexturePro(
            rm.texturaJogador,
            (Rectangle) {
                qa->fonte.x,
                qa->fonte.y,
                j->olhandoParaDireita ? qa->fonte.width : -qa->fonte.width,
                qa->fonte.height
            },
            j->ret,
            (Vector2) { 0 },
            0.0f,
            tonalidade
        );

        if ( MOSTRAR_RETANGULOS ) {
            float xDesenho = j->olhandoParaDireita
                ? j->ret.x + qa->retColisao.x
                : j->ret.x + j->ret.width - qa->retColisao.x - qa->retColisao.width;
            float yDesenho = j->ret.y + qa->retColisao.y;
            DrawRectangle( xDesenho, yDesenho, qa->retColisao.width, qa->retColisao.height, Fade( GREEN, 0.5f ) );
        }

    }

}

static QuadroAnimacao *getQuadroAnimacaoAtualJogador( Jogador *j ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualJogador( j ) );
}

static Animacao *getAnimacaoAtualJogador( Jogador *j ) {
    return j->animacoes[j->estado];
}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo X.
 */
static void resolverColisaoJogadorObstaculosMapaX( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Obstaculo *o = (Obstaculo*) el->objeto;

        if ( CheckCollisionRecs( retColCalculado, o->ret ) ) {
            if ( retColCalculado.x + retColCalculado.width / 2 < o->ret.x + o->ret.width / 2 ) {
                j->ret.x = o->ret.x - qa->retColisao.width - deslocamentoX;
            } else {
                j->ret.x = o->ret.x + o->ret.width - deslocamentoX;
            }
            j->vel.x = 0;
        }

        el = el->proximo;

    }

}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo Y.
 */
static void resolverColisaoJogadorObstaculosMapaY( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->obstaculos;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Obstaculo *o = (Obstaculo*) el->objeto;

        if ( CheckCollisionRecs( retColCalculado, o->ret ) ) {
            if ( retColCalculado.y + retColCalculado.height / 2 < o->ret.y + o->ret.height / 2 ) {
                j->ret.y = o->ret.y - qa->retColisao.height - deslocamentoY;
                j->quantidadePulos = 0;
            } else {
                j->ret.y = o->ret.y + o->ret.height - deslocamentoY;
            }
            j->vel.y = 0;
        }

        el = el->proximo;

    }

}

static void resolverColisaoJogadorItensMapa( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->itens;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Item *item = (Item*) el->objeto;

        if ( item->tipo == TIPO_ITEM_ANEL ) {

            ItemAnel *itemAnel = (ItemAnel*) item->objeto;

            if ( !itemAnel->ativo || itemAnel->estado == ESTADO_ITEM_ANEL_COLETADO ) {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemAnel( itemAnel );
            
            Rectangle retColItemCalculado = {
                itemAnel->ret.x + qaItem->retColisao.x,
                itemAnel->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width,
                qaItem->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColItemCalculado ) ) {
                itemAnel->estado = ESTADO_ITEM_ANEL_COLETADO;
                coletarAnel( j, 1 );
                PlaySound( rm.somAnel );
            }

        } else if ( item->tipo == TIPO_ITEM_ANEL_AZUL ) {

            ItemAnelAzul *itemAnelAzul = (ItemAnelAzul*) item->objeto;

            if ( !itemAnelAzul->ativo || itemAnelAzul->estado == ESTADO_ITEM_ANEL_AZUL_COLETADO ) {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemAnelAzul( itemAnelAzul );
            
            Rectangle retColItemCalculado = {
                itemAnelAzul->ret.x + qaItem->retColisao.x,
                itemAnelAzul->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width,
                qaItem->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColItemCalculado ) ) {
                itemAnelAzul->estado = ESTADO_ITEM_ANEL_AZUL_COLETADO;
                coletarAnel( j, 10 );
                PlaySound( rm.somAnel );
            }

        }

        el = el->proximo;

    }

}

static void resolverColisaoJogadorInimigosMapa( Jogador *j, Mapa *mapa ) {

    ElementoMapa *el = mapa->inimigos;

    while ( el != NULL ) {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador( j );

        float deslocamentoX = j->olhandoParaDireita
            ? qa->retColisao.x
            : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height
        };

        Inimigo *inimigo = (Inimigo*) el->objeto;

        QuadroAnimacao *qaInimigo = NULL;
        bool *olhandoParaDireita = NULL;
        Rectangle *ret = NULL;

        if ( inimigo->tipo == TIPO_INIMIGO_MOTOBUG ) {

            InimigoMotobug *motobug = (InimigoMotobug*) inimigo->objeto;

            if ( !motobug->ativo || motobug->estado == ESTADO_INIMIGO_MOTOBUG_MORRENDO ) {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoMotobug( motobug );
            olhandoParaDireita = &motobug->olhandoParaDireita;
            ret = &motobug->ret;

            float deslocamentoX = *olhandoParaDireita
                ? ret->width - qaInimigo->retColisao.x - qaInimigo->retColisao.width
                : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX,
                ret->y + deslocamentoY,
                qaInimigo->retColisao.width,
                qaInimigo->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColInimigoCalculado ) ) {

                if ( j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO ) {
                    j->vel.y = j->velPulo;
                    motobug->estado = ESTADO_INIMIGO_MOTOBUG_MORRENDO;
                    adicionarPontuacao( j, 100 );
                    PlaySound( rm.somHitInimigo );
                } else if ( !j->invulneravel ) {
                    if ( j->quantidadeAneis > 0 ) {
                        espalharAneis( j, NULL );
                        PlaySound( rm.somHitComAnel );
                    } else {
                        perderVida( j, NULL );
                        PlaySound( rm.somMorte );
                    }
                    j->invulneravel = true;
                }

                return;

            }

        } else if ( inimigo->tipo == TIPO_INIMIGO_SPIKES ) {

            InimigoSpikes *spikes = (InimigoSpikes*) inimigo->objeto;

            if ( !spikes->ativo || spikes->estado == ESTADO_INIMIGO_SPIKES_MORRENDO ) {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoSpikes( spikes );
            olhandoParaDireita = &spikes->olhandoParaDireita;
            ret = &spikes->ret;

            float deslocamentoX = *olhandoParaDireita
                ? ret->width - qaInimigo->retColisao.x - qaInimigo->retColisao.width
                : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX,
                ret->y + deslocamentoY,
                qaInimigo->retColisao.width,
                qaInimigo->retColisao.height
            };

            if ( CheckCollisionRecs( retColCalculado, retColInimigoCalculado ) ) {

                if ( j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO ) {
                    j->vel.y = j->velPulo;
                    spikes->estado = ESTADO_INIMIGO_SPIKES_MORRENDO;
                    adicionarPontuacao( j, 100 );
                    PlaySound( rm.somHitInimigo );
                } else if ( !j->invulneravel ) {
                    if ( j->quantidadeAneis > 0 ) {
                        espalharAneis( j, NULL );
                        PlaySound( rm.somHitComAnel );
                    } else {
                        perderVida( j, NULL );
                        PlaySound( rm.somMorte );
                    }
                    j->invulneravel = true;
                }

                return;

            }

        }

        el = el->proximo;

    }

}

// ========================================================================
// SISTEMAS DE PONTUAÇÃO, TEMPO E VIDAS
// ========================================================================

/**
 * @brief Adiciona pontos à pontuação do jogador e verifica vida extra.
 */
void adicionarPontuacao( Jogador *j, int pontos ) {
    j->pontuacao += pontos;
    
    if ( j->pontuacao >= j->proximoBonusVida ) {
        if ( j->quantidadeVidas < 99 ) {
            j->quantidadeVidas++;
            TraceLog( LOG_INFO, "Vida extra! Total de vidas: %d", j->quantidadeVidas );
        }
        j->proximoBonusVida += 50000;
    }
    
    TraceLog( LOG_INFO, "Pontuação: %d (+%d)", j->pontuacao, pontos );
}

/**
 * @brief Coleta anéis e adiciona pontuação.
 */
void coletarAnel( Jogador *j, int quantidade ) {
    j->quantidadeAneis += quantidade;
    adicionarPontuacao( j, quantidade * 10 );
    TraceLog( LOG_INFO, "Coletou %d anel(is). Total: %d anéis, Pontuação: %d", 
              quantidade, j->quantidadeAneis, j->pontuacao );
}

/**
 * @brief Remove uma vida do jogador. Se chegar a 0, game over.
 */
void perderVida( Jogador *j, GameWorld *gw ) {
    j->quantidadeVidas--;
    
    if ( j->quantidadeVidas <= 0 ) {
        j->quantidadeVidas = 0;
        TraceLog( LOG_INFO, "GAME OVER! Não restam vidas." );
        // Aqui você pode chamar uma função de game over
        // gameOver(gw);
    } else {
        // Reseta anéis (no estilo Sonic, perde todos os anéis ao morrer)
        j->quantidadeAneis = 0;
        j->vel.x = 0;
        j->vel.y = 0;
        TraceLog( LOG_INFO, "Vida perdida! Vidas restantes: %d, Anéis zerados.", j->quantidadeVidas );
    }
}

/**
 * @brief Espalha os anéis quando o jogador toma dano (estilo Sonic).
 */
void espalharAneis( Jogador *j, GameWorld *gw ) {
    if ( j->quantidadeAneis <= 0 ) return;
    
    // Quantidade máxima de anéis que podem ser espalhados (clássico: 32)
    int maxAneis = 32;
    int aneisParaEspalhar = (j->quantidadeAneis > maxAneis) ? maxAneis : j->quantidadeAneis;
    
    // Garante no mínimo 16 anéis (comportamento clássico)
    if ( aneisParaEspalhar < 16 && j->quantidadeAneis >= 16 ) {
        aneisParaEspalhar = 16;
    }
    
    TraceLog( LOG_INFO, "Perdeu %d anéis! (Quantidade anterior: %d)", 
              aneisParaEspalhar, j->quantidadeAneis );
    
    // TODO: Criar objetos anel voando para fora do jogador
    // Por enquanto, apenas reseta os anéis
    
    // Zera os anéis do jogador
    j->quantidadeAneis = 0;
}

/**
 * @brief Atualiza o cronômetro da fase.
 */
void atualizarCronometro( Jogador *j, float delta, GameWorld *gw ) {
    // Só atualiza se o jogador estiver vivo
    if ( j->quantidadeVidas <= 0 ) return;
    
    j->tempoFase += delta;
    
    // Converte para minutos e segundos para exibição
    int totalSegundos = (int)j->tempoFase;
    j->minutos = totalSegundos / 60;
    j->segundos = totalSegundos % 60;
    
    // Morte por tempo limite (9:59 = 599 segundos)
    if ( totalSegundos >= 599 ) {
        TraceLog( LOG_WARNING, "TIME OVER! Perdeu uma vida." );
        perderVida( j, gw );
    }
}

/**
 * @brief Calcula o bônus de fim de fase (anéis + tempo).
 * @return Pontos totais do bônus
 */
int calcularBonusFimDeFase( Jogador *j ) {
    // Bônus por anéis: cada anel vale 100 pontos
    int bonusAneis = j->quantidadeAneis * 100;
    
    // Bônus por tempo: tempo restante (600 segundos - tempo usado)
    int tempoMaximo = 600;  // 10 minutos = 600 segundos
    int tempoRestante = tempoMaximo - (int)j->tempoFase;
    
    if ( tempoRestante < 0 ) tempoRestante = 0;
    
    // 50 pontos por segundo restante
    int bonusTempo = tempoRestante * 50;
    
    int bonusTotal = bonusAneis + bonusTempo;
    
    TraceLog( LOG_INFO, "Bônus fim de fase: Anéis=%d (%d pts), Tempo=%d seg (%d pts), Total=%d", 
              j->quantidadeAneis, bonusAneis, tempoRestante, bonusTempo, bonusTotal );
    
    return bonusTotal;
}

/**
 * @brief Aplica o bônus de fim de fase à pontuação.
 */
void aplicarBonusFimDeFase( Jogador *j ) {
    int bonus = calcularBonusFimDeFase( j );
    adicionarPontuacao( j, bonus );
    TraceLog( LOG_INFO, "Bônus aplicado! Nova pontuação: %d", j->pontuacao );
}
