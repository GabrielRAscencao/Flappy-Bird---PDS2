/**
 * @file ConfigScreen.cpp
 * @brief ConfigScreenimplementação do projeto Traveling Dragon.
 */


#include "ConfigScreen.hpp"
#include <allegro5/allegro_primitives.h> // Necessário para desenho de formas primitivas
#include <iostream> // Para saída de avisos

/**
 * @brief Construtor da classe ConfigScreen.
 * @param f Ponteiro para a fonte a ser usada no texto.
 * @param bg Ponteiro para o bitmap de fundo da tela.
 * @param screenW Largura da tela atual do jogo.
 * @param screenH Altura da tela atual do jogo.
 */
ConfigScreen::ConfigScreen(ALLEGRO_FONT* f, ALLEGRO_BITMAP* bg, int screenW, int screenH)
    : font(f), background(bg), SCREEN_W(screenW), SCREEN_H(screenH), waitingForKeyPress(true)
{
    // Dimensões de design para cálculo da escala
    const float DESIGN_W = 1280.0f;
    const float DESIGN_H = 720.0f;
    // Calcula os fatores de escala para ajustar o conteúdo à resolução atual
    scale_x = SCREEN_W / DESIGN_W;
    scale_y = SCREEN_H / DESIGN_H;

    // Verifica se a fonte foi carregada corretamente
    if (!font) {
        std::cerr << "AVISO CONFIGSCREEN: Fonte nao carregada no construtor.\n";
    }
}

/**
 * @brief Destrutor da classe ConfigScreen.
 * Nota: Não destrói a fonte nem o background, pois a propriedade desses assets é de outra classe (ex: AssetManager).
 */
ConfigScreen::~ConfigScreen() {
    // Não destruir font nem background (proprietários são outros)
}

/**
 * @brief Atualiza o estado da tela de configurações.
 * @param deltaTime Tempo decorrido desde a última atualização (atualmente não usado).
 * Esta tela é estática, então não há lógica de atualização dinâmica aqui.
 */
void ConfigScreen::update(float deltaTime) {
    // Nada a atualizar, tela estática
}

/**
 * @brief Renderiza a tela de configurações.
 * Desenha o fundo e uma mensagem instruindo o usuário a voltar ao menu.
 */
void ConfigScreen::render() {
    // Desenha o fundo da tela (o mesmo que a tela de ranking)
    if (background) {
        int bg_w = al_get_bitmap_width(background);
        int bg_h = al_get_bitmap_height(background);
        // Calcula a escala para cobrir toda a tela, mantendo a proporção
        float bg_scale = std::max(SCREEN_W / (float)bg_w, SCREEN_H / (float)bg_h);
        al_draw_scaled_bitmap(background, 0, 0, bg_w, bg_h, 0, 0, bg_w * bg_scale, bg_h * bg_scale, 0);
    } else {
        // Fallback: se o background for nulo, pinta a tela de preto
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    // Retorna se a fonte não estiver carregada para evitar erros de desenho
    if (!font) return;

    // Desenha o texto centralizado para instruir o usuário a voltar ao menu
    al_draw_text(font, al_map_rgb(255, 255, 255),
        SCREEN_W / 2.0f, SCREEN_H / 2.0f,
        ALLEGRO_ALIGN_CENTER,
        "Aperte qualquer tecla para voltar");
}

/**
 * @brief Lida com eventos de entrada na tela de configurações.
 * @param ev O evento Allegro a ser processado.
 * @return 1 se uma tecla for pressionada (sinal para voltar ao menu), 0 caso contrário.
 */
int ConfigScreen::handleEvent(ALLEGRO_EVENT ev) {
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        waitingForKeyPress = false; // Sinaliza que uma tecla foi pressionada
        return 1; // Retorna 1 para indicar que a tela deve ser fechada
    }
    return 0; // Nenhum evento relevante processado
}

/**
 * @brief Reseta o estado da tela de configurações.
 * Útil para reexibir a tela.
 */
void ConfigScreen::resetState() {
    waitingForKeyPress = true; // Reinicia o estado de espera por uma tecla
}