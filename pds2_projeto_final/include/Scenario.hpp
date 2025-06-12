#ifndef SCENARIO_HPP 
#define SCENARIO_HPP

#include <vector>
#include "Bird.hpp"
#include "Pipe.hpp"
#include "ScoreManager.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>

class Scenario {
public:
    // Novo construtor com parâmetros adicionais de resolução e escala
    Scenario(ALLEGRO_BITMAP* bg, ALLEGRO_BITMAP* birdBmp, ALLEGRO_BITMAP* pipeBmp, ALLEGRO_FONT* font,
             int screenWidth, int screenHeight, float scaleX = 1.0f, float scaleY = 1.0f);

    void update(float deltaTime);
    void render();
    bool isGameOver() const;
    void reset();
    int getScore() const;
    Bird& getBird();
    void setGameOver();

private:
    ALLEGRO_BITMAP* background;
    ALLEGRO_BITMAP* pipeSprite;
    ALLEGRO_FONT* font;
    ALLEGRO_FONT* fontLarge;
    Bird bird;
    std::vector<Pipe> pipes;
    std::vector<bool> pontuado;
    float scrollOffset;
    float pipeTimer;
    bool gameOver;

    int screenWidth;
    int screenHeight;
    float scaleX;
    float scaleY;

    ScoreManager scoreManager;

    void spawnPipe();
    void checkCollisions();
    void updateBackground(float deltaTime);
};

#endif
