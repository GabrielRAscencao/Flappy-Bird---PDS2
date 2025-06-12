#include "GameOverScreen.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <algorithm>

GameOverScreen::GameOverScreen(ALLEGRO_FONT* f, ALLEGRO_BITMAP* bg)
    : font(f), background(bg), scroll(0), selected(0) {
    al_get_display_mode(al_get_num_display_modes() - 1, &display_mode);
    SCREEN_W = al_get_display_width(al_get_current_display());
    SCREEN_H = al_get_display_height(al_get_current_display());
    scale_x = SCREEN_W / 800.0f;
    scale_y = SCREEN_H / 600.0f;

    // Fonte grande para "Game Over"
    fontLarge = al_load_ttf_font("bin/Minecrafter.Reg.ttf", 64 * scale_y, 0);
    if (!fontLarge) fontLarge = font;  // fallback
}

void GameOverScreen::render(int score, int recordPessoal, int recordGeral, bool r1, bool r2) {
    scroll -= 2;
    if (scroll <= -al_get_bitmap_width(background)) scroll = 0;

    int bg_w = al_get_bitmap_width(background);
    int bg_h = al_get_bitmap_height(background);
    float scale = std::max(SCREEN_W / (float)bg_w, SCREEN_H / (float)bg_h);

    al_draw_scaled_bitmap(background, 0, 0, bg_w, bg_h, scroll, 0, bg_w * scale, bg_h * scale, 0);
    al_draw_scaled_bitmap(background, 0, 0, bg_w, bg_h, scroll + bg_w * scale, 0, bg_w * scale, bg_h * scale, 0);

    float cx = SCREEN_W / 2.0f;

    // Cálculo do balão principal (ajustado)
    float box_w = SCREEN_W * 0.7f;
    float box_h = SCREEN_H * 0.45f;
    if (r1 && r2)
        box_h += 100 * scale_y;
    else if (r1 || r2)
        box_h += 50 * scale_y;

    float box_x = (SCREEN_W - box_w) / 2;
    float box_y = 60 * scale_y;

    al_draw_filled_rounded_rectangle(box_x, box_y, box_x + box_w, box_y + box_h, 20, 20, al_map_rgba(50, 50, 50, 200));

    // GAME OVER com fonte grande
    al_draw_text(fontLarge, al_map_rgb(255, 0, 0), cx, box_y + 20 * scale_y, ALLEGRO_ALIGN_CENTER, "GAME OVER");

    char buffer[128];
    float text_y = box_y + 130 * scale_y;

    sprintf(buffer, "Sua pontuacao: %d", score);
    al_draw_text(font, al_map_rgb(255, 255, 255), cx, text_y, ALLEGRO_ALIGN_CENTER, buffer);

    sprintf(buffer, "Recorde pessoal: %d", recordPessoal);
    al_draw_text(font, al_map_rgb(255, 255, 255), cx, text_y + 40 * scale_y, ALLEGRO_ALIGN_CENTER, buffer);

    sprintf(buffer, "Recorde geral: %d", recordGeral);
    al_draw_text(font, al_map_rgb(255, 255, 255), cx, text_y + 80 * scale_y, ALLEGRO_ALIGN_CENTER, buffer);

    // Balões individuais de recorde
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

    // Botões
    float btn_w = 200 * scale_x;
    float btn_h = 50 * scale_y;
    float btn_y = SCREEN_H - 100 * scale_y;

    replayBtn = {cx - btn_w - 20, btn_y, cx - 20, btn_y + btn_h};
    menuBtn   = {cx + 20,        btn_y, cx + btn_w + 20, btn_y + btn_h};

    // Hover detection
    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse);
    float mx = mouse.x;
    float my = mouse.y;

    bool hoverReplay = (mx >= replayBtn[0] && mx <= replayBtn[2] && my >= replayBtn[1] && my <= replayBtn[3]);
    bool hoverMenu   = (mx >= menuBtn[0]   && mx <= menuBtn[2]   && my >= menuBtn[1]   && my <= menuBtn[3]);

    ALLEGRO_COLOR bg_normal = al_map_rgb(200, 200, 200);
    ALLEGRO_COLOR bg_hover = al_map_rgb(120, 120, 255);
    ALLEGRO_COLOR border = al_map_rgb(0, 0, 0);

    // "Jogar novamente"
    al_draw_filled_rounded_rectangle(replayBtn[0] - 2, replayBtn[1] - 2, replayBtn[2] + 2, replayBtn[3] + 2, 10, 10, border);
    al_draw_filled_rounded_rectangle(replayBtn[0], replayBtn[1], replayBtn[2], replayBtn[3], 10, 10, hoverReplay ? bg_hover : bg_normal);
    al_draw_text(font, al_map_rgb(0, 0, 0), (replayBtn[0] + replayBtn[2]) / 2, replayBtn[1] + 10 * scale_y, ALLEGRO_ALIGN_CENTER, "Jogar novamente");

    // "Voltar ao menu"
    al_draw_filled_rounded_rectangle(menuBtn[0] - 2, menuBtn[1] - 2, menuBtn[2] + 2, menuBtn[3] + 2, 10, 10, border);
    al_draw_filled_rounded_rectangle(menuBtn[0], menuBtn[1], menuBtn[2], menuBtn[3], 10, 10, hoverMenu ? bg_hover : bg_normal);
    al_draw_text(font, al_map_rgb(0, 0, 0), (menuBtn[0] + menuBtn[2]) / 2, menuBtn[1] + 10 * scale_y, ALLEGRO_ALIGN_CENTER, "Voltar ao menu");
}


int GameOverScreen::handleEvent(ALLEGRO_EVENT ev) {
    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        float mx = ev.mouse.x;
        float my = ev.mouse.y;

        if (mx >= replayBtn[0] && mx <= replayBtn[2] && my >= replayBtn[1] && my <= replayBtn[3]) return 1;
        if (mx >= menuBtn[0] && mx <= menuBtn[2] && my >= menuBtn[1] && my <= menuBtn[3]) return 2;
    }
    return 0;
}
