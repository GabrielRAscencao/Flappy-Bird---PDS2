/**
 * @file GameOverScreen.hpp
 * @brief GameOverScreenheader do projeto Traveling Dragon.
 */

#ifndef GAMEOVERSCREEN_HPP
#define GAMEOVERSCREEN_HPP

#include <allegro5/allegro.h>          // Para usar tipos e funções básicas do Allegro
#include <allegro5/allegro_font.h>     // Para renderizar texto com fontes
#include <allegro5/allegro_primitives.h> // Para desenhar formas como retângulos (para botões)
#include <allegro5/allegro_audio.h>    // Para tocar efeitos sonoros
#include <string>                       // Para usar std::string para textos
#include <array>                        // Para usar std::array para definir retângulos de botões

/**
 * @brief Gerencia e exibe a tela de "Game Over" do jogo.
 *
 * Esta classe é responsável por mostrar a pontuação do jogador,
 * os recordes (pessoal e geral) e oferecer opções para reiniciar
 * o jogo ou voltar ao menu principal.
 */
class GameOverScreen {
public:
    /**
     * @brief Construtor da classe GameOverScreen.
     *
     * Inicializa a tela de Game Over com as fontes e o bitmap de fundo.
     *
     * @param f Ponteiro para a fonte Allegro a ser usada.
     * @param gameOverBackground Ponteiro para o bitmap de fundo específico da tela de Game Over.
     */
    GameOverScreen(ALLEGRO_FONT* f, ALLEGRO_BITMAP* gameOverBackground);

    /**
     * @brief Destrutor da classe GameOverScreen.
     * Libera quaisquer recursos alocados pela tela de Game Over.
     */
    ~GameOverScreen();

    /**
     * @brief Renderiza todos os elementos visuais da tela de Game Over.
     *
     * Exibe a pontuação final, o recorde pessoal e o recorde geral,
     * indicando se o jogador bateu algum recorde. Também renderiza os botões de ação.
     *
     * @param score A pontuação final da partida atual.
     * @param recordPessoal O maior recorde pessoal do jogador.
     * @param recordGeral O maior recorde geral entre todos os jogadores.
     * @param r1 true se o recorde pessoal foi batido na partida, false caso contrário.
     * @param r2 true se o recorde geral foi batido na partida, false caso contrário.
     */
    void render(int score, int recordPessoal, int recordGeral, bool r1, bool r2);

    /**
     * @brief Processa um evento do Allegro específico para a tela de Game Over.
     *
     * Lida com cliques do mouse nos botões "Reiniciar" e "Menu".
     *
     * @param ev Ponteiro para o evento Allegro a ser processado.
     * @return Um código inteiro indicando a ação desejada: 0 (nenhuma), 1 (Reiniciar), 2 (Menu).
     */
    int handleEvent(ALLEGRO_EVENT* ev);

    /**
     * @brief Define o som que será reproduzido quando o mouse passar sobre os botões.
     * @param som Ponteiro para o ALLEGRO_SAMPLE do som de hover.
     */
    void setHoverSound(ALLEGRO_SAMPLE* som);

private:
    ALLEGRO_FONT* font;             ///< @brief Fonte padrão para textos na tela.
    ALLEGRO_FONT* fontLarge;        ///< @brief Fonte maior, usada para destaque (ex: pontuações).
    ALLEGRO_BITMAP* background;     ///< @brief Imagem de fundo genérica (pode não ser usada se gameOverBackground for o principal).
    ALLEGRO_BITMAP* gameOverBackground; ///< @brief Imagem de fundo principal da tela de Game Over.
    float scroll;                   ///< @brief Variável para controle de rolagem de fundo, se houver animação.

    float SCREEN_W, SCREEN_H;       ///< @brief Largura e altura da tela de exibição.
    float scale_x, scale_y;         ///< @brief Fatores de escala para ajustar elementos à resolução.

    std::array<float, 4> replayBtn; ///< @brief Define a área e posição do botão "Reiniciar" (x, y, largura, altura).
    std::array<float, 4> menuBtn;   ///< @brief Define a área e posição do botão "Menu".

    /**
     * @brief Desenha um botão na tela com um texto e um estilo que pode mudar ao passar o mouse.
     * @param text O texto a ser exibido no botão.
     * @param rect Um array de 4 floats (x, y, largura, altura) definindo a área do botão.
     * @param hover true se o mouse estiver sobre o botão, false caso contrário.
     */
    void drawButton(const std::string& text, const std::array<float, 4>& rect, bool hover);

    /**
     * @brief Verifica se as coordenadas do mouse estão sobre uma área retangular definida.
     * @param rect Um array de 4 floats (x, y, largura, altura) da área a ser verificada.
     * @param mx A coordenada X do mouse.
     * @param my A coordenada Y do mouse.
     * @return true se o mouse estiver sobre a área, false caso contrário.
     */
    bool isMouseOver(const std::array<float, 4>& rect, float mx, float my);

    int selected;                   ///< @brief Armazena o código do botão que foi clicado ou está ativo.

    ALLEGRO_SAMPLE* somHover;       ///< @brief O sample de áudio para o efeito sonoro de hover.
    bool lastHoverReplay;           ///< @brief Estado de hover anterior do botão "Reiniciar", para controlar o som.
    bool lastHoverMenu;             ///< @brief Estado de hover anterior do botão "Menu", para controlar o som.

    // @note `previousHoverStates` parece ser redundante com `lastHoverReplay` e `lastHoverMenu`.
    // Considerar remover ou unificar.
    std::array<bool, 2> previousHoverStates;
};

#endif // GAMEOVERSCREEN_HPP