/**
 * @file test_Scenario.cpp
 * @brief test_Scenarioimplementação do projeto Traveling Dragon.
 */


#include "doctest.h"           // Inclui o cabeçalho do Doctest.
#include "../include/Scenario.hpp" // Inclui a classe Scenario.

// Ponteiros nulos para bitmaps e fontes, usados como "dummies" para instanciar Scenario
// quando o foco do teste não é o carregamento de assets.
static ALLEGRO_BITMAP* dummyBmp = nullptr;
static ALLEGRO_FONT* dummyFont = nullptr;
static ALLEGRO_SAMPLE* dummySample = nullptr;

/**
 * @brief Caso de teste para verificar se o método `reset()` da classe Scenario
 * zera a pontuação e redefine o estado de Game Over.
 *
 * @details Este teste:
 * - Cria uma instância de Scenario.
 * - Chama o método `reset()`.
 * - Verifica se a pontuação é 0.
 * - Verifica se o estado de Game Over é falso.
 */
TEST_CASE("Reset zera o score e game over") {
    // Instancia Scenario com dados dummy para focar no comportamento do reset.
    Scenario scenario(dummyBmp, dummyBmp, dummyBmp, dummyFont, 800, 600, dummySample, dummySample);

    // Garante que o cenário está resetado e nas condições iniciais esperadas para um novo jogo.
    scenario.reset();
    CHECK(scenario.getScore() == 0);      // Verifica se a pontuação foi zerada.
    CHECK_FALSE(scenario.isGameOver());   // Verifica se o jogo não está em Game Over.
}

/**
 * @brief Caso de teste básico para o método `increaseSpeedByPercent()`.
 *
 * @details Este teste verifica apenas se o método pode ser chamado sem causar
 * erros, já que não há um getter direto para a velocidade interna
 * do cano no Scenario para uma verificação mais profunda.
 * Serve como um teste de sanidade e compilação.
 */
TEST_CASE("Aumentar velocidade dos canos") {
    Scenario scenario(dummyBmp, dummyBmp, dummyBmp, dummyFont, 800, 600, dummySample, dummySample);
    scenario.reset();

    // Como não há um getter público para 'currentPipeSpeed',
    // este teste apenas verifica se a chamada do método não resulta em erro de compilação/runtime.
    // Em um teste mais completo, você idealmente verificaria o impacto da velocidade,
    // se houvesse uma forma de observá-la.
    scenario.increaseSpeedByPercent(0.1f); // Aumenta a velocidade em 0.1%
    CHECK(true); // Apenas para indicar que o teste foi executado e não houve falha imediata.
}