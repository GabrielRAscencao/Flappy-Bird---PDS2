#ifndef GAMEOVERSCREEN_HPP
#define GAMEOVERSCREEN_HPP

#include <allegro5/allegro_font.h>
#include <allegro5/allegro.h>
#include <string>
#include <array>

class GameOverScreen {
public:
    GameOverScreen(ALLEGRO_FONT* font, ALLEGRO_BITMAP* bg);
    void render(int score, int recordPessoal, int recordGeral, bool bateuRecordePessoal, bool bateuRecordeGeral);
    int handleEvent(ALLEGRO_EVENT ev);

private:
    ALLEGRO_FONT* font;
    ALLEGRO_FONT* fontLarge;
    ALLEGRO_BITMAP* background;
    float scroll;
    float SCREEN_W, SCREEN_H;
    float scale_x, scale_y;
    ALLEGRO_DISPLAY_MODE display_mode;
    std::array<float, 4> replayBtn;
    std::array<float, 4> menuBtn;
    int selected;

    // --- NOVOS MÉTODOS ADICIONADOS ---
    void drawButton(const std::string& text, const std::array<float, 4>& rect, bool hover);
    bool isMouseOver(const std::array<float, 4>& rect, float mx, float my);
    // ---------------------------------
};

#endif