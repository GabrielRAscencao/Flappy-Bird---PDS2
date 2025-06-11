#include "Pipe.hpp"
#include <allegro5/allegro_primitives.h>

Pipe::Pipe(float x, float y, float w, float h, float speed, bool isTop, ALLEGRO_BITMAP* sprite)
    : GameObject(x, y, w, h), speed(speed), isTop(isTop), sprite(sprite) {}

void Pipe::update(float deltaTime) {
    x -= speed * deltaTime;
}

void Pipe::render() {
    if (sprite) {
        int bmpW = al_get_bitmap_width(sprite);
        int bmpH = al_get_bitmap_height(sprite);
        float scaleW = width / bmpW;
        float scaleH = height / bmpH;

        al_draw_scaled_bitmap(
            sprite,
            0, 0, bmpW, bmpH,
            x, y,
            width, height,
            isTop ? ALLEGRO_FLIP_VERTICAL : 0 // vira o cano de cima
        );
    } else {
        al_draw_filled_rectangle(x, y, x + width, y + height, al_map_rgb(0, 255, 0));
    }
}

bool Pipe::isOffScreen() const {
    return x + width < 0;
}
