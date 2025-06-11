#ifndef PIPE_HPP
#define PIPE_HPP

#include "GameObject.hpp"
#include <allegro5/allegro.h>

// cano do jogo (vem de par: cima + baixo)
class Pipe : public GameObject {
private:
    float speed;
    bool isTop;
    ALLEGRO_BITMAP* sprite; // agora usa img ao inves de cor

public:
    Pipe(float x, float y, float w, float h, float speed, bool isTop, ALLEGRO_BITMAP* sprite);
    void update(float deltaTime) override;
    void render() override;

    bool isOffScreen() const;
};

#endif

