/**
 * @file Mapa.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Mapa.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Mapa.h"
#include "Inimigo.h"
#include "InimigoMotobug.h"
#include "InimigoPeixe.h"
#include "InimigoSpikes.h"
#include "Item.h"
#include "ItemAnel.h"
#include "ItemAnelAzul.h"
#include "ItemEscudo.h"
#include "Obstaculo.h"
#include "Tipos.h"
#include "ResourceManager.h"

static void inserirObstaculo( Mapa *mapa, ElementoMapa *obstaculo );
static void inserirItem( Mapa *mapa, ElementoMapa *item );
static void inserirInimigo( Mapa *mapa, ElementoMapa *inimigo );
static void inserirFim( Mapa *mapa, Rectangle ret );

/**
 * @brief Carrega um mapa a partir de uma arquivo.
 */
Mapa *carregarMapa( const char *caminhoArquivo ) {

    // aloca um novo mapa
    Mapa *novoMapa = (Mapa*) malloc( sizeof( Mapa ) );

    novoMapa->obstaculos = NULL;
    novoMapa->quantidadeObstaculos = 0;

    novoMapa->itens = NULL;
    novoMapa->quantidadeItens = 0;

    novoMapa->inimigos = NULL;
    novoMapa->quantidadeInimigos = 0;

    novoMapa->finais = NULL;
    novoMapa->quantidadeFinais = 0;

    novoMapa->dimensaoPadraoElementos = 48;
    novoMapa->linhas = 0;
    novoMapa->colunas = 0;
    
    // carrega dados do arquivo de texto
    char *dadosMapa = LoadFileText( caminhoArquivo );

    // marcadores para processamento do mapa
    char *caractereAtual = dadosMapa;
    int linhaAtual = 0;
    int colunaAtual = 0;

    // caractere atual marca inicialmente a primeira posição de dadosMapa
    // C-strings terminam em '\0', sendo assim, caminhamos caractere por 
    // caractere até o fim
    while ( *caractereAtual != '\0' ) {

        char c = *caractereAtual;

        // fim de linha?
        if ( c == '\n' ) {

            linhaAtual++;
            colunaAtual = 0;

            novoMapa->linhas = linhaAtual;

        } else {

            if ( c != ' ' ) {

                ElementoMapa *el = (ElementoMapa*) malloc( sizeof( ElementoMapa ) );
                el->proximo = NULL;

                if ( c == 'E' ) {

                    inserirFim(
                        novoMapa,
                        (Rectangle) {
                            .x = novoMapa->dimensaoPadraoElementos * colunaAtual,
                            .y = novoMapa->dimensaoPadraoElementos * linhaAtual,
                            .width = novoMapa->dimensaoPadraoElementos,
                            .height = novoMapa->dimensaoPadraoElementos
                        }
                    );

                } else if ( c >= 'A' && c <= 'Z' ) {

                    int deslocamento = c - 'A';

                    el->objeto = criarObstaculo( 
                        (Rectangle) { 
                            .x = novoMapa->dimensaoPadraoElementos * colunaAtual, 
                            .y = novoMapa->dimensaoPadraoElementos * linhaAtual, 
                            .width = novoMapa->dimensaoPadraoElementos, 
                            .height = novoMapa->dimensaoPadraoElementos
                        },
                        GRAY,
                        (Rectangle) { 
                            1 + ( novoMapa->dimensaoPadraoElementos + 1 ) * deslocamento, 
                            1, 
                            novoMapa->dimensaoPadraoElementos,
                            novoMapa->dimensaoPadraoElementos
                        },
                        &rm.texturaTerreno
                    );

                    el->tipo = TIPO_ELEMENTO_MAPA_OBSTACULO;

                    inserirObstaculo( novoMapa, el );

                } else if ( c >= 'a' && c <= 'z' ) {

                    Item *item = NULL;

                    switch ( c ) {

                        case 'a':

                            item = criarItem( TIPO_ITEM_ANEL );

                            item->objeto = criarItemAnel( 
                                (Rectangle) { 
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual + 8, 
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual + 5, 
                                    .width = 32, 
                                    .height = 32
                                },
                                YELLOW
                            );

                            el->objeto = item;
                            el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                            break;

                        case 'b':

                            item = criarItem( TIPO_ITEM_ANEL_AZUL );

                            item->objeto = criarItemAnelAzul( 
                                (Rectangle) { 
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual + 8, 
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual + 5, 
                                    .width = 32, 
                                    .height = 32
                                },
                                SKYBLUE
                            );

                            el->objeto = item;
                            el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                            break;

                        case 'c':

                            item = criarItem( TIPO_ITEM_ESCUDO );

                            item->objeto = criarItemEscudo(
                                (Rectangle){
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual,
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual,
                                    .width = 32,
                                    .height = 32
                                },
                                SKYBLUE
                            );

                            el->objeto = item;
                            el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                            break;

                        default:
                            TraceLog( LOG_ERROR, "Tipo de item desconhecido." );
                            abort();
                            break;
                    }

                    inserirItem( novoMapa, el );

                } else if ( c >= '0' && c <= '9' ) {

                    Inimigo *inimigo = NULL;

                    switch ( c ) {

                        case '0':

                            inimigo = criarInimigo( TIPO_INIMIGO_MOTOBUG );

                            inimigo->objeto = criarInimigoMotobug( 
                                (Rectangle) { 
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual, 
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual - 12, 
                                    .width = 80, 
                                    .height = 60
                                },
                                YELLOW
                            );

                            el->objeto = inimigo;
                            el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                            break;

                        case '1':

                            inimigo = criarInimigo( TIPO_INIMIGO_SPIKES );

                            inimigo->objeto = criarInimigoSpikes(
                                (Rectangle) {
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual,
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual - 21,
                                    .width = 80,
                                    .height = 60
                                },
                                BLUE
                            );

                            el->objeto = inimigo;
                            el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                            break;

                        case '3':

                            inimigo = criarInimigo( TIPO_INIMIGO_PEIXE );

                            inimigo->objeto = criarInimigoPeixe(
                                (Rectangle) {
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual,
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual - 12,
                                    .width = 60,
                                    .height = 40
                                },
                                YELLOW,
                                novoMapa->dimensaoPadraoElementos * colunaAtual - 120,
                                novoMapa->dimensaoPadraoElementos * colunaAtual + 120,
                                novoMapa->dimensaoPadraoElementos * linhaAtual - 80,
                                novoMapa->dimensaoPadraoElementos * linhaAtual + 80
                            );

                            el->objeto = inimigo;
                            el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                            break;

                        case '2':

                            inimigo = criarInimigo( TIPO_INIMIGO_MOTOBUG );

                            inimigo->objeto = criarInimigoMotobug(
                                (Rectangle) {
                                    .x = novoMapa->dimensaoPadraoElementos * colunaAtual,
                                    .y = novoMapa->dimensaoPadraoElementos * linhaAtual - 12,
                                    .width = 80,
                                    .height = 60
                                },
                                YELLOW
                            );

                            el->objeto = inimigo;
                            el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                            break;

                        default:
                            TraceLog( LOG_ERROR, "Tipo de inimigo desconhecido." );
                            abort();
                            break;
                    }

                    inserirInimigo( novoMapa, el );

                } else {
                    TraceLog( LOG_ERROR, "Entidade inválida no mapa." );
                    abort();
                }

            }

            colunaAtual++;

            if ( novoMapa->colunas < colunaAtual ) {
                novoMapa->colunas = colunaAtual;
            }

        }

        caractereAtual++;

    }

    novoMapa->linhas++;
    
    // descarrega os dados
    UnloadFileText( dadosMapa );

    return novoMapa;

}

/**
 * @brief Destroi um mapa.
 */
void destruirMapa( Mapa *m ) {

    if ( m != NULL ) {
        
        ElementoMapa *el = NULL;
        
        el = m->obstaculos;
        while ( el != NULL ) {
            destruirObstaculo( (Obstaculo*) el->objeto );
            ElementoMapa *t = el;
            el = el->proximo;
            free( t );
        }

        el = m->itens;
        while ( el != NULL ) {
            destruirItem( (Item*) el->objeto );
            ElementoMapa *t = el;
            el = el->proximo;
            free( t );
        }

        el = m->inimigos;
        while ( el != NULL ) {
            destruirInimigo( (Inimigo*) el->objeto );
            ElementoMapa *t = el;
            el = el->proximo;
            free( t );
        }

        free( m->finais );

    }

}

/**
 * @brief Atualiza um mapa.
 */
void atualizarMapa( Mapa *m, GameWorld *gw, float delta ) {

    ElementoMapa *el = NULL;

    el = m->itens;
    while ( el != NULL ) {
        atualizarItem( (Item*) el->objeto, delta );
        el = el->proximo;
    }

    el = m->inimigos;
    while ( el != NULL ) {
        atualizarInimigo( (Inimigo*) el->objeto, gw, delta );
        el = el->proximo;
    }

}

/**
 * @brief Desenha um mapa.
 */
void desenharMapa( Mapa *m ) {

    ElementoMapa *el = NULL;

    el = m->obstaculos;
    while ( el != NULL ) {
        desenharObstaculo( (Obstaculo*) el->objeto );
        el = el->proximo;
    }

    el = m->itens;
    while ( el != NULL ) {
        desenharItem( (Item*) el->objeto );
        el = el->proximo;
    }

    el = m->inimigos;
    while ( el != NULL ) {
        desenharInimigo( (Inimigo*) el->objeto );
        el = el->proximo;
    }

    for ( int i = 0; i < m->quantidadeFinais; i++ ) {
        Rectangle fim = m->finais[i];
        DrawRectangleRec( fim, Fade( GREEN, 0.25f ) );
        DrawRectangleLines( fim.x, fim.y, fim.width, fim.height, GREEN );
        DrawText( "FIM", (int) fim.x + 10, (int) fim.y + 14, 18, WHITE );
    }

}

/**
 * @brief Calcula a largura do mapa.
 */
int calcularLarguraMapa( Mapa *m ) {
    return (int) ( m->dimensaoPadraoElementos * m->colunas );
}

/**
 * @brief Calcula a altura do mapa.
 */
int calcularAlturaMapa( Mapa *m ) {
    return (int) ( m->dimensaoPadraoElementos * m->linhas );
}

/**
 * @brief Insere um obstáculo na lista de obstáculos.
 */
static void inserirObstaculo( Mapa *mapa, ElementoMapa *obstaculo ) {
    if ( mapa->obstaculos == NULL ) {
        mapa->obstaculos = obstaculo;
    } else {
        obstaculo->proximo = mapa->obstaculos;
        mapa->obstaculos = obstaculo;
    }
    mapa->quantidadeObstaculos++;
}

/**
 * @brief Insere um item na lista de itens.
 */
static void inserirItem( Mapa *mapa, ElementoMapa *item ) {
    if ( mapa->itens == NULL ) {
        mapa->itens = item;
    } else {
        item->proximo = mapa->itens;
        mapa->itens = item;
    }
    mapa->quantidadeItens++;
}

/**
 * @brief Insere um inimigo na lista de inimigos.
 */
static void inserirInimigo( Mapa *mapa, ElementoMapa *inimigo ) {
    if ( mapa->inimigos == NULL ) {
        mapa->inimigos = inimigo;
    } else {
        inimigo->proximo = mapa->inimigos;
        mapa->inimigos = inimigo;
    }
    mapa->quantidadeInimigos++;
}

static void inserirFim( Mapa *mapa, Rectangle ret ) {
    mapa->quantidadeFinais++;
    mapa->finais = (Rectangle*) realloc( mapa->finais, mapa->quantidadeFinais * sizeof( Rectangle ) );
    mapa->finais[mapa->quantidadeFinais - 1] = ret;
}

/**
 * @brief Mata todos os inimigos do mapa.
 */
void matarTodosInimigos( Mapa *m ) {
    if ( m == NULL ) return;
    ElementoMapa *el = m->inimigos;
    while ( el != NULL ) {
        Inimigo *inimigo = (Inimigo*) el->objeto;
        if ( inimigo->tipo == TIPO_INIMIGO_MOTOBUG ) {
            InimigoMotobug *motobug = (InimigoMotobug*) inimigo->objeto;
            if ( motobug->estado == ESTADO_INIMIGO_MOTOBUG_ANDANDO ) {
                motobug->estado = ESTADO_INIMIGO_MOTOBUG_MORRENDO;
            }
        } else if ( inimigo->tipo == TIPO_INIMIGO_SPIKES ) {
            InimigoSpikes *spikes = (InimigoSpikes*) inimigo->objeto;
            if ( spikes->estado == ESTADO_INIMIGO_SPIKES_ANDANDO ) {
                spikes->estado = ESTADO_INIMIGO_SPIKES_MORRENDO;
            }
        } else if ( inimigo->tipo == TIPO_INIMIGO_PEIXE ) {
            InimigoPeixe *peixe = (InimigoPeixe*) inimigo->objeto;
            if ( peixe->estado == ESTADO_INIMIGO_PEIXE_NADANDO ) {
                peixe->estado = ESTADO_INIMIGO_PEIXE_MORRENDO;
            }
        }
        el = el->proximo;
    }
}
