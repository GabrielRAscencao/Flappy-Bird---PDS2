/**
 * @file GameOverScreen.cpp
 * @brief GameOverScreenimplementação do projeto Traveling Dragon.
 */


#include "GameOverScreen.hpp"
#include <allegro5/allegro_primitives.h> // Para desenho de formas primitivas
#include <allegro5/allegro_font.h> // Para manipulação de fontes
#include <allegro5/allegro_ttf.h> // Para carregamento de fontes TTF
#include <algorithm> // Para std::max
#include <array> // Para std::array
#include <cstdio> // Para sprintf
#include <iostream> // Para saída de erros/avisos

/**
 * @brief Construtor da classe GameOverScreen.
 * @param f Ponteiro para a fonte padrão a ser utilizada.
 * @param gameoverBackground Ponteiro para o bitmap de fundo da tela de game over.
 */
GameOverScreen::GameOverScreen(ALLEGRO_FONT* f, ALLEGRO_BITMAP* gameoverBackground)
    : font(f), gameOverBackground(gameoverBackground), scroll(0), selected(0), somHover(nullptr)
{
    // Obtém as dimensões atuais da tela para escalabilidade
    SCREEN_W = static_cast<float>(al_get_display_width(al_get_current_display()));
    SCREEN_H = static_cast<float>(al_get_display_height(al_get_current_display()));

    // Define as dimensões de design para cálculo de escala
    const float DESIGN_W = 1280.0f;
    const float DESIGN_H = 720.0f;
    // Calcula os fatores de escala para adaptar o layout à resolução atual
    scale_x = SCREEN_W / DESIGN_W;
    scale_y = SCREEN_H / DESIGN_H;

    // Tenta carregar uma fonte maior para os textos principais
    fontLarge = al_load_ttf_font("assets/editundo.ttf", static_cast<int>(42 * scale_y), 0);
    if (!fontLarge) {
        std::cerr << "AVISO: Nao foi possivel carregar assets/editundo.ttf para fontLarge. Usando fonte padrao.\n";
        fontLarge = font; // Fallback para a fonte padrão se a customizada não carregar
    }

    // Inicializa as áreas dos botões como vazias (serão calculadas em render)
    replayBtn = {0,0,0,0};
    menuBtn = {0,0,0,0};

    // Inicializa os estados anteriores de hover dos botões como falsos
    previousHoverStates = {false, false};
}

/**
 * @brief Destrutor da classe GameOverScreen.
 * Libera a fonte grande se ela foi carregada separadamente da fonte padrão.
 */
GameOverScreen::~GameOverScreen() {
    if (fontLarge && fontLarge != font) { // Garante que só destrói se foi carregada aqui e não é a mesma que `font`
        al_destroy_font(fontLarge);
        fontLarge = nullptr;
    }
}

/**
 * @brief Desenha um botão com texto centralizado e efeito de hover.
 * @param text O texto a ser exibido no botão.
 * @param rect Um array de 4 floats representando (x1, y1, x2, y2) da área clicável do botão.
 * @param hover True se o mouse estiver sobre o botão, false caso contrário.
 */
void GameOverScreen::drawButton(const std::string& text, const std::array<float, 4>& rect, bool hover) {
    float x1 = rect.at(0), y1 = rect.at(1), x2 = rect.at(2), y2 = rect.at(3);
    float cx = (x1 + x2) / 2.0f; // Centro X do botão
    float cy = (y1 + y2) / 2.0f; // Centro Y do botão

    ALLEGRO_COLOR outline = al_map_rgb(0, 0, 0); // Cor do contorno (sombra)
    // Cor do texto: branco com opacidade reduzida (60%) no hover
    ALLEGRO_COLOR textColor = al_map_rgba(255, 255, 255, hover ? 153 : 255); 

    const float PADDING_X = 20.0f * scale_x; // Preenchimento horizontal
    const float PADDING_Y = 10.0f * scale_y; // Preenchimento vertical

    float textWidth = al_get_text_width(fontLarge, text.c_str());
    float textHeight = al_get_font_line_height(fontLarge);

    float rectWidth = textWidth + 2 * PADDING_X;
    float rectHeight = textHeight + 2 * PADDING_Y;

    // Calcula as coordenadas do retângulo de fundo do botão
    float rectLeft = cx - rectWidth / 2.0f;
    float rectTop = cy - rectHeight / 2.0f + 18.0f; // Ajuste para posicionar o retângulo corretamente
    float rectRight = cx + rectWidth / 2.0f;
    float rectBottom = cy + rectHeight / 2.0f + 20.0f; // Ajuste para posicionar o retângulo corretamente

    if (hover) {
        // Desenha um retângulo arredondado translúcido como fundo quando o botão está em hover
        ALLEGRO_COLOR bgColor = al_map_rgba_f(0, 0, 0, 0.4); // Preto com 40% de opacidade
        al_draw_filled_rounded_rectangle(rectLeft, rectTop, rectRight, rectBottom, 10 * scale_x, 10 * scale_y, bgColor);
    }

    // Desenha o contorno preto do texto (efeito de sombra ou contorno)
    for (int dx = -3; dx <= 3; ++dx) {
        for (int dy = -3; dy <= 3; ++dy) {
            if (dx == 0 && dy == 0) continue; // Pula o centro
            al_draw_text(fontLarge, outline, cx + dx, cy + dy, ALLEGRO_ALIGN_CENTER, text.c_str());
        }
    }

    // Desenha o texto principal do botão
    al_draw_text(fontLarge, textColor, cx, cy, ALLEGRO_ALIGN_CENTER, text.c_str());
}

/**
 * @brief Verifica se as coordenadas do mouse estão sobre uma área retangular.
 * @param rect Um array de 4 floats representando (x1, y1, x2, y2) da área.
 * @param mx Coordenada X do mouse.
 * @param my Coordenada Y do mouse.
 * @return True se o mouse estiver sobre o retângulo, false caso contrário.
 */
bool GameOverScreen::isMouseOver(const std::array<float, 4>& rect, float mx, float my) {
    return mx >= rect.at(0) && mx <= rect.at(2) && my >= rect.at(1) && my <= rect.at(3);
}

/**
 * @brief Renderiza a tela de Game Over com as informações de pontuação e botões.
 * @param score A pontuação obtida na partida.
 * @param recordPessoal O recorde pessoal do jogador.
 * @param recordGeral O recorde geral do jogo.
 * @param r1 True se um novo recorde pessoal foi atingido.
 * @param r2 True se um novo recorde geral foi atingido.
 */
void GameOverScreen::render(int score, int recordPessoal, int recordGeral, bool r1, bool r2) {
    // Desenha o fundo da tela de game over ou um fundo sólido se não houver bitmap
    if (gameOverBackground) {
        int bg_w = al_get_bitmap_width(gameOverBackground);
        int bg_h = al_get_bitmap_height(gameOverBackground);
        // Calcula a escala para cobrir toda a tela, mantendo a proporção
        float bg_scale = std::max(SCREEN_W / (float)bg_w, SCREEN_H / (float)bg_h);
        al_draw_scaled_bitmap(gameOverBackground, 0, 0, bg_w, bg_h, 0, 0, bg_w * bg_scale, bg_h * bg_scale, 0);
    } else {
        // Fallback: fundo cinza escuro
        al_clear_to_color(al_map_rgb(50, 50, 50));
    }

    float cx = SCREEN_W / 2.0f; // Centro X da tela
    float box_h = SCREEN_H * 0.45f; // Altura base da "caixa" de informações (não usada diretamente para desenho de caixa, mas para espaçamento)
    // Ajusta a altura da "caixa" se novos recordes foram batidos para acomodar as mensagens
    if (r1 && r2) {
        box_h += 100 * scale_y;
    } else if (r1 || r2) {
        box_h += 50 * scale_y;
    }

    float box_y = 60 * scale_y; // Posição Y inicial da "caixa"

    char buffer[128]; // Buffer para formatar strings de texto
    float text_y = box_y + 260 * scale_y; // Posição Y para o primeiro texto de pontuação

    if (font) { // Garante que a fonte está disponível antes de desenhar textos
        // Exibe a pontuação atual
        sprintf(buffer, "Sua pontuação: %d", score);
        al_draw_text(fontLarge, al_map_rgb(255, 255, 255), cx, text_y, ALLEGRO_ALIGN_CENTER, buffer);

        // Exibe o recorde pessoal
        sprintf(buffer, "Recorde pessoal: %d", recordPessoal);
        al_draw_text(fontLarge, al_map_rgb(255, 255, 255), cx, text_y + 40 * scale_y, ALLEGRO_ALIGN_CENTER, buffer);

        // Exibe o recorde geral
        sprintf(buffer, "Recorde geral: %d", recordGeral);
        al_draw_text(fontLarge, al_map_rgb(255, 255, 255), cx, text_y + 80 * scale_y, ALLEGRO_ALIGN_CENTER, buffer);

        // Se um novo recorde pessoal foi batido, exibe a mensagem
        if (r1) {
            float ry = text_y + 160 * scale_y;
            al_draw_filled_rounded_rectangle(cx - 180 * scale_x, ry - 10 * scale_y,
                cx + 180 * scale_x, ry + 30 * scale_y, 12, 12, al_map_rgb(255, 255, 255));
            al_draw_text(font, al_map_rgb(0, 0, 0), cx, ry, ALLEGRO_ALIGN_CENTER, "Novo recorde pessoal!");
        }

        // Se um novo recorde geral foi batido, exibe a mensagem (ajusta a posição se houver recorde pessoal também)
        if (r2) {
            float ry = text_y + (r1 ? 220 : 290) * scale_y; // Posiciona abaixo do recorde pessoal se houver
            al_draw_filled_rounded_rectangle(cx - 180 * scale_x, ry - 10 * scale_y,
                cx + 180 * scale_x, ry + 30 * scale_y, 12, 12, al_map_rgb(255, 255, 255));
            al_draw_text(font, al_map_rgb(0, 0, 0), cx, ry, ALLEGRO_ALIGN_CENTER, "Novo recorde geral!");
        }
    }

    float btn_w = 300.0f * scale_x; // Largura dos botões
    float btn_h = 60.0f * scale_y; // Altura dos botões
    float button_gap = 90.0f * scale_y; // Espaçamento entre os botões

    float total_btns_width = (btn_w * 2) + button_gap; // Largura total ocupada pelos dois botões e o espaçamento
    float btn_y = SCREEN_H - (btn_h / 2.0f) - (60.0f * scale_y); // Posição Y dos botões (na parte inferior da tela)

    // Calcula as coordenadas dos retângulos para cada botão
    float first_btn_x1 = cx - (total_btns_width / 2.0f);
    float first_btn_x2 = first_btn_x1 + btn_w;
    float second_btn_x1 = first_btn_x2 + button_gap;
    float second_btn_x2 = second_btn_x1 + btn_w;

    replayBtn = {first_btn_x1, btn_y, first_btn_x2, btn_y + btn_h};
    menuBtn = {second_btn_x1, btn_y, second_btn_x2, btn_y + btn_h};

    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse); // Obtém o estado atual do mouse
    float mx = static_cast<float>(mouse.x);
    float my = static_cast<float>(mouse.y);

    // Verifica se o mouse está sobre os botões
    bool hoverReplay = isMouseOver(replayBtn, mx, my);
    bool hoverMenu = isMouseOver(menuBtn, mx, my);

    // Toca o som de hover se o estado de hover mudou de false para true
    if (hoverReplay && !previousHoverStates[0]) {
        if (somHover) al_play_sample(somHover, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
    }
    if (hoverMenu && !previousHoverStates[1]) {
        if (somHover) al_play_sample(somHover, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
    }

    // Atualiza os estados anteriores de hover
    previousHoverStates[0] = hoverReplay;
    previousHoverStates[1] = hoverMenu;

    // Desenha os botões
    drawButton("Jogar novamente", replayBtn, hoverReplay);
    drawButton("Voltar ao menu", menuBtn, hoverMenu);
}

/**
 * @brief Lida com eventos de entrada na tela de Game Over.
 * @param ev Ponteiro para o evento Allegro a ser processado.
 * @return 1 para "Jogar novamente", 2 para "Voltar ao menu", ou 0 se nenhum botão for clicado.
 */
int GameOverScreen::handleEvent(ALLEGRO_EVENT* ev) {
    if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        float mx = static_cast<float>(ev->mouse.x);
        float my = static_cast<float>(ev->mouse.y);

        if (isMouseOver(replayBtn, mx, my)) {
            return 1; // Retorna 1 para "Jogar novamente"
        } else if (isMouseOver(menuBtn, mx, my)) {
            return 2; // Retorna 2 para "Voltar ao menu"
        }
    }
    return 0; // Nenhum botão foi clicado
}

/**
 * @brief Define o som a ser tocado quando o mouse passa sobre um botão.
 * @param som Ponteiro para o ALLEGRO_SAMPLE do som de hover.
 */
void GameOverScreen::setHoverSound(ALLEGRO_SAMPLE* som) {
    somHover = som;
}