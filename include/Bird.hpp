/**
 * @file Bird.hpp
 * @brief Birdheader do projeto Traveling Dragon.
 */

#ifndef BIRD_HPP
#define BIRD_HPP

#include <allegro5/allegro.h> // Necessário para usar tipos do Allegro, como ALLEGRO_BITMAP
#include "GameObject.hpp"     // A classe base da qual Bird herda propriedades e comportamentos

/**
 * @brief Representa o personagem principal do jogo, o pássaro.
 *
 * A classe Bird gerencia a lógica de movimento, física (gravidade, pulo),
 * animação e rotação do pássaro na tela. Herda de GameObject para ter
 * atributos de posição e dimensão.
 */
class Bird : public GameObject {
public:
    /// @brief Ângulo máximo de rotação para cima do pássaro.
    static const float MAX_UP_ROTATION;
    /// @brief Ângulo máximo de rotação para baixo do pássaro.
    static const float MAX_DOWN_ROTATION;

    /// @brief Proporção da altura da tela para a posição Y inicial do pássaro.
    static const float INITIAL_Y_RATIO;
    /// @brief Proporção para o deslocamento X do centro da tela para o pássaro.
    static const float CENTER_X_OFFSET_RATIO;

    /**
     * @brief Ativa ou desativa a rotação visual do pássaro com base na sua velocidade vertical.
     * @param on Se true, a rotação é ativada; se false, o pássaro mantém um ângulo fixo.
     */
    void setRotacionar(bool on);

    /**
     * @brief Verifica se a rotação do pássaro está ativa.
     * @return true se a rotação estiver ativa, false caso contrário.
     */
    bool isRotacionando() const;

    /**
     * @brief Retorna o frame (imagem) atual da animação do pássaro.
     * @return Um ponteiro para o ALLEGRO_BITMAP correspondente ao frame atual.
     */
    ALLEGRO_BITMAP* getCurrentFrame() const {
        return frames[currentFrame];
    }

    /**
     * @brief Construtor da classe Bird.
     *
     * Inicializa o pássaro configurando sua folha de sprites, as dimensões da tela
     * e os fatores de escala para o posicionamento e renderização.
     *
     * @param sheet A folha de sprites (ALLEGRO_BITMAP) contendo as imagens do pássaro.
     * @param screenW Largura da tela de exibição do jogo.
     * @param screenH Altura da tela de exibição do jogo.
     * @param sX Fator de escala horizontal para o pássaro.
     * @param sY Fator de escala vertical para o pássaro.
     */
    Bird(ALLEGRO_BITMAP* sheet, float screenW, float screenH, float sX, float sY);

    /**
     * @brief Destrutor da classe Bird.
     * Responsável por liberar quaisquer recursos alocados dinamicamente, se houver.
     */
    ~Bird();

    /**
     * @brief Aplica uma força vertical para cima no pássaro, simulando um "flap" (bater asas).
     * Isso altera a velocidade vertical do pássaro.
     */
    void flap();

    /**
     * @brief Atualiza a lógica do pássaro a cada frame do jogo.
     *
     * Gerencia a aplicação da gravidade, movimento vertical, atualização da animação
     * e a rotação visual do pássaro com base em sua velocidade.
     *
     * @param deltaTime O tempo decorrido desde a última atualização (em segundos).
     */
    void update(float deltaTime);

    /**
     * @brief Renderiza (desenha) o pássaro na tela na sua posição e com sua rotação atual.
     */
    void render();

    /**
     * @brief Reseta o estado do pássaro para as condições iniciais de uma nova partida.
     * Isso inclui posição, velocidade, ângulo de rotação e estado de animação.
     */
    void reset();

private:
    ALLEGRO_BITMAP* spriteSheet;    ///< @brief A folha de sprites completa do pássaro.
    ALLEGRO_BITMAP* frames[3];      ///< @brief Array de 3 frames individuais do pássaro, extraídos da spriteSheet.
    float screenHeight;             ///< @brief Altura da tela de referência para cálculos de posição.
    float screenWidth;              ///< @brief Largura da tela de referência para cálculos de posição.
    float scaleX, scaleY;           ///< @brief Fatores de escala aplicados ao tamanho do pássaro.

    float velocityY;    ///< @brief Velocidade vertical atual do pássaro.
    float gravity;      ///< @brief Força da gravidade que puxa o pássaro para baixo.
    float flapForce;    ///< @brief Força aplicada ao pássaro quando ele bate as asas.

    int currentFrame;           ///< @brief O índice do frame atual da animação do pássaro.
    double frameTimer;          ///< @brief Timer para controlar a transição entre os frames da animação.
    double animationSpeed;      ///< @brief Velocidade da animação (tempo entre a troca de frames).

    float rotationAngle;        ///< @brief Ângulo de rotação atual do pássaro.
    float targetRotationAngle;  ///< @brief Ângulo para o qual o pássaro está rotacionando.
    float rotationSpeed;        ///< @brief Velocidade com que o pássaro muda seu ângulo de rotação.

    float frameWidth;           ///< @brief Largura de um único frame (sprite) do pássaro.
    float frameHeight;          ///< @brief Altura de um único frame (sprite) do pássaro.

    bool rotacionar = true;     ///< @brief Flag que indica se o pássaro deve rotacionar ou permanecer reto.
};

#endif // BIRD_HPP