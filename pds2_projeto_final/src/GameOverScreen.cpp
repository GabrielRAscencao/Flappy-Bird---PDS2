#include "GameOverScreen.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <algorithm> // Para std::max
#include <array>     // Para std::array
#include <cstdio>    // Para sprintf

// Construtor
GameOverScreen::GameOverScreen(ALLEGRO_FONT* f, ALLEGRO_BITMAP* bg)
    : font(f), background(bg), scroll(0), selected(0) {
    al_get_display_mode(al_get_num_display_modes() - 1, &display_mode);
    SCREEN_W = al_get_display_width(al_get_current_display());
    SCREEN_H = al_get_display_height(al_get_current_display());
    
    const float DESIGN_W = 1280.0f;
    const float DESIGN_H = 720.0f;
    scale_x = SCREEN_W / DESIGN_W;
    scale_y = SCREEN_H / DESIGN_H;

    fontLarge = al_load_ttf_font("bin/Minecrafter.Reg.ttf", 64 * scale_y, 0);
    if (!fontLarge) fontLarge = font;
}

// Função para desenhar um botão (COM AS CORES DE HOVER DESEJADAS)
void GameOverScreen::drawButton(const std::string& text, const std::array<float, 4>& rect, bool hover) {
    // Cores dos botões: Roxo escuro (normal) e roxo claro/esbranquiçado (hover)
    ALLEGRO_COLOR bg = hover ? al_map_rgb(200, 200, 255) : al_map_rgb(100, 100, 255);
    
    ALLEGRO_COLOR border = al_map_rgb(0, 0, 0);
    float x1 = rect.at(0), y1 = rect.at(1), x2 = rect.at(2), y2 = rect.at(3);

    // Usa retângulos normais, como no Menu.cpp
    al_draw_filled_rectangle(x1 - 2, y1 - 2, x2 + 2, y2 + 2, border);
    al_draw_filled_rectangle(x1, y1, x2, y2, bg);

    float textHeight = al_get_font_line_height(font);
    float text_y_pos = y1 + (y2 - y1 - textHeight) / 2 + 2;

    al_draw_text(font, al_map_rgb(255, 255, 255), (x1 + x2) / 2, text_y_pos, ALLEGRO_ALIGN_CENTER, text.c_str());
}

// Função isMouseOver (SEM MUDANÇAS)
bool GameOverScreen::isMouseOver(const std::array<float, 4>& rect, float mx, float my) {
    return mx >= rect.at(0) && mx <= rect.at(2) && my >= rect.at(1) && my <= rect.at(3);
}

// Função render (SEM MUDANÇAS, pois já está otimizada)
void GameOverScreen::render(int score, int recordPessoal, int recordGeral, bool r1, bool r2) {
    scroll -= 2;
    if (scroll <= -al_get_bitmap_width(background)) scroll = 0;

    int bg_w = al_get_bitmap_width(background);
    int bg_h = al_get_bitmap_height(background);
    
    float bg_scale = std::max(SCREEN_W / (float)bg_w, SCREEN_H / (float)bg_h);

    al_draw_scaled_bitmap(background, 0, 0, bg_w, bg_h, scroll, 0, bg_w * bg_scale, bg_h * bg_scale, 0);
    al_draw_scaled_bitmap(background, 0, 0, bg_w, bg_h, scroll + bg_w * bg_scale, 0, bg_w * bg_scale, bg_h * bg_scale, 0);

    float cx = SCREEN_W / 2.0f;

    float box_w = SCREEN_W * 0.7f;
    float box_h = SCREEN_H * 0.45f;
    if (r1 && r2)
        box_h += 100 * scale_y;
    else if (r1 || r2)
        box_h += 50 * scale_y;

    float box_x = (SCREEN_W - box_w) / 2;
    float box_y = 60 * scale_y;

    al_draw_filled_rounded_rectangle(box_x, box_y, box_x + box_w, box_y + box_h, 20, 20, al_map_rgba(50, 50, 50, 200));

    al_draw_text(fontLarge, al_map_rgb(255, 0, 0), cx, box_y + 20 * scale_y, ALLEGRO_ALIGN_CENTER, "GAME OVER");

    char buffer[128];
    float text_y = box_y + 130 * scale_y;

    sprintf(buffer, "Sua pontuacao: %d", score);
    al_draw_text(font, al_map_rgb(255, 255, 255), cx, text_y, ALLEGRO_ALIGN_CENTER, buffer);

    sprintf(buffer, "Recorde pessoal: %d", recordPessoal);
    al_draw_text(font, al_map_rgb(255, 255, 255), cx, text_y + 40 * scale_y, ALLEGRO_ALIGN_CENTER, buffer);

    sprintf(buffer, "Recorde geral: %d", recordGeral);
    al_draw_text(font, al_map_rgb(255, 255, 255), cx, text_y + 80 * scale_y, ALLEGRO_ALIGN_CENTER, buffer);

    if (r1) {
        float ry = text_y + 130 * scale_y;
        al_draw_filled_rounded_rectangle(cx - 180, ry - 10, cx + 180, ry + 30, 12, 12, al_map_rgb(255, 255, 100));
        al_draw_text(font, al_map_rgb(0, 0, 0), cx, ry, ALLEGRO_ALIGN_CENTER, "Novo recorde pessoal!");
    }

    if (r2) {
        float ry = text_y + (r1 ? 180 : 130) * scale_y;
        al_draw_filled_rounded_rectangle(cx - 180, ry - 10, cx + 180, ry + 30, 12, 12, al_map_rgb(255, 255, 100));
        al_draw_text(font, al_map_rgb(0, 0, 0), cx, ry, ALLEGRO_ALIGN_CENTER, "Novo recorde geral!");
    }

    float btn_w = 300.0f;
    float btn_h = 60.0f;

    float button_gap = 40.0f;
    float total_btns_width = (btn_w * 2) + button_gap;

    float btn_y = SCREEN_H - (btn_h / 2.0f) - 60.0f;

    float first_btn_x1 = cx - (total_btns_width / 2.0f);
    float first_btn_x2 = first_btn_x1 + btn_w;

    float second_btn_x1 = first_btn_x2 + button_gap;
    float second_btn_x2 = second_btn_x1 + btn_w;

    replayBtn = {first_btn_x1, btn_y, first_btn_x2, btn_y + btn_h};
    menuBtn   = {second_btn_x1, btn_y, second_btn_x2, btn_y + btn_h};

    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse);
    float mx = (float)mouse.x;
    float my = (float)mouse.y;

    bool hoverReplay = isMouseOver(replayBtn, mx, my);
    bool hoverMenu   = isMouseOver(menuBtn, mx, my);

    drawButton("Jogar novamente", replayBtn, hoverReplay);
    drawButton("Voltar ao menu", menuBtn, hoverMenu);
}

// Função handleEvent (SEM MUDANÇAS)
int GameOverScreen::handleEvent(ALLEGRO_EVENT ev) {
    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        float mx = (float)ev.mouse.x;
        float my = (float)ev.mouse.y;

        if (isMouseOver(replayBtn, mx, my)) return 1;
        if (isMouseOver(menuBtn, mx, my)) return 2;
    }
    return 0;
}