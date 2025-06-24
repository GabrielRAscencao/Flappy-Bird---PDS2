/**
 * @file PlayerManager.hpp
 * @brief PlayerManagerheader do projeto Traveling Dragon.
 */

#ifndef PLAYERMANAGER_HPP
#define PLAYERMANAGER_HPP

#include <vector>   // Para usar std::vector para armazenar jogadores
#include <string>   // Para usar std::string para nomes de arquivos e dados
#include "Player.hpp" // Para ter a definição da classe Player

/**
 * @brief Gerencia o armazenamento e a manipulação dos dados de todos os jogadores.
 *
 * A classe PlayerManager é responsável por carregar e salvar os dados dos jogadores
 * em um arquivo, além de permitir o cadastro, busca e acesso ao ranking de jogadores.
 */
class PlayerManager {
private:
    std::vector<Player> jogadores; ///< @brief Vetor que armazena todos os objetos Player carregados ou cadastrados.
    std::string caminhoArquivo;    ///< @brief O caminho completo do arquivo onde os dados dos jogadores são persistidos.

public:
    /**
     * @brief Construtor da classe PlayerManager.
     *
     * Inicializa o gerenciador de jogadores com o caminho do arquivo de persistência.
     *
     * @param caminho O caminho do arquivo (ex: "players.dat") onde os dados serão salvos/carregados.
     */
    PlayerManager(const std::string& caminho);

    /**
     * @brief Carrega os dados dos jogadores de um arquivo para a memória.
     *
     * Se o arquivo não existir ou estiver corrompido, o vetor de jogadores pode
     * ser inicializado vazio ou com dados padrão.
     */
    void carregar();

    /**
     * @brief Salva os dados atuais dos jogadores que estão na memória para o arquivo.
     * Marca como `const` porque não altera o estado do PlayerManager, apenas o arquivo.
     */
    void salvar() const;

    /**
     * @brief Cadastra um novo jogador no sistema.
     *
     * Se um jogador com o mesmo apelido já existir, ele pode ser atualizado ou
     * um aviso pode ser gerado, dependendo da implementação.
     *
     * @param nome O nome completo do jogador a ser cadastrado.
     * @param apelido O apelido único do jogador.
     */
    void cadastrar(const std::string& nome, const std::string& apelido);

    /**
     * @brief Busca um jogador pelo seu apelido.
     * @param apelido O apelido do jogador a ser buscado.
     * @return Um ponteiro para o objeto Player encontrado, ou `nullptr` se nenhum jogador
     * com o apelido especificado for encontrado.
     */
    Player* buscar(const std::string& apelido);

    /**
     * @brief Imprime o ranking de todos os jogadores cadastrados no console.
     * A ordem é geralmente decrescente pela maior pontuação.
     * Marca como `const` pois não altera os dados dos jogadores.
     */
    void imprimirRanking() const;

    /**
     * @brief Retorna uma referência constante para o vetor de todos os jogadores.
     * Permite acessar a lista de jogadores sem permitir sua modificação externa direta.
     * @return Uma referência constante para o `std::vector<Player>` de jogadores.
     */
    const std::vector<Player>& getJogadores() const;

};

#endif