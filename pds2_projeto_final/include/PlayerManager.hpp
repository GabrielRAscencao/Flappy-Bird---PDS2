#ifndef PLAYERMANAGER_HPP
#define PLAYERMANAGER_HPP

#include <vector>
#include <string>
#include "Player.hpp"

class PlayerManager {
private:
    std::vector<Player> jogadores;
    std::string caminhoArquivo;

public:
    PlayerManager(const std::string& caminho);

    void carregar();
    void salvar() const;
    void cadastrar(const std::string& nome, const std::string& apelido);
    Player* buscar(const std::string& apelido);
    void imprimirRanking() const;

    const std::vector<Player>& getJogadores() const;

};

#endif