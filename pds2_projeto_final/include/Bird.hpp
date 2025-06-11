#ifndef BIRD_HPP
#define BIRD_HPP

#include "GameObject.hpp"
#include <allegro5/allegro.h>

// classe do passarim q a gente controla
class Bird : public GameObject {
private:
    ALLEGRO_BITMAP* sprite;
    float scale;
    float velocityY;           // velocidade pra baixo
    float gravity;             // aceleração puxa pra baixo
    float flapForce;           // força do pulo

public:
    Bird(ALLEGRO_BITMAP* sprite, float x, float y, float scale = 1.0f);
    ~Bird();

    void update(float deltaTime) override; // agora atualiza com gravidade
    void render() override;                // desenha o bicho

    void move(float dx, float dy);         // movimenta o passarim
    void flap();                           // faz o passarim subir
};

#endif
