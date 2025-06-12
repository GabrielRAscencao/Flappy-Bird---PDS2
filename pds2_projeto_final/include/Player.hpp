#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>

class Player {
private:
    std::string nome;
    std::string apelido;
    int partidas;
    int maiorPontuacao;

public:
    Player(std::string nome, std::string apelido);

    std::string getApelido() const;
    std::string getNome() const;
    int getPartidas() const;
    int getMaiorPontuacao() const;

    void adicionarPartida(int pontuacao);
    std::string toString() const;
};

#endif
