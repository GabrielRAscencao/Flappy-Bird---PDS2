#include "Bullet.hpp"

Bullet::Bullet(ALLEGRO_BITMAP* sprite, float x, float y, float speed)
    : sprite(sprite), x(x), y(y), speed(speed)
{
    width = al_get_bitmap_width(sprite);
    height = al_get_bitmap_height(sprite);
}

void Bullet::update(float deltaTime) {
    // Movimento para a direita com velocidade constante
    x += speed * deltaTime;
}

void Bullet::render() const {
    al_draw_bitmap(sprite, x - width / 2, y - height / 2, 0);
}

bool Bullet::isOffScreen(int screenWidth, int screenHeight) const {
    return (x - width / 2 > screenWidth) || (y < 0) || (y > screenHeight);
}

float Bullet::getX() const { return x; }
float Bullet::getY() const { return y; }
float Bullet::getWidth() const { return width; }
float Bullet::getHeight() const { return height; }
