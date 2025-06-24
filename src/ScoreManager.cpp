/**
 * @file ScoreManager.cpp
 * @brief ScoreManagerimplementação do projeto Traveling Dragon.
 */


#include "ScoreManager.hpp"

/**
 * @brief Construtor da classe ScoreManager.
 * Inicializa a pontuação com 0 ao ser criado.
 */
ScoreManager::ScoreManager() {
    reset(); // Chama reset para garantir que a pontuação começa em zero
}

/**
 * @brief Reseta a pontuação para zero.
 * Usado no início de cada nova partida.
 */
void ScoreManager::reset() {
    score = 0;
}

/**
 * @brief Incrementa a pontuação em 1.
 * Chamado quando o jogador marca um ponto (ex: passa por um cano).
 */
void ScoreManager::increment() {
    score++;
}

/**
 * @brief Retorna a pontuação atual.
 * @return O valor atual da pontuação.
 */
int ScoreManager::getScore() const {
    return score;
}