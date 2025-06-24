/**
 * @file test_PlayerManager.cpp
 * @brief test_PlayerManagerimplementação do projeto Traveling Dragon.
 */


#include "doctest.h"              // Inclui o cabeçalho do Doctest para escrever testes.
#include "../include/PlayerManager.hpp" // Inclui a classe PlayerManager.
#include <algorithm>              // Necessário para std::sort na ordenação do ranking.
#include <vector>                 // Necessário para std::vector.

/**
 * @brief Caso de teste para verificar o cadastro de jogadores e a ordenação do ranking
 * utilizando o método `Player::adicionarPartida` para definir as pontuações.
 *
 * @details Este teste:
 * - Cria uma instância de PlayerManager.
 * - Cadastra três jogadores.
 * - Busca esses jogadores para garantir que foram adicionados corretamente.
 * - Utiliza `adicionarPartida()` para atribuir pontuações a cada jogador.
 * - Obtém a lista de jogadores e a ordena com base na maior pontuação.
 * - Verifica se a ordem dos jogadores no ranking está correta.
 */
TEST_CASE("Cadastro de jogadores e ranking com Player::adicionarPartida") {
    // Inicializa o PlayerManager com um arquivo de teste.
    // É importante usar um arquivo de teste separado para não sobrescrever dados reais.
    PlayerManager manager("test_players.dat");

    // Cadastra novos jogadores.
    manager.cadastrar("Gabriel", "gabriel");
    manager.cadastrar("João", "joao");
    manager.cadastrar("Alvaro", "alvaro");

    // Busca os ponteiros para os jogadores recém-cadastrados.
    Player* p1 = manager.buscar("gabriel");
    Player* p2 = manager.buscar("joao");
    Player* p3 = manager.buscar("alvaro");

    // Verifica se os jogadores foram encontrados (não são nulos).
    REQUIRE(p1 != nullptr); // Se falhar aqui, os testes subsequentes podem ser inválidos.
    REQUIRE(p2 != nullptr);
    REQUIRE(p3 != nullptr);

    // Usa adicionarPartida para setar a pontuação de cada jogador.
    // Este método também incrementa o número de partidas.
    p1->adicionarPartida(30); // Gabriel tem 30 pontos.
    p2->adicionarPartida(10); // João tem 10 pontos.
    p3->adicionarPartida(20); // Alvaro tem 20 pontos.

    // Obtém a lista atual de jogadores do manager.
    auto jogadores = manager.getJogadores();
    // Cria uma cópia do vetor de jogadores para poder ordená-lo sem afetar o manager original.
    std::vector<Player> ranking = jogadores;

    // Ordena o vetor 'ranking' em ordem decrescente de 'maiorPontuacao'.
    std::sort(ranking.begin(), ranking.end(),
        [](const Player& a, const Player& b) {
            return a.getMaiorPontuacao() > b.getMaiorPontuacao();
        });

    // Verifica a ordem esperada dos jogadores no ranking: Gabriel (30), Alvaro (20), João (10).
    CHECK(ranking[0].getApelido() == "gabriel");
    CHECK(ranking[1].getApelido() == "alvaro");
    CHECK(ranking[2].getApelido() == "joao");
}