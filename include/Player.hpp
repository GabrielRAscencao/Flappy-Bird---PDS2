/**
 * @file Player.hpp
 * @brief Playerheader do projeto Traveling Dragon.
 */

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string> // Necessário para usar a classe std::string

/**
 * @brief Representa um jogador individual no jogo.
 *
 * A classe Player armazena e gerencia as informações de um jogador,
 * como nome, apelido, número de partidas jogadas e sua maior pontuação alcançada.
 */
class Player {
private:
    std::string nome;           ///< @brief Nome completo do jogador.
    std::string apelido;        ///< @brief Apelido único do jogador, usado para identificação no ranking.
    int partidas;               ///< @brief Número total de partidas jogadas por este jogador.
    int maiorPontuacao;         ///< @brief A maior pontuação alcançada pelo jogador em uma única partida.

public:
    /**
     * @brief Construtor da classe Player.
     *
     * Inicializa um novo objeto Player com um nome e um apelido.
     * As partidas e a maior pontuação são inicializadas como zero.
     *
     * @param nome O nome completo do jogador.
     * @param apelido O apelido do jogador, que será usado para identificá-lo.
     */
    Player(std::string nome, std::string apelido);

    /**
     * @brief Retorna o apelido do jogador.
     * @return Uma string constante contendo o apelido.
     */
    std::string getApelido() const;

    /**
     * @brief Retorna o nome completo do jogador.
     * @return Uma string constante contendo o nome.
     */
    std::string getNome() const;

    /**
     * @brief Retorna o número de partidas jogadas pelo jogador.
     * @return O número de partidas.
     */
    int getPartidas() const;

    /**
     * @brief Retorna a maior pontuação já alcançada pelo jogador.
     * @return A maior pontuação.
     */
    int getMaiorPontuacao() const;

    /**
     * @brief Atualiza as estatísticas do jogador após uma partida.
     *
     * Incrementa o número de partidas jogadas e atualiza a maior pontuação
     * se a pontuação atual for maior que a anterior.
     *
     * @param pontuacao A pontuação alcançada na partida recém-finalizada.
     */
    void adicionarPartida(int pontuacao);

    /**
     * @brief Gera uma representação em string dos dados do jogador.
     * Útil para depuração ou para exibir informações em console/logs.
     * @return Uma string formatada com as informações do jogador.
     */
    std::string toString() const;
};

#endif