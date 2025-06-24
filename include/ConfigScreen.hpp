/**
 * @file ConfigScreen.hpp
 * @brief ConfigScreenheader do projeto Traveling Dragon.
 */

#ifndef CONFIGSCREEN_HPP
#define CONFIGSCREEN_HPP

#include <allegro5/allegro.h>     // Para funcionalidades básicas do Allegro
#include <allegro5/allegro_font.h> // Para desenhar texto com fontes

/**
 * @brief Gerencia a tela de configurações do jogo.
 *
 * Esta classe é responsável por exibir as opções de configuração do jogo
 * e processar as interações do usuário com essas opções.
 */
class ConfigScreen {
public:
    /**
     * @brief Construtor da classe ConfigScreen.
     *
     * Inicializa a tela de configurações com os recursos visuais necessários
     * e as dimensões da tela.
     *
     * @param font Ponteiro para a fonte Allegro a ser usada para o texto.
     * @param background Ponteiro para o bitmap de fundo da tela de configurações.
     * @param screenWidth Largura da tela do jogo.
     * @param screenHeight Altura da tela do jogo.
     */
    ConfigScreen(ALLEGRO_FONT* font, ALLEGRO_BITMAP* background, int screenWidth, int screenHeight);

    /**
     * @brief Destrutor da classe ConfigScreen.
     * Responsável por liberar recursos alocados, se houver.
     */
    ~ConfigScreen();

    /**
     * @brief Atualiza a lógica interna da tela de configurações.
     * @param deltaTime O tempo decorrido desde a última atualização (em segundos).
     */
    void update(float deltaTime);

    /**
     * @brief Renderiza (desenha) todos os elementos visuais da tela de configurações.
     */
    void render();

    /**
     * @brief Processa um evento do Allegro, como entrada de teclado ou mouse,
     * e reage de acordo com a interação na tela de configurações.
     * @param ev O evento Allegro a ser processado.
     * @return Um inteiro que representa a ação a ser tomada após o evento (ex: 0 para continuar, 1 para voltar ao menu).
     */
    int handleEvent(ALLEGRO_EVENT ev);

    /**
     * @brief Reseta o estado interno da tela de configurações para o seu padrão.
     * Útil ao entrar na tela para garantir um estado limpo.
     */
    void resetState();

private:
    ALLEGRO_FONT* font;         ///< @brief Fonte utilizada para desenhar o texto na tela de configurações.
    ALLEGRO_BITMAP* background; ///< @brief Imagem de fundo da tela de configurações.
    int SCREEN_W;               ///< @brief Largura da tela de exibição.
    int SCREEN_H;               ///< @brief Altura da tela de exibição.
    bool waitingForKeyPress;    ///< @brief Flag que indica se a tela está aguardando um pressionamento de tecla.
    float scale_x;              ///< @brief Fator de escala horizontal para ajustar elementos à resolução.
    float scale_y;              ///< @brief Fator de escala vertical para ajustar elementos à resolução.
};

#endif