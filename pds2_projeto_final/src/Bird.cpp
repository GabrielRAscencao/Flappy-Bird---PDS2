#include "Bird.hpp"
#include <allegro5/allegro.h>

// construtor do passarim
Bird::Bird(ALLEGRO_BITMAP* sprite, float x, float y, float screenHeight, float scaleX, float scaleY)
    : GameObject(x, y,
                 al_get_bitmap_width(sprite) * scaleX,
                 al_get_bitmap_height(sprite) * scaleY),
      sprite(sprite), scaleX(scaleX), scaleY(scaleY), screenHeight(screenHeight) {
    velocityY = 0;
    gravity = 600.0f * scaleY;
    flapForce = -250.0f * scaleY;
}

// destrutor
Bird::~Bird() {
    // nada por enquanto
}

void Bird::update(float deltaTime) {
    velocityY += gravity * deltaTime;
    y += velocityY * deltaTime;

    if (y < height / 2) {
        y = height / 2;
        velocityY = 0;
    }

    if (y > screenHeight - height / 2) {
        y = screenHeight - height / 2;
        velocityY = 0;
    }
}

void Bird::render() {
    int bmpWidth = al_get_bitmap_width(sprite);
    int bmpHeight = al_get_bitmap_height(sprite);

    float renderWidth = bmpWidth * scaleX;
    float renderHeight = bmpHeight * scaleY;

    al_draw_scaled_bitmap(sprite,
        0, 0,
        bmpWidth, bmpHeight,
        x - renderWidth / 2, y - renderHeight / 2,
        renderWidth, renderHeight,
        0);
}

void Bird::move(float dx, float dy) {
    x += dx;
    y += dy;
}

void Bird::flap() {
    velocityY = flapForce;
}

ALLEGRO_BITMAP* Bird::getBitmap() const {
    return sprite;
}
