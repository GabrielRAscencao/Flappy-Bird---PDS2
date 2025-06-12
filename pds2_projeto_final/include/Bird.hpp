#ifndef BIRD_HPP
#define BIRD_HPP

#include "GameObject.hpp"
#include <allegro5/allegro.h>

// classe do passarim q a gente controla
class Bird : public GameObject {
private:
    ALLEGRO_BITMAP* sprite;
    float scaleX;
    float scaleY;
    float velocityY;
    float gravity;
    float flapForce;
    float screenHeight;   // altura da tela

public:
    // construtorzinho
    Bird(ALLEGRO_BITMAP* sprite, float x, float y, float screenHeight, float scaleX = 1.0f, float scaleY = 1.0f);

    // destrutor
    ~Bird();

    void update(float deltaTime) override;
    void render() override;

    void move(float dx, float dy);
    void flap();

    ALLEGRO_BITMAP* getBitmap() const;  // método pra pegar o sprite
};

#endif
