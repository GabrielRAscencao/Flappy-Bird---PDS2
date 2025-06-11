#include "Bird.hpp"
#include <allegro5/allegro.h>

Bird::Bird(ALLEGRO_BITMAP* sprite, float x, float y, float scale)
    : GameObject(x, y, 0, 0), sprite(sprite), scale(scale) {
    width = al_get_bitmap_width(sprite) * scale;
    height = al_get_bitmap_height(sprite) * scale;
    velocityY = 0;
    gravity = 600.0f; // mais alto = cai mais rapido
    flapForce = -250.0f; // força q sobe qnd aperta espaco
}

Bird::~Bird() {}

void Bird::update(float deltaTime) {
    velocityY += gravity * deltaTime; // aplica gravidade
    y += velocityY * deltaTime;       // muda a posicao vertical

    // trava pra n sair da tela
    if (y < height / 2) {
        y = height / 2;
        velocityY = 0;
    }
    if (y > 600 - height / 2) {
        y = 600 - height / 2;
        velocityY = 0;
    }
}

void Bird::render() {
    al_draw_scaled_bitmap(sprite,
        0, 0,
        al_get_bitmap_width(sprite), al_get_bitmap_height(sprite),
        x - width / 2, y - height / 2,
        width, height,
        0);
}

void Bird::move(float dx, float dy) {
    x += dx;
    y += dy;
}

void Bird::flap() {
    velocityY = flapForce; // sobe com a força do pulo
}

