/**
 * @file GameObject.hpp
 * @brief GameObjectheader do projeto Traveling Dragon.
 */

#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

/**
 * @brief Classe base abstrata para todos os objetos visuais no jogo.
 *
 * Define as propriedades fundamentais de posição (x, y) e dimensão (width, height)
 * para qualquer objeto que precisa ser renderizado ou interagir no mundo do jogo.
 * Fornece métodos básicos para acessar essas propriedades e mover o objeto.
 */
class GameObject {
protected:
    float x;        ///< @brief Coordenada X da posição do objeto (canto superior esquerdo).
    float y;        ///< @brief Coordenada Y da posição do objeto (canto superior esquerdo).
    float width;    ///< @brief Largura do objeto.
    float height;   ///< @brief Altura do objeto.

public:
    /**
     * @brief Construtor da classe GameObject.
     *
     * Inicializa a posição e as dimensões do objeto. Este construtor é definido
     * inline no cabeçalho por ser simples e direto.
     *
     * @param initialX A coordenada X inicial do objeto.
     * @param initialY A coordenada Y inicial do objeto.
     * @param w A largura do objeto.
     * @param h A altura do objeto.
     */
    GameObject(float initialX, float initialY, float w, float h)
        : x(initialX), y(initialY), width(w), height(h) {}

    /**
     * @brief Destrutor virtual da classe GameObject.
     *
     * Essencial para garantir o polimorfismo correto. Quando um objeto derivado
     * é deletado através de um ponteiro para GameObject, este destrutor virtual
     * assegura que o destrutor da classe mais derivada seja chamado, evitando vazamentos de memória.
     * O `= default` indica que o compilador deve gerar a implementação padrão.
     */
    virtual ~GameObject() = default;

    /**
     * @brief Retorna a coordenada X atual do objeto.
     * @return A coordenada X.
     */
    float getX() const { return x; }

    /**
     * @brief Retorna a coordenada Y atual do objeto.
     * @return A coordenada Y.
     */
    float getY() const { return y; }

    /**
     * @brief Retorna a largura do objeto.
     * @return A largura.
     */
    float getWidth() const { return width; }

    /**
     * @brief Retorna a altura do objeto.
     * @return A altura.
     */
    float getHeight() const { return height; }

    /**
     * @brief Move o objeto pela quantidade especificada.
     *
     * Este é um método virtual, o que significa que classes derivadas podem
     * sobrescrever esta implementação para adicionar comportamentos de movimento
     * específicos. A implementação padrão simplesmente adiciona os deslocamentos
     * às coordenadas atuais.
     *
     * @param dx O deslocamento a ser aplicado no eixo X.
     * @param dy O deslocamento a ser aplicado no eixo Y.
     */
    virtual void move(float dx, float dy) {
        x += dx;
        y += dy;
    }
};

#endif // GAMEOBJECT_HPP