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
static void atualizarCamera( GameWorld *gw );

static void inicializar( GameWorld *gw );
static void reiniciar( GameWorld *gw );

/**
 * @brief Cria uma instância alocada dinamicamente da struct GameWorld.
 */
GameWorld *createGameWorld( void ) {
    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
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

    Jogador *j = gw->jogador;

    atualizarCronometro( j, delta, gw );
    if ( j->quantidadeVidas <= 0 ) {
        reiniciar( gw );
        return;
    }

    atualizarMapa( gw->mapa, gw, delta );
    entradaJogador( j, delta );
    atualizarJogador( j, gw, delta );
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

    BeginMode2D( gw->camera );
    desenharFundo( gw );
    desenharMapa( gw->mapa );
    desenharJogador( gw->jogador );
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

    //gw->mapa = carregarMapa( "resources/mapas/mapaTeste.txt" );
    gw->mapa = carregarMapa( "resources/mapas/mapa01.txt" );
    gw->jogador = criarJogador( GetScreenWidth() / 2 + 144, calcularAlturaMapa( gw->mapa ) - 196, 96, 96 );

    gw->camera = (Camera2D) {
        .offset = { 0 },    // deslocamento relativo da câmera em relação ao alvo
        .target = { 0 },    // o alvo da câmera, ou seja, a coordenada em que ela está centralizada
        .rotation = 0.0f,   // rotação da câmera em graus. o pivô é o alvo.
        .zoom = 1.0f        // zoom da câmera. 1.0f significa sem escala
    };

    gw->gravidade = 900;

}

static void reiniciar( GameWorld *gw ) {

    destruirMapa( gw->mapa );
    destruirJogador( gw->jogador );

    if ( IsMusicStreamPlaying( rm.musicaFase01 ) ) {
        StopMusicStream( rm.musicaFase01 );
    }

    inicializar( gw );

}
