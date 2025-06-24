/**
 * @file PlayerManager.cpp
 * @brief PlayerManagerimplementação do projeto Traveling Dragon.
 */


#include "PlayerManager.hpp"
#include <fstream> // Para operações de arquivo (leitura/escrita)
#include <iostream> // Para saída de erros/avisos

/**
 * @brief Construtor da classe PlayerManager.
 * @param caminho O caminho do arquivo onde os dados dos jogadores serão armazenados.
 */
PlayerManager::PlayerManager(const std::string& caminho) : caminhoArquivo(caminho) {}

/**
 * @brief Carrega os dados dos jogadores de um arquivo para a memória.
 * Os dados existentes em memória são limpos antes do carregamento.
 *
 * @note Este método assume um formato de arquivo específico: "nome apelido partidas maiorPontuacao\n".
 * Ele também simplifica a lógica de 'partidas' aplicando a 'maiorPontuacao' a todas as partidas,
 * o que pode não ser ideal para um histórico detalhado.
 */
void PlayerManager::carregar() {
    jogadores.clear(); // Limpa a lista atual de jogadores em memória
    std::ifstream arq(caminhoArquivo); // Abre o arquivo para leitura
    // Variáveis temporárias para ler os dados de cada jogador
    std::string nome, apelido;
    int partidas, maiorPontuacao;

    // Loop de leitura: lê os dados de cada linha do arquivo
    while (arq >> nome >> apelido >> partidas >> maiorPontuacao) {
        Player p(nome, apelido); // Cria um novo objeto Player
        // Nota: Esta parte do código está aplicando a `maiorPontuacao` a todas as `partidas` registradas,
        // o que significa que o histórico de pontuações individuais não é mantido, apenas o total de partidas
        // e a maior pontuação geral. Se um histórico mais detalhado for necessário, a lógica de salvamento/carregamento
        // e a estrutura do Player precisarão ser revisadas.
        for (int i = 0; i < partidas; ++i)
            p.adicionarPartida(maiorPontuacao); // Adiciona as partidas, atualizando a maior pontuação
        jogadores.push_back(p); // Adiciona o jogador à lista em memória
    }
    // O arquivo é fechado automaticamente quando `arq` sai do escopo
}

/**
 * @brief Salva os dados dos jogadores da memória para o arquivo.
 * O arquivo é sobrescrito a cada salvamento.
 */
void PlayerManager::salvar() const {
    std::ofstream arq(caminhoArquivo); // Abre o arquivo para escrita (sobrescreve se existir)
    // Itera sobre todos os jogadores na lista e escreve seus dados no arquivo
    for (const auto& p : jogadores) {
        arq << p.getNome() << " " << p.getApelido() << " " << p.getPartidas() << " " << p.getMaiorPontuacao() << "\n";
    }
    // O arquivo é fechado automaticamente quando `arq` sai do escopo
}

/**
 * @brief Cadastra um novo jogador na lista.
 * @param nome O nome real do novo jogador.
 * @param apelido O apelido do novo jogador.
 */
void PlayerManager::cadastrar(const std::string& nome, const std::string& apelido) {
    // Usa emplace_back para construir o objeto Player diretamente no vetor, evitando cópias desnecessárias
    jogadores.emplace_back(nome, apelido);
}

/**
 * @brief Retorna uma referência constante para o vetor de jogadores.
 * Isso permite acessar a lista de jogadores sem modificá-la diretamente.
 * @return Uma referência constante para `std::vector<Player>`.
 */
const std::vector<Player>& PlayerManager::getJogadores() const {
    return jogadores;
}

/**
 * @brief Busca um jogador na lista pelo seu apelido.
 * @param apelido O apelido do jogador a ser buscado.
 * @return Um ponteiro para o objeto Player se encontrado, ou `nullptr` caso contrário.
 */
Player* PlayerManager::buscar(const std::string& apelido) {
    for (auto& p : jogadores) { // Itera sobre os jogadores (por referência para poder retornar ponteiro)
        if (p.getApelido() == apelido) {
            return &p; // Retorna um ponteiro para o jogador encontrado
        }
    }
    return nullptr; // Retorna nullptr se o jogador não for encontrado
}

/**
 * @brief Imprime o ranking de jogadores no console.
 * Itera sobre a lista de jogadores e exibe os dados de cada um.
 *
 * @note Esta função apenas imprime os dados como eles estão no vetor interno,
 * sem ordenação prévia. Para um ranking verdadeiro, a lista de jogadores
 * deve ser ordenada antes de chamar esta função (como feito em `RankingScreen`).
 */
void PlayerManager::imprimirRanking() const {
    std::cout << "Ranking dos jogadores:\n";
    for (const auto& p : jogadores) {
        std::cout << p.toString() << std::endl;
    }
}