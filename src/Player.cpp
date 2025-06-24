/**
 * @file Player.cpp
 * @brief Playerimplementação do projeto Traveling Dragon.
 */


#include "Player.hpp"

/**
 * @brief Construtor da classe Player.
 * @param nome O nome real do jogador.
 * @param apelido O apelido (nickname) do jogador.
 */
Player::Player(std::string nome, std::string apelido)
    : nome(nome), apelido(apelido), partidas(0), maiorPontuacao(0) {}

/**
 * @brief Retorna o nome real do jogador.
 * @return O nome do jogador.
 */
std::string Player::getNome() const { return nome; }

/**
 * @brief Retorna o apelido do jogador.
 * @return O apelido do jogador.
 */
std::string Player::getApelido() const { return apelido; }

/**
 * @brief Retorna o número total de partidas jogadas pelo jogador.
 * @return O número de partidas.
 */
int Player::getPartidas() const { return partidas; }

/**
 * @brief Retorna a maior pontuação alcançada pelo jogador.
 * @return A maior pontuação.
 */
int Player::getMaiorPontuacao() const { return maiorPontuacao; }

/**
 * @brief Adiciona uma partida ao registro do jogador e atualiza a maior pontuação.
 * @param pontuacao A pontuação obtida na partida que acabou de ser jogada.
 */
void Player::adicionarPartida(int pontuacao) {
    partidas++; // Incrementa o contador de partidas
    // Atualiza a maior pontuação se a pontuação atual for maior
    if (pontuacao > maiorPontuacao)
        maiorPontuacao = pontuacao;
}

/**
 * @brief Retorna uma representação em string formatada dos dados do jogador.
 * @return Uma string contendo apelido, nome, maior pontuação e número de partidas.
 */
std::string Player::toString() const {
    return apelido + " (" + nome + ") - Pontuação: " + std::to_string(maiorPontuacao) + " | Partidas: " + std::to_string(partidas);
}