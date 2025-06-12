#ifndef MENU_HPP
#define MENU_HPP

#include <allegro5/allegro_font.h>
#include <allegro5/allegro.h>
#include <string>
#include <array>

class Menu {
public:
    Menu(ALLEGRO_FONT* font, ALLEGRO_BITMAP* background, int screenWidth, int screenHeight);

    void update(double deltaTime);
    void render();
    void onClick(int mx, int my);
    void onChar(char c, int keycode);
    void resetAction();

    std::string getApelido() const;
    int getSelectedAction() const;

private:
    ALLEGRO_FONT* font;
    ALLEGRO_FONT* fontLarge;
    ALLEGRO_BITMAP* background;
    std::string apelido;
    int selectedAction;
    bool inputActive;
    bool showWarning;
    bool cursorVisible;
    double cursorTimer;

    float buttonWidth;
    float buttonHeight;

    std::array<float, 4> startBtn;
    std::array<float, 4> rankBtn;
    std::array<float, 4> exitBtn;

    float bgOffsetX;
    int screenWidth;
    int screenHeight;

    void updateCursor(double deltaTime);
    void drawButton(const std::string& text, const std::array<float, 4>& rect, bool hover);
    bool isMouseOver(const std::array<float, 4>& rect, int mx, int my);
};

#endif
