/**
 * @file Menu.hpp
 * @brief Menuheader do projeto Traveling Dragon.
 */

#ifndef MENU_HPP
#define MENU_HPP

#include <allegro5/allegro.h>          // Para funcionalidades básicas do Allegro
#include <allegro5/allegro_font.h>     // Para renderizar texto com fontes
#include <allegro5/allegro_primitives.h> // Para desenhar formas geométricas (caixa de input, botões)
#include <allegro5/allegro_audio.h>    // Para tocar efeitos sonoros
#include <string>                       // Para usar std::string (apelido, mensagens)
#include <array>                        // Para usar std::array (seções de botões)
#include <vector>                       // Para usar std::vector (posições dos botões)

/**
 * @brief Gerencia e exibe a tela de menu principal do jogo.
 *
 * Esta classe é responsável por mostrar as opções do menu (Jogar, Ranking, Configurações, Sair),
 * permitir a entrada de apelido para o jogador e lidar com a navegação e seleção de opções.
 */
class Menu {
public:
    /**
     * @brief Construtor da classe Menu.
     *
     * Inicializa o menu com as fontes a serem usadas e o bitmap de fundo.
     *
     * @param font Ponteiro para a fonte Allegro padrão.
     * @param fontlarge Ponteiro para uma fonte Allegro maior para títulos/destaques.
     * @param bg Ponteiro para o bitmap de fundo do menu.
     */
    Menu(ALLEGRO_FONT* font, ALLEGRO_FONT* fontlarge, ALLEGRO_BITMAP* bg);

    /**
     * @brief Atualiza a lógica interna do menu a cada frame.
     *
     * Isso inclui a animação do cursor no campo de input de apelido e
     * o timer para exibição de mensagens de aviso.
     *
     * @param deltaTime O tempo decorrido desde a última atualização (em segundos).
     */
    void update(double deltaTime);

    /**
     * @brief Renderiza (desenha) todos os elementos visuais do menu na tela.
     * Inclui o fundo, o campo de input de apelido, os botões e mensagens de aviso.
     */
    void render();

    /**
     * @brief Processa um evento de clique do mouse nas áreas dos botões do menu.
     * @param mx A coordenada X do clique do mouse.
     * @param my A coordenada Y do clique do mouse.
     */
    void onClick(int mx, int my);

    /**
     * @brief Processa um evento de caractere digitado (input de texto) e um keycode.
     * Usado principalmente para preencher o campo de apelido.
     * @param unichar O caractere Unicode digitado.
     * @param keycode O código da tecla pressionada.
     */
    void onChar(int unichar, int keycode);

    /**
     * @brief Retorna o apelido atualmente digitado ou selecionado no menu.
     * @return Uma string contendo o apelido do jogador.
     */
    std::string getApelido() const;

    /**
     * @brief Retorna a ação que foi selecionada pelo usuário no menu.
     * @return Um código inteiro representando a ação (ex: 1 para Jogar, 2 para Ranking, etc.).
     */
    int getSelectedAction() const;

    /**
     * @brief Reseta o estado da ação selecionada para um valor padrão.
     * Útil para garantir que uma ação não seja acionada múltiplas vezes.
     */
    void resetAction();

    /**
     * @brief Exibe uma mensagem de aviso temporária na tela do menu.
     * @param message A string da mensagem a ser exibida.
     */
    void displayWarning(const std::string& message);

    /**
     * @brief Ativa ou desativa a capacidade de o usuário digitar no campo de apelido.
     * @param active true para ativar a digitação, false para desativar.
     */
    void setInputActive(bool active);

    /**
     * @brief Verifica se a entrada de texto para o apelido está ativa.
     * @return true se a entrada estiver ativa, false caso contrário.
     */
    bool isInputActive() const;

    /**
     * @brief Define o som que será reproduzido quando o mouse passar sobre os botões do menu.
     * @param som Ponteiro para o ALLEGRO_SAMPLE do som de hover.
     */
    void setHoverSound(ALLEGRO_SAMPLE* som);

private:
    ALLEGRO_FONT* font;             ///< @brief Fonte padrão usada para a maioria dos textos do menu.
    ALLEGRO_FONT* fontlarge;        ///< @brief Fonte maior, usada para títulos ou destaque visual.
    ALLEGRO_BITMAP* background;     ///< @brief Imagem de fundo do menu.

    std::string apelido;            ///< @brief A string que armazena o apelido digitado pelo jogador.
    int selectedAction;             ///< @brief Flag para indicar qual opção do menu foi selecionada (0: nenhuma, 1: Jogar, 2: Ranking, 3: Sair, 4: Configurações).
    bool inputActive;               ///< @brief Controla se o campo de input de apelido está ativo para digitação.
    bool cursorVisible;             ///< @brief Controla a visibilidade do cursor piscando no campo de input.
    double cursorTimer;             ///< @brief Timer para controlar o piscar do cursor.
    float scroll;                   ///< @brief Variável para controlar a rolagem do fundo (se animado).

    std::string currentWarningMessage; ///< @brief A mensagem de aviso que está sendo exibida no momento.
    double warningDisplayTimer;        ///< @brief Timer para controlar por quanto tempo a mensagem de aviso permanece visível.

    float SCREEN_W, SCREEN_H;       ///< @brief Largura e altura da tela de exibição.
    float scale_x, scale_y;         ///< @brief Fatores de escala para ajustar elementos à resolução atual.

    float buttonWidth;              ///< @brief Largura padrão dos botões do menu.
    float buttonHeight;             ///< @brief Altura padrão dos botões do menu.
    ALLEGRO_FONT* buttonFont;       ///< @brief Fonte utilizada especificamente para o texto dos botões.
    std::vector<std::array<float, 2>> buttonPositions; ///< @brief Um vetor de pares (x, y) definindo as posições centrais de cada botão.

    /// @brief Duração padrão para exibir mensagens de aviso na tela.
    static const double WARNING_DURATION;

    /**
     * @brief Atualiza a lógica de animação do cursor no campo de input.
     * @param deltaTime O tempo decorrido desde a última atualização.
     */
    void updateCursor(double deltaTime);

    /**
     * @brief Desenha o texto de um botão na tela.
     * O estilo do texto pode mudar se o mouse estiver sobre o botão.
     * @param x Coordenada X central do texto.
     * @param y Coordenada Y central do texto.
     * @param text A string de texto a ser desenhada.
     * @param hover true se o mouse estiver sobre o botão, false caso contrário.
     */
    void drawButtonText(float x, float y, const std::string& text, bool hover);

    /**
     * @brief Retorna o rótulo (texto) de um botão com base no seu índice.
     * @param index O índice do botão (0 para "Jogar", 1 para "Ranking", etc.).
     * @return A string correspondente ao rótulo do botão.
     */
    std::string getButtonLabel(int index) const;

    /**
     * @brief Remove espaços em branco desnecessários do início e fim do apelido.
     */
    void trimApelido();

    ALLEGRO_SAMPLE* somHover;       ///< @brief O sample de áudio para o efeito sonoro de hover.
    bool lastHoverState[4];         ///< @brief Array para controlar o estado de hover de cada botão e evitar tocar o som repetidamente.

    // @note `previousHoverStates` parece ser redundante com `lastHoverState`.
    // Considerar remover ou unificar.
    std::vector<bool> previousHoverStates;
};

#endif // MENU_HPP