/**
 * @file RankingScreen.hpp
 * @brief RankingScreenheader do projeto Traveling Dragon.
 */

#ifndef RANKINGSCREEN_HPP
#define RANKINGSCREEN_HPP

#include <allegro5/allegro_font.h> // Para renderizar texto com fontes
#include <allegro5/allegro.h>      // Para funcionalidades básicas do Allegro
#include <vector>                  // Para usar std::vector para a lista de jogadores
#include <string>                  // Para usar std::string para nomes e textos
#include <algorithm>               // Para usar std::sort na ordenação do ranking
#include "PlayerManager.hpp"       // Para acessar os dados dos jogadores
#include "Player.hpp"              // Para ter a definição da classe Player

/**
 * @brief Gerencia e exibe a tela de ranking de pontuações do jogo.
 *
 * Esta classe é responsável por buscar os dados dos jogadores, ordená-los
 * por pontuação e exibi-los em um formato de ranking na tela, permitindo
 * também a rolagem visual do fundo.
 */
class RankingScreen {
public:
    /**
     * @brief Construtor da classe RankingScreen.
     *
     * Inicializa a tela de ranking com os recursos visuais e os dados do jogador.
     *
     * @param font Ponteiro para a fonte Allegro a ser usada para o texto do ranking.
     * @param rankingBackground Ponteiro para o bitmap de fundo específico da tela de ranking.
     * @param pm Ponteiro para o PlayerManager, para acessar os dados dos jogadores.
     * @param screenW Largura da tela de exibição.
     * @param screenH Altura da tela de exibição.
     */
    RankingScreen(ALLEGRO_FONT* font, ALLEGRO_BITMAP* rankingBackground, PlayerManager* pm, int screenW, int screenH);

    /**
     * @brief Renderiza todos os elementos visuais da tela de ranking.
     *
     * Exibe a lista de jogadores com suas pontuações, destacando o jogador atual
     * se ele estiver presente no ranking. Também renderiza o fundo com rolagem.
     *
     * @param currentPlayer Ponteiro para o jogador atual, que pode ser destacado no ranking. Pode ser nullptr.
     * @param deltaTime O tempo decorrido desde a última atualização (usado para rolagem do fundo).
     */
    void render(Player* currentPlayer, float deltaTime);

    /**
     * @brief Atualiza a lógica interna da tela de ranking.
     * Principalmente usada para animar a rolagem do fundo.
     * @param deltaTime O tempo decorrido desde a última atualização.
     */
    void update(float deltaTime);

    /**
     * @brief Processa um evento do Allegro específico para a tela de ranking.
     *
     * Lida com eventos de teclado, como o pressionamento de uma tecla para
     * retornar ao menu principal.
     *
     * @param ev O evento Allegro a ser processado.
     * @return Um código inteiro indicando a ação desejada: 0 (nenhuma), 1 (voltar ao menu).
     */
    int handleEvent(ALLEGRO_EVENT ev);

    /**
     * @brief Reseta o estado interno da tela de ranking.
     * Usado para garantir que a tela não saia imediatamente ao ser carregada
     * se uma tecla de saída já estiver pressionada.
     */
    void resetState();

private:
    ALLEGRO_FONT* font;         ///< @brief Fonte utilizada para desenhar o texto do ranking.
    ALLEGRO_BITMAP* background; ///< @brief (Pode ser uma imagem geral de fundo, se não for a rankingBackground).
    ALLEGRO_BITMAP* rankingBackground; ///< @brief Imagem de fundo principal da tela de ranking.
    PlayerManager* playerManager; ///< @brief Ponteiro para o gerenciador de jogadores, para acesso aos dados. Não é o proprietário.
    float SCREEN_W, SCREEN_H;   ///< @brief Largura e altura da tela de exibição.
    float scale_x, scale_y;     ///< @brief Fatores de escala para ajustar elementos à resolução.

    float scrollOffset;         ///< @brief Posição de rolagem do fundo para criar um efeito de movimento contínuo.
    bool waitingForKeyPress;    ///< @brief Flag que indica se a tela está aguardando um pressionamento de tecla para sair.
};

#endif // RANKINGSCREEN_HPP