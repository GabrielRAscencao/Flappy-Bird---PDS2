#include "Scenario.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <algorithm>
#include <cstdlib>

Scenario::Scenario(ALLEGRO_BITMAP* bg, ALLEGRO_BITMAP* birdBmp, ALLEGRO_BITMAP* pipeBmp, ALLEGRO_FONT* font,
                   int screenWidth, int screenHeight, float scaleX, float scaleY)
    : background(bg), pipeSprite(pipeBmp), font(font), // atribui o parâmetro ao membro
      bird(birdBmp, screenWidth / 3.0f, screenHeight / 2.0f, screenHeight, scaleX, scaleY),
      scrollOffset(0), pipeTimer(0), gameOver(false),
      screenWidth(screenWidth), screenHeight(screenHeight), scaleX(scaleX), scaleY(scaleY)
{
    scoreManager.reset();
    fontLarge = al_load_ttf_font("bin/Minecrafter.Reg.ttf", 40, 0);
    if (!fontLarge) fontLarge = font;  // fallback

}

void Scenario::reset() {

    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float scale = scaleX;
    pipes.clear();
    pontuado.clear();
    bird = Bird(bird.getBitmap(), screenWidth / 3.0f, screenHeight / 2.0f, screenHeight, scaleX, scaleY);
    pipeTimer = 0;
    scrollOffset = 0;
    gameOver = false;
    scoreManager.reset();
}

void Scenario::update(float deltaTime) {
    if (gameOver) return;

    scrollOffset -= 100.0f * deltaTime;

    int bg_w = al_get_bitmap_width(background);
    float img_largura = bg_w * scaleX;

    if (scrollOffset <= -img_largura)
        scrollOffset += img_largura;

    bird.update(deltaTime);

    pipeTimer += deltaTime;
    if (pipeTimer >= 2.5f) {
        spawnPipe();
        pipeTimer = 0;
    }

    for (auto& p : pipes)
        p.update(deltaTime);

    for (size_t i = 0; i < pipes.size(); i += 2) {
        if (!pontuado[i / 2] && pipes[i].getX() + pipes[i].getWidth() < bird.getX()) {
            scoreManager.increment();
            pontuado[i / 2] = true;
        }
    }

    checkCollisions();

    pipes.erase(std::remove_if(pipes.begin(), pipes.end(), [](Pipe& p) {
        return p.isOffScreen();
    }), pipes.end());

    while (pontuado.size() > pipes.size() / 2) {
        pontuado.erase(pontuado.begin());
    }
}

void Scenario::render() {
    int bg_w = al_get_bitmap_width(background);
    int bg_h = al_get_bitmap_height(background);
    float img_largura = bg_w * scaleX;

    al_draw_scaled_bitmap(background, 0, 0, bg_w, bg_h, scrollOffset, 0, img_largura, screenHeight, 0);
    al_draw_scaled_bitmap(background, 0, 0, bg_w, bg_h, scrollOffset + img_largura, 0, img_largura, screenHeight, 0);

    for (auto& p : pipes)
        p.render();

    bird.render();

    al_draw_textf(fontLarge, al_map_rgb(0, 0, 0), 10, 10, 0, "Pontos: %d", scoreManager.getScore());
}

bool Scenario::isGameOver() const {
    return gameOver;
}

Bird& Scenario::getBird() {
    return bird;
}

void Scenario::setGameOver() {
    gameOver = true;
}

int Scenario::getScore() const {
    return scoreManager.getScore();
}

void Scenario::spawnPipe() {
    float pipe_w = 80 * scaleX;
    float minY = 100 * scaleY;
    float gapHeight = 200 * scaleY;
    float maxY = screenHeight - gapHeight - minY;

    float gapY = minY + rand() % int(maxY - minY);

    float pipeSpeed = 100 * scaleX;

    pipes.emplace_back(screenWidth, 0, pipe_w, gapY, pipeSpeed, true, pipeSprite);
    pipes.emplace_back(screenWidth, gapY + gapHeight, pipe_w, screenHeight - (gapY + gapHeight), pipeSpeed, false, pipeSprite);

    pontuado.push_back(false);
}

void Scenario::checkCollisions() {
    for (auto& pipe : pipes) {
        float px1 = pipe.getX();
        float px2 = px1 + pipe.getWidth();
        float py1 = pipe.getY();
        float py2 = py1 + pipe.getHeight();

        float bx1 = bird.getX() - bird.getWidth() / 2;
        float bx2 = bird.getX() + bird.getWidth() / 2;
        float by1 = bird.getY() - bird.getHeight() / 2;
        float by2 = bird.getY() + bird.getHeight() / 2;

        bool colidiuX = bx1 < px2 && bx2 > px1;
        bool colidiuY = by1 < py2 && by2 > py1;

        if (colidiuX && colidiuY) {
            gameOver = true;
            break;
        }
    }
}
