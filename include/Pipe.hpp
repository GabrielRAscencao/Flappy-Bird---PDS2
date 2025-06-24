/**
 * @file Pipe.hpp
 * @brief Pipeheader do projeto Traveling Dragon.
 */

#ifndef PIPE_HPP
#define PIPE_HPP

#include "GameObject.hpp"           // A classe base para objetos no jogo (posição, dimensão)
#include <allegro5/allegro_primitives.h> // Para desenhar formas geométricas (não estritamente necessário se usar só bitmap, mas bom ter)
#include <allegro5/allegro_image.h>     // Para carregar e desenhar imagens (bitmaps)

/**
 * @brief Representa um par de obstáculos (cano superior e inferior) no jogo.
 *
 * A classe Pipe gerencia a posição, movimento e detecção de pontuação
 * para um par de canos que o pássaro deve atravessar. Herda de GameObject.
 */
class Pipe : public GameObject {
public:
    /**
     * @brief Construtor da classe Pipe.
     *
     * Inicializa um par de canos com sua posição horizontal de início,
     * a posição vertical do centro do espaço entre eles e a altura desse espaço.
     * Também recebe o bitmap do cano, altura da tela e fatores de escala.
     *
     * @param x_start A coordenada X inicial do cano na tela (direita).
     * @param gap_center_y A coordenada Y do centro do espaço (buraco) entre o cano superior e inferior.
     * @param gap_h A altura do espaço (buraco) entre os canos.
     * @param pipe_bmp O bitmap (imagem) a ser usado para desenhar os canos.
     * @param screenHeight A altura da tela de exibição do jogo.
     * @param sX Fator de escala horizontal para os canos.
     * @param sY Fator de escala vertical para os canos.
     */
    Pipe(float x_start, float gap_center_y, float gap_h, ALLEGRO_BITMAP* pipe_bmp, float screenHeight, float sX, float sY);

    /**
     * @brief Atualiza a posição horizontal do cano.
     * O cano se move da direita para a esquerda. O `deltaTime` é ignorado aqui,
     * pois a velocidade é tratada como fixa por frame, uma escolha comum em jogos com pixel art.
     * @param deltaTime O tempo decorrido desde a última atualização (em segundos).
     */
    void update(float deltaTime);

    /**
     * @brief Renderiza (desenha) o cano superior e o cano inferior na tela.
     */
    void render();

    /**
     * @brief Retorna a coordenada Y do topo do cano superior.
     * @return A coordenada Y superior do cano de cima.
     */
    float getTopPipeY() const;

    /**
     * @brief Retorna a coordenada Y da base do cano inferior.
     * @return A coordenada Y inferior do cano de baixo.
     */
    float getBottomPipeY() const;

    /**
     * @brief Retorna a coordenada Y da borda inferior do cano superior.
     * @return A coordenada Y onde o cano superior termina.
     */
    float getTopPipeBottomY() const;

    /**
     * @brief Retorna a coordenada Y da borda superior do cano inferior.
     * @return A coordenada Y onde o cano inferior começa.
     */
    float getBottomPipeTopY() const;

    /**
     * @brief Retorna o bitmap (sprite) usado para desenhar o cano.
     * @return Um ponteiro para o ALLEGRO_BITMAP do sprite do cano.
     */
    ALLEGRO_BITMAP* getPipeSprite() const {
        return pipeSprite;
    }

    /**
     * @brief Verifica se o pássaro já marcou ponto ao passar por este cano.
     * @return true se o cano já foi contabilizado para a pontuação, false caso contrário.
     */
    bool hasScored() const;

    /**
     * @brief Define o estado de pontuação do cano.
     * Usado para marcar o cano como "pontuado" uma vez que o pássaro o atravessa.
     * @param s O novo estado de pontuação (true para pontuado, false caso contrário).
     */
    void setScored(bool s);

    /**
     * @brief Define a nova velocidade horizontal do cano.
     * @param newSpeedX A nova velocidade em pixels por frame (negativo para mover para a esquerda).
     */
    void setSpeed(float newSpeedX);

private:
    ALLEGRO_BITMAP* pipeSprite;     ///< @brief O bitmap que representa a imagem de um segmento de cano.
    float gapY;                     ///< @brief Coordenada Y do centro do espaço entre o cano superior e o inferior.
    float gapHeight;                ///< @brief Altura do espaço vazio entre o cano superior e o inferior.
    float speedX;                   ///< @brief Velocidade horizontal de movimento do cano (negativa para mover para a esquerda).
    float scale_x, scale_y;         ///< @brief Fatores de escala da tela, usados para ajustar o posicionamento e renderização.
    float spriteScaleX, spriteScaleY; ///< @brief Fatores de escala aplicados diretamente ao sprite do cano para seu redimensionamento.
    bool scored;                    ///< @brief Flag que indica se este cano já contribuiu para a pontuação do jogador.
};

#endif // PIPE_HPP