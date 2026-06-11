/**
 * @file Jogador.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Jogador.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

typedef struct Jogador Jogador;

/**
 * @brief Cria uma instância alocada dinamicamente da struct Jogador.
 */
Jogador *criarJogador( float x, float y, float w, float h );

/**
 * @brief Destrói um objeto Jogador e libera seus recursos.
 */
void destruirJogador( Jogador *j );

/**
 * @brief Lê a entrada do usuário e atualiza as velocidades do jogador.
 */
void entradaJogador( Jogador *j, float delta );

/**
 * @brief Aplica física e resolve colisões do jogador com o mundo.
 */
void atualizarJogador( Jogador *j, GameWorld *gw, float delta );

/**
 * @brief Desenha o jogador.
 */
void desenharJogador( Jogador *j );

// Declarações das funções de sistemas
void adicionarPontuacao( Jogador *j, int pontos );
void coletarAnel( Jogador *j, int quantidade );
void perderVida( Jogador *j, GameWorld *gw );
void espalharAneis( Jogador *j, GameWorld *gw );
void atualizarCronometro( Jogador *j, float delta, GameWorld *gw );
int calcularBonusFimDeFase( Jogador *j );
void aplicarBonusFimDeFase( Jogador *j );

struct Jogador {
    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velAndandoRapido;
    float velCorrendo;
    float velPulo;
    float velMaxQueda;

    float aceleracao;
    float desaceleracao;
    float frenagem;

    int quantidadePulos;
    int quantidadeMaxPulos;

    // ===== SISTEMA DE ANÉIS =====
    int quantidadeAneis;

    // ===== SISTEMA DE PONTUAÇÃO =====
    int pontuacao;
    int proximoBonusVida;

    // ===== SISTEMA DE TEMPO =====
    float tempoFase;
    int minutos;
    int segundos;

    // ===== SISTEMA DE VIDAS =====
    int quantidadeVidas;

    bool invulneravel;
    float tempoInvulnerabilidade;
    float contadorTempoInvulnerabilidade;

    bool piscaPisca;
    float tempoPiscaPisca;
    float contadorTempoPiscaPisca;

    bool freando;

    float velocidadeDanoQueda;

    EstadoJogador estado;
    bool olhandoParaDireita;

    Animacao animacaoParado;
    Animacao animacaoAndando;
    Animacao animacaoAndandoRapido;
    Animacao animacaoCorrendo;
    Animacao animacaoPulando;
    Animacao animacaoPulandoRapido;
    Animacao animacaoPulandoCorrendo;

    Animacao *animacoes[8];
    int quantidadeAnimacoes;
};