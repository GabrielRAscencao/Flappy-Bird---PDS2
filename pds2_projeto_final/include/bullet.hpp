#ifndef BULLET_HPP
#define BULLET_HPP

#include <allegro5/allegro.h>

class Bullet {
public:
    Bullet(ALLEGRO_BITMAP* sprite, float x, float y, float speed);

    void update(float deltaTime);
    void render() const;
    bool isOffScreen(int screenWidth, int screenHeight) const;

    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

private:
    ALLEGRO_BITMAP* sprite;
    float x, y;
    float speed;  // velocidade horizontal (ex: vai pra direita)
    int width, height;
};

#endif
