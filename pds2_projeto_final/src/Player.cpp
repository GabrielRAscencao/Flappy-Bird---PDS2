#include "Player.hpp"

Player::Player(std::string nome, std::string apelido)
    : nome(nome), apelido(apelido), partidas(0), maiorPontuacao(0) {}

std::string Player::getNome() const { return nome; }
std::string Player::getApelido() const { return apelido; }
int Player::getPartidas() const { return partidas; }
int Player::getMaiorPontuacao() const { return maiorPontuacao; }

void Player::adicionarPartida(int pontuacao) {
    partidas++;
    if (pontuacao > maiorPontuacao)
        maiorPontuacao = pontuacao;
}

std::string Player::toString() const {
    return apelido + " (" + nome + ") - Pontuação: " + std::to_string(maiorPontuacao) + " | Partidas: " + std::to_string(partidas);
}
