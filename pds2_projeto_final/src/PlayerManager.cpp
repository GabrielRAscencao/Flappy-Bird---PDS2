#include "PlayerManager.hpp"
#include <fstream>
#include <iostream>

PlayerManager::PlayerManager(const std::string& caminho) : caminhoArquivo(caminho) {}

void PlayerManager::carregar() {
    jogadores.clear();
    std::ifstream arq(caminhoArquivo);
    std::string nome, apelido;
    int partidas, maiorPontuacao;

    while (arq >> nome >> apelido >> partidas >> maiorPontuacao) {
        Player p(nome, apelido);
        for (int i = 0; i < partidas; ++i)
            p.adicionarPartida(maiorPontuacao); // aplica maior pontuação em todas
        jogadores.push_back(p);
    }
}

void PlayerManager::salvar() const {
    std::ofstream arq(caminhoArquivo);
    for (const auto& p : jogadores) {
        arq << p.getNome() << " " << p.getApelido() << " " << p.getPartidas() << " " << p.getMaiorPontuacao() << "\n";
    }
}

void PlayerManager::cadastrar(const std::string& nome, const std::string& apelido) {
    jogadores.emplace_back(nome, apelido);
}

const std::vector<Player>& PlayerManager::getJogadores() const {
    return jogadores;
}


Player* PlayerManager::buscar(const std::string& apelido) {
    for (auto& p : jogadores)
        if (p.getApelido() == apelido)
            return &p;
    return nullptr;
}

void PlayerManager::imprimirRanking() const {
    std::cout << "Ranking dos jogadores:\n";
    for (const auto& p : jogadores)
        std::cout << p.toString() << std::endl;
}

