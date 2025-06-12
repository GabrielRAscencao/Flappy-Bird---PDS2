#include "Menu.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>

Menu::Menu(ALLEGRO_FONT* fonte, ALLEGRO_BITMAP* fundo, int screenWidth, int screenHeight)
    : font(fonte), background(fundo), screenWidth(screenWidth), screenHeight(screenHeight),
      apelido(""), selectedAction(0), inputActive(false), showWarning(false),
      cursorVisible(true), cursorTimer(0.0), bgOffsetX(0.0f)
{
    fontLarge = al_load_ttf_font("bin/Minecrafter.Reg.ttf", 72, 0);
    if (!fontLarge) fontLarge = font;  // fallback

    buttonWidth = 300.0f;
    buttonHeight = 60.0f;

    const float centerX = screenWidth / 2.0f;
    // Usando proporções relativas à altura da tela para o Y dos botões
    float startBtnY = screenHeight * 0.4f;
    float rankBtnY  = screenHeight * 0.52f;
    float exitBtnY  = screenHeight * 0.64f;

    startBtn = {centerX - buttonWidth / 2.0f, startBtnY, centerX + buttonWidth / 2.0f, startBtnY + buttonHeight};
    rankBtn  = {centerX - buttonWidth / 2.0f, rankBtnY,  centerX + buttonWidth / 2.0f, rankBtnY + buttonHeight};
    exitBtn  = {centerX - buttonWidth / 2.0f, exitBtnY,  centerX + buttonWidth / 2.0f, exitBtnY + buttonHeight};
}

void Menu::update(double deltaTime) {
    bgOffsetX -= 50.0f * deltaTime;
    int bgWidth = al_get_bitmap_width(background);
    if (bgOffsetX <= -bgWidth) {
        bgOffsetX += bgWidth;
    }
    updateCursor(deltaTime);
}

void Menu::updateCursor(double deltaTime) {
    if (!inputActive) {
        cursorVisible = false;
        cursorTimer = 0.0;
        return;
    }

    cursorTimer += deltaTime;
    if (cursorTimer >= 0.5) {
        cursorVisible = !cursorVisible;
        cursorTimer = 0.0;
    }
}

void Menu::render() {
    int bgWidth = al_get_bitmap_width(background);
    int bgHeight = al_get_bitmap_height(background);
  
    al_draw_scaled_bitmap(background, 0, 0, bgWidth, bgHeight, (int)bgOffsetX, 0, bgWidth, screenHeight, 0);
    al_draw_scaled_bitmap(background, 0, 0, bgWidth, bgHeight, (int)(bgOffsetX + bgWidth), 0, bgWidth, screenHeight, 0);

    // Usando proporção para o Y do título
    al_draw_text(fontLarge, al_map_rgb(0, 0, 0), screenWidth / 2, screenHeight * 0.12f, ALLEGRO_ALIGN_CENTER, "FLAPPY BIRD");

    const float inputY = screenHeight * 0.27f;
    const char* texto = (apelido.empty() && !inputActive) ? "Digite seu apelido" : apelido.c_str();
    int textWidth = al_get_text_width(font, texto);
    float textHeight = al_get_font_line_height(font);
    float paddingX = 20.0f, paddingY = 10.0f;

    float boxW = (float)(textWidth + paddingX * 2);
    if (boxW < 20.0f) boxW = 20.0f;
    float boxX = screenWidth / 2.0f - boxW / 2.0f;
    float boxY = inputY;
    float boxH = textHeight + paddingY;

    al_draw_filled_rounded_rectangle(boxX, boxY, boxX + boxW, boxY + boxH, 10, 10, al_map_rgb(255, 255, 255));
    al_draw_rounded_rectangle(boxX, boxY, boxX + boxW, boxY + boxH, 10, 10, al_map_rgb(0, 0, 0), 2);

    float textY = boxY + (boxH - textHeight) / 2 + 3;

    if (apelido.empty() && !inputActive) {
        al_draw_text(font, al_map_rgb(150, 150, 150), screenWidth / 2.0f, textY, ALLEGRO_ALIGN_CENTER, "Digite seu apelido");
    } else {
        al_draw_text(font, al_map_rgb(0, 0, 0), boxX + paddingX, textY, 0, apelido.c_str());

        if (inputActive && cursorVisible) {
            float cursorX = boxX + paddingX + textWidth;
            float cursorY1 = boxY + 6;
            float cursorY2 = boxY + boxH - 6;
            al_draw_filled_rectangle(cursorX, cursorY1, cursorX + 2, cursorY2, al_map_rgb(0, 0, 0));
        }
    }

    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse);

    drawButton("Jogar", startBtn, isMouseOver(startBtn, mouse.x, mouse.y));
    drawButton("Ranking", rankBtn, isMouseOver(rankBtn, mouse.x, mouse.y));
    drawButton("Sair", exitBtn, isMouseOver(exitBtn, mouse.x, mouse.y));

    if (showWarning) {
        al_draw_text(font, al_map_rgb(255, 0, 0), screenWidth / 2, inputY + boxH + 10, ALLEGRO_ALIGN_CENTER, "Apelido nao pode ser vazio!");
    }
}

void Menu::drawButton(const std::string& text, const std::array<float, 4>& rect, bool hover) {
    ALLEGRO_COLOR bg = hover ? al_map_rgb(200, 200, 255) : al_map_rgb(100, 100, 255);
    float x1 = rect[0], y1 = rect[1], x2 = rect[2], y2 = rect[3];

    al_draw_filled_rectangle(x1, y1, x2, y2, bg);
    al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(0, 0, 0), 2);

    float textHeight = al_get_font_line_height(font);
    al_draw_text(font, al_map_rgb(255, 255, 255), (x1 + x2) / 2, y1 + (y2 - y1 - textHeight) / 2, ALLEGRO_ALIGN_CENTER, text.c_str());
}

bool Menu::isMouseOver(const std::array<float, 4>& rect, int mx, int my) {
    return mx >= rect[0] && mx <= rect[2] && my >= rect[1] && my <= rect[3];
}

void Menu::onClick(int mx, int my) {
    const float inputY = screenHeight * 0.27f;
    float paddingX = 20.0f;
    int textWidth = al_get_text_width(font, apelido.c_str());
    float boxW = (float)(textWidth + paddingX * 2);
    if (boxW < 20.0f) boxW = 20.0f;
    const float inputX = screenWidth / 2.0f - boxW / 2.0f;
    const float inputHeight = al_get_font_line_height(font) + 10.0f;

    bool clickInput = (mx >= inputX && mx <= inputX + boxW &&
                       my >= inputY && my <= inputY + inputHeight);

    if (clickInput) {
        inputActive = true;
        return;
    }

    inputActive = false;

    if (isMouseOver(startBtn, mx, my)) {
        if (apelido.empty()) {
            showWarning = true;
        } else {
            showWarning = false;
            selectedAction = 1;
        }
    }
    else if (isMouseOver(rankBtn, mx, my)) {
        selectedAction = 2;
    }
    else if (isMouseOver(exitBtn, mx, my)) {
        selectedAction = 3;
    }
}

void Menu::onChar(char c, int keycode) {
    if (!inputActive) return;

    if (keycode == ALLEGRO_KEY_BACKSPACE) {
        if (!apelido.empty()) {
            apelido.pop_back();
        }
    } else if (keycode == ALLEGRO_KEY_ENTER) {
        inputActive = false;
    } else {
        if (apelido.size() >= 15) return;
        if (!(isalnum(c) || c == '_' || c == '-' || c == ' ')) return;
        if (apelido.empty() && c == ' ') return;
        if (!apelido.empty() && apelido.back() == ' ' && c == ' ') return;
        apelido.push_back(c);
    }
}

void Menu::resetAction() {
    selectedAction = -1;
}

std::string Menu::getApelido() const {
    return apelido;
}

int Menu::getSelectedAction() const {
    return selectedAction;
}
