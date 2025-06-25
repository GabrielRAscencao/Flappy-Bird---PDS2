/**
 * @file PlayerManager.cpp
 * @brief Implementação da classe PlayerManager do projeto Traveling Dragon.
 */

#include "PlayerManager.hpp"
#include <fstream>      // Para operações de leitura e escrita de arquivos
#include <iostream>     // Para mensagens no console (erros, avisos)
#include <filesystem>   // Para garantir a criação da pasta de destino

/**
 * @brief Construtor da classe PlayerManager.
 * 
 * Inicializa o gerenciador de jogadores com o caminho fornecido para o arquivo
 * de persistência de dados.
 * 
 * @param caminho O caminho completo do arquivo onde os dados serão salvos/carregados.
 */
PlayerManager::PlayerManager(const std::string& caminho) : caminhoArquivo(caminho) {}

/**
 * @brief Carrega os dados dos jogadores de um arquivo para a memória.
 * 
 * Os dados existentes na memória são limpos antes do carregamento.
 * Cada linha do arquivo deve conter: nome apelido partidas maiorPontuacao
 */
void PlayerManager::carregar() {
    jogadores.clear(); // Limpa os dados existentes na memória

    std::ifstream arq(caminhoArquivo); // Tenta abrir o arquivo para leitura
    if (!arq.is_open()) {
        std::cerr << "Aviso: não foi possível abrir " << caminhoArquivo << " para leitura.\n";
        return; // Se o arquivo não existir, apenas termina (não é erro crítico)
    }

    std::string nome, apelido;
    int partidas, maiorPontuacao;

    while (arq >> nome >> apelido >> partidas >> maiorPontuacao) {
        Player p(nome, apelido);
        for (int i = 0; i < partidas; ++i)
            p.adicionarPartida(maiorPontuacao); // Simula partidas para manter contagem e recorde
        jogadores.push_back(p);
    }

    // O arquivo é fechado automaticamente ao sair do escopo
}

/**
 * @brief Salva os dados dos jogadores da memória para o arquivo.
 * 
 * O arquivo será sobrescrito. Cria a pasta automaticamente se ela não existir.
 */
void PlayerManager::salvar() const {
    // Garante que o diretório onde o arquivo será salvo exista
    std::filesystem::create_directories(std::filesystem::path(caminhoArquivo).parent_path());

    std::ofstream arq(caminhoArquivo); // Abre o arquivo para escrita
    if (!arq.is_open()) {
        std::cerr << "Erro: não foi possível salvar os dados em " << caminhoArquivo << "\n";
        return;
    }

    for (const auto& p : jogadores) {
        arq << p.getNome() << " " << p.getApelido() << " "
            << p.getPartidas() << " " << p.getMaiorPontuacao() << "\n";
    }
}

/**
 * @brief Cadastra um novo jogador na memória.
 * 
 * @param nome Nome real do jogador.
 * @param apelido Apelido único do jogador.
 */
void PlayerManager::cadastrar(const std::string& nome, const std::string& apelido) {
    jogadores.emplace_back(nome, apelido);
}

/**
 * @brief Retorna uma referência constante para a lista de jogadores.
 */
const std::vector<Player>& PlayerManager::getJogadores() const {
    return jogadores;
}

/**
 * @brief Busca um jogador pelo apelido.
 * 
 * @param apelido O apelido do jogador a ser buscado.
 * @return Ponteiro para o jogador encontrado, ou nullptr se não existir.
 */
Player* PlayerManager::buscar(const std::string& apelido) {
    for (auto& p : jogadores) {
        if (p.getApelido() == apelido)
            return &p;
    }
    return nullptr;
}

/**
 * @brief Imprime no console o ranking dos jogadores cadastrados.
 * 
 * A lista não é ordenada aqui — apenas exibida como está.
 */
void PlayerManager::imprimirRanking() const {
    std::cout << "Ranking dos jogadores:\n";
    for (const auto& p : jogadores) {
        std::cout << p.toString() << std::endl;
    }
}
