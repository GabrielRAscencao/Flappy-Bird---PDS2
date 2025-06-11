#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <allegro5/allegro.h>

// classe base dos objetos q vao aparecer na tela
class GameObject {
protected:
    float x, y;              // posicao do obj na tela
    float width, height;     // tamanho

public:
    GameObject(float x, float y, float w, float h);
    virtual ~GameObject() {}

    virtual void update(float deltaTime) = 0; // atualiza posicao ou estado
    virtual void render() = 0;                // desenha o obj na tela

    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
};

#endif