/**
 * @file GameWorld.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do GameWorld.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "GameWorld.h"
#include "Jogador.h"
#include "Macros.h"
#include "Mapa.h"
#include "Obstaculo.h"
#include "Tipos.h"
#include "ResourceManager.h"

#include "raylib/raylib.h"
//#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

static void desenharFundo( GameWorld *gw );
static void desenharAgua( GameWorld *gw );
static void desenharMola( Rectangle mola, Color corPrincipal, float compressao );
static void desenharMolas( GameWorld *gw );
static void desenharMenu( GameWorld *gw );
static void atualizarCamera( GameWorld *gw );

static void inicializar( GameWorld *gw );
static void reiniciar( GameWorld *gw );

static const char *NOMES_MAPAS[] = {
    "Green Hill original",
    "Fase da agua",
    "Mapa de teste"
};

static const char *ARQUIVOS_MAPAS[] = {
    "resources/mapas/mapa01.txt",
    "resources/mapas/mapaFigura.txt",
    "resources/mapas/mapaTeste.txt"
};

static const int QUANTIDADE_MAPAS = 3;

/**
 * @brief Cria uma instância alocada dinamicamente da struct GameWorld.
 */
GameWorld *createGameWorld( void ) {
    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    gw->mapa = NULL;
    gw->jogador = NULL;
    gw->menuAtivo = true;
    gw->mapaSelecionado = 1;
    inicializar( gw );
    return gw;
}

/**
 * @brief Destrói um objeto GameWorld e suas dependências.
 */
void destroyGameWorld( GameWorld *gw ) {
    if ( gw != NULL ) {
        destruirMapa( gw->mapa );
        destruirJogador( gw->jogador );
        free( gw );
    }
}

/**
 * @brief Lê a entrada do usuário e atualiza o estado do jogo.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    if ( !IsMusicStreamPlaying( rm.musicaFase01 ) ) {
        PlayMusicStream( rm.musicaFase01 );
    } else {
        UpdateMusicStream( rm.musicaFase01 );
    }

    if ( IsKeyPressed( KEY_R ) ) {
        reiniciar( gw );
        return;
    }

    if ( gw->menuAtivo ) {
        if ( IsKeyPressed( KEY_DOWN ) ) {
            gw->mapaSelecionado = ( gw->mapaSelecionado + 1 ) % QUANTIDADE_MAPAS;
        } else if ( IsKeyPressed( KEY_UP ) ) {
            gw->mapaSelecionado--;
            if ( gw->mapaSelecionado < 0 ) {
                gw->mapaSelecionado = QUANTIDADE_MAPAS - 1;
            }
        } else if ( IsKeyPressed( KEY_ONE ) ) {
            gw->mapaSelecionado = 0;
        } else if ( IsKeyPressed( KEY_TWO ) ) {
            gw->mapaSelecionado = 1;
        } else if ( IsKeyPressed( KEY_THREE ) ) {
            gw->mapaSelecionado = 2;
        }

        if ( IsKeyPressed( KEY_ENTER ) || IsKeyPressed( KEY_SPACE ) ) {
            gw->menuAtivo = false;
            reiniciar( gw );
        }

        return;
    }

    Jogador *j = gw->jogador;

    if ( gw->jogadorNaAgua ) {
        gw->contadorRestartAgua += delta;
        j->vel.x = 0;
        j->vel.y = 140;
        j->ret.y += j->vel.y * delta;
        atualizarCamera( gw );

        if ( gw->contadorRestartAgua >= 1.0f ) {
            reiniciar( gw );
        }

        return;
    }

    atualizarCronometro( j, delta, gw );
    if ( j->quantidadeVidas <= 0 ) {
        reiniciar( gw );
        return;
    }

    atualizarMapa( gw->mapa, gw, delta );
    entradaJogador( j, delta );
    atualizarJogador( j, gw, delta );

    for ( int i = 0; i < gw->quantidadeMolas; i++ ) {
        if ( j->vel.y >= 0 && CheckCollisionRecs( j->ret, gw->molas[i] ) ) {
            j->ret.y = gw->molas[i].y - j->ret.height;
            gw->compressaoMolas[i] = 18.0f;
            j->vel.y = -700;
            j->quantidadePulos = 0;
            j->estado = ESTADO_JOGADOR_PULANDO;
            PlaySound( rm.somMola );
        } else if ( gw->compressaoMolas[i] > 0.0f ) {
            gw->compressaoMolas[i] -= 60.0f * delta;
            if ( gw->compressaoMolas[i] < 0.0f ) {
                gw->compressaoMolas[i] = 0.0f;
            }
        }
    }

    if ( gw->temAgua && j->ret.y + j->ret.height > gw->agua.y + 12 ) {
        PlaySound( rm.somAgua );
        gw->jogadorNaAgua = true;
        gw->contadorRestartAgua = 0.0f;
        j->vel.x = 0;
        j->vel.y = 140;
        return;
    }
    if ( j->quantidadeVidas <= 0 ) {
        reiniciar( gw );
        return;
    }

    atualizarCamera( gw );
}

/**
 * @brief Desenha o estado do jogo.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( (Color) { 36, 0, 180, 255 } );

    if ( gw->menuAtivo ) {
        desenharMenu( gw );
        EndDrawing();
        return;
    }

    BeginMode2D( gw->camera );
    desenharFundo( gw );
    desenharAgua( gw );
    desenharMapa( gw->mapa );
    desenharMolas( gw );
    desenharJogador( gw->jogador );
    if ( gw->jogadorNaAgua ) {
        DrawRectangleRec( gw->agua, (Color) { 0, 156, 252, 150 } );
    }
    EndMode2D();

    Jogador *j = gw->jogador;
    int largura = GetScreenWidth();

    // ===== HUD ESTILO SONIC =====
    DrawText( TextFormat( "RINGS  %d", j->quantidadeAneis ), largura - 200, 10, 24, ORANGE );
    DrawText( TextFormat( "SCORE  %06d", j->pontuacao ), largura - 200, 40, 24, ORANGE );
    DrawText( TextFormat( "TIME   %02d:%02d", j->minutos, j->segundos ), largura - 200, 70, 24, ORANGE );
    DrawText( "LIVES", 10, 10, 20, ORANGE );
    for ( int i = 0; i < j->quantidadeVidas && i < 9; i++ ) {
        DrawRectangle( 70 + i * 35, 10, 25, 25, Fade( RED, 0.5f ) );
    }
    DrawText( TextFormat( "VEL: %.1f/%.1f", j->vel.x, j->vel.y ), 10, GetScreenHeight() - 60, 16, ORANGE );
    DrawText( TextFormat( "STATE: %d", j->estado ), 10, GetScreenHeight() - 40, 16, ORANGE );
    DrawText( TextFormat( "INV: %.1f/%.1f", j->contadorTempoInvulnerabilidade, j->tempoInvulnerabilidade ), 10, GetScreenHeight() - 20, 16, ORANGE );
    DrawFPS( 10, GetScreenHeight() - 80 );

    EndDrawing();
}

static void desenharMenu( GameWorld *gw ) {

    int largura = GetScreenWidth();
    int x = largura / 2 - 170;
    int y = 120;

    DrawText( "ESCOLHA O MAPA", x, 60, 32, ORANGE );

    for ( int i = 0; i < QUANTIDADE_MAPAS; i++ ) {
        Color cor = i == gw->mapaSelecionado ? YELLOW : WHITE;
        const char *marcador = i == gw->mapaSelecionado ? ">" : " ";
        DrawText( TextFormat( "%s %d - %s", marcador, i + 1, NOMES_MAPAS[i] ), x, y + i * 42, 24, cor );
    }

    DrawText( "Use SETAS ou 1/2/3", x, y + 150, 20, LIGHTGRAY );
    DrawText( "ENTER para jogar", x, y + 180, 20, LIGHTGRAY );

}

static void desenharMolas( GameWorld *gw ) {

    for ( int i = 0; i < gw->quantidadeMolas; i++ ) {
        desenharMola( gw->molas[i], i % 2 == 0 ? RED : YELLOW, gw->compressaoMolas[i] );
    }

}

static void desenharMola( Rectangle mola, Color corPrincipal, float compressao ) {

    float x = mola.x;
    float y = mola.y;
    float w = mola.width;
    float h = mola.height;
    Color sombra = (Color) { 80, 80, 80, 255 };
    Color metal = (Color) { 220, 220, 220, 255 };

    compressao = compressao < 0.0f ? 0.0f : compressao;
    compressao = compressao > 18.0f ? 18.0f : compressao;

    float baseY = mola.y + mola.height - 8;
    float espacamento = 8.0f;

    DrawRectangle( x, y, w, 8, corPrincipal );
    DrawRectangle( x, baseY, w, 8, corPrincipal );
    DrawRectangleLines( x, y, w, h, BLACK );

    for ( float iy = y + 10; iy < baseY - 4; iy += espacamento ) {
        DrawLine( x + 6, iy, x + w - 6, iy + 4, metal );
        DrawLine( x + 6, iy + 2, x + w - 6, iy + 6, sombra );
    }

}

static void desenharAgua( GameWorld *gw ) {

    if ( !gw->temAgua ) {
        return;
    }

    DrawRectangleRec( gw->agua, (Color) { 0, 156, 252, 255 } );

    for ( int y = (int) gw->agua.y + 8; y < (int) ( gw->agua.y + gw->agua.height ); y += 14 ) {
        for ( int x = (int) gw->agua.x; x < (int) ( gw->agua.x + gw->agua.width ); x += 38 ) {
            float onda = sinf( ( x + y ) * 0.05f ) * 4.0f;
            DrawLine( x, y + (int) onda, x + 18, y + (int) onda, (Color) { 180, 240, 255, 180 } );
        }
    }

    DrawRectangle( gw->agua.x, gw->agua.y, gw->agua.width, 6, (Color) { 160, 240, 255, 220 } );

}

static void desenharFundo( GameWorld *gw ) {

    int larguraFundo = rm.texturaFundo.width;
    int larguraMapa = calcularLarguraMapa( gw->mapa );
    int alturaMapa = calcularAlturaMapa( gw->mapa );
    int repeticoes = larguraMapa / larguraFundo;

    int deslocamentoParallax = (int) ( ( gw->camera.target.x / (float) larguraMapa ) * 200 );

    for ( int i = 0; i <= repeticoes; i++ ) {
        DrawTexture( rm.texturaFundo, larguraFundo * i - deslocamentoParallax, alturaMapa - rm.texturaFundo.height, WHITE );
    }

}

static void atualizarCamera( GameWorld *gw ) {

    Jogador *j = gw->jogador;
    Camera2D *c = &gw->camera;

    c->offset.x = GetScreenWidth() / 2;
    c->offset.y = GetScreenHeight() / 2;

    // O target é arredondado para o inteiro mais próximo para garantir que a
    // translação da câmera ocorra sempre em posições inteiras de pixel. Sem esse
    // arredondamento, o valor float contínuo de ret.x faz os tiles serem
    // renderizados em posições subpixel, causando frestas visíveis entre eles.
    c->target.x = roundf( j->ret.x + j->ret.width / 2.0f );
    c->target.y = roundf( j->ret.y + j->ret.height / 2.0f );

    int minX = GetScreenWidth() / 2;
    int maxX = calcularLarguraMapa( gw->mapa ) - GetScreenWidth() / 2;
    int maxY = calcularAlturaMapa( gw->mapa ) - GetScreenHeight() / 2;

    if ( c->target.x < minX ) {
        c->target.x = minX;
    } else if ( c->target.x > maxX ) {
        c->target.x = maxX;
    }

    if ( c->target.y > maxY ) {
        c->target.y = maxY;
    }

}

static void inicializar( GameWorld *gw ) {

    if ( gw->mapa ) {
        destruirMapa( gw->mapa );
        gw->mapa = NULL;
    }
    if ( gw->jogador ) {
        destruirJogador( gw->jogador );
        gw->jogador = NULL;
    }

    gw->mapa = carregarMapa( ARQUIVOS_MAPAS[gw->mapaSelecionado] );

    if ( !gw->mapa ) {
        return;
    }

    gw->jogador = criarJogador( GetScreenWidth() / 2 + 144, calcularAlturaMapa( gw->mapa ) - 196, 96, 96 );
    gw->camera = (Camera2D){
        .offset = { 0 },
        .target = { 0 },
        .rotation = 0.0f,
        .zoom = 1.0f
    };

    gw->gravidade = 900;
    gw->temAgua = gw->mapaSelecionado == 1;
    gw->jogadorNaAgua = false;
    gw->contadorRestartAgua = 0.0f;
    gw->quantidadeMolas = 0;
    gw->velocidadeLancamentoMola = 1200.0f;
    for ( int i = 0; i < 16; i++ ) {
        gw->compressaoMolas[i] = 0.0f;
    }
    gw->agua = (Rectangle){
        .x = 0,
        .y = calcularAlturaMapa( gw->mapa ) - 96,
        .width = calcularLarguraMapa( gw->mapa ),
        .height = 96
    };

    if ( gw->temAgua ) {
        gw->quantidadeMolas = 7;
        gw->molas[0] = (Rectangle) { 420, 420, 34, 48 };
        gw->molas[1] = (Rectangle) { 860, 420, 34, 48 };
        gw->molas[2] = (Rectangle) { 1300, 420, 34, 48 };
        gw->molas[3] = (Rectangle) { 1660, 660, 34, 48 };
        gw->molas[4] = (Rectangle) { 2050, 660, 34, 48 };
        gw->molas[5] = (Rectangle) { 2420, 660, 34, 48 };
        gw->molas[6] = (Rectangle) { 2700, 420, 34, 48 };
    }

}

static void reiniciar( GameWorld *gw ) {

    if ( gw ) {
        if ( gw->mapa ) {
            destruirMapa( gw->mapa );
            gw->mapa = NULL;
        }
        if ( gw->jogador ) {
            destruirJogador( gw->jogador );
            gw->jogador = NULL;
        }
    }

    inicializar( gw );

}
