/**
 * @file GameObject.cpp
 * @brief GameObjectimplementação do projeto Traveling Dragon.
 */


#include "GameObject.hpp"

/**
 * @brief Construtor da classe GameObject.
 * @param x Posição X inicial do objeto.
 * @param y Posição Y inicial do objeto.
 * @param w Largura do objeto.
 * @param h Altura do objeto.
 */
GameObject::GameObject(float x, float y, float w, float h)
    : x(x), y(y), width(w), height(h)
{
    // Construtor simples, inicializa as propriedades básicas
}

/**
 * @brief Move o objeto por uma determinada distância.
 * @param dx Deslocamento no eixo X.
 * @param dy Deslocamento no eixo Y.
 */
void GameObject::move(float dx, float dy) {
    x += dx; // Atualiza a posição X
    y += dy; // Atualiza a posição Y
}