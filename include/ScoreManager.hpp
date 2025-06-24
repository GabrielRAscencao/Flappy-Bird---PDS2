/**
 * @file ScoreManager.hpp
 * @brief ScoreManagerheader do projeto Traveling Dragon.
 */

#ifndef SCOREMANAGER_HPP
#define SCOREMANAGER_HPP

/**
 * @brief Gerencia a pontuação do jogador durante uma partida.
 *
 * Esta classe é responsável por controlar a pontuação, permitindo
 * incrementá-la e resetá-la, além de fornecer o valor atual da pontuação.
 */
class ScoreManager {
private:
    int score; ///< @brief A pontuação atual do jogador na partida.

public:
    /**
     * @brief Construtor da classe ScoreManager.
     * Inicializa a pontuação com zero.
     */
    ScoreManager();

    /**
     * @brief Reseta a pontuação para zero.
     * Deve ser chamado no início de cada nova partida.
     */
    void reset();

    /**
     * @brief Incrementa a pontuação em uma unidade.
     * Chamado toda vez que o jogador marca um ponto no jogo.
     */
    void increment();

    /**
     * @brief Retorna a pontuação atual.
     * @return O valor inteiro da pontuação atual.
     */
    int getScore() const;
};

#endif