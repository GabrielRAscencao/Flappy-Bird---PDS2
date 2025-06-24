/**
 * @file Pipe.cpp
 * @brief Pipeimplementação do projeto Traveling Dragon.
 */


#include "Pipe.hpp"
#include <allegro5/allegro_primitives.h> // Para desenhar formas primitivas (fallback)
#include <iostream> // Para saída de avisos
#include <cmath> // Para std::round

/**
 * @brief Construtor da classe Pipe.
 *
 * Cria um novo par de canos com uma lacuna (gap) específica.
 *
 * @param x_start Posição X inicial do cano.
 * @param gap_center_y Posição Y central da lacuna entre os canos.
 * @param gap_h Altura da lacuna entre os canos.
 * @param pipe_bmp Ponteiro para o bitmap da imagem do cano.
 * @param screenHeight Altura total da tela do jogo.
 * @param sX Fator de escala X para o cano.
 * @param sY Fator de escala Y para o cano.
 */
Pipe::Pipe(float x_start, float gap_center_y, float gap_h, ALLEGRO_BITMAP* pipe_bmp, float screenHeight, float sX, float sY)
    : GameObject(x_start, 0.0f, // Posição Y inicial é 0, altura total da tela
                     538.0f * sX, // Largura inicial baseada na imagem original e escala
                     screenHeight), // Altura do GameObject é a altura da tela
      pipeSprite(pipe_bmp),
      gapY(gap_center_y),
      gapHeight(gap_h),
      // A velocidade e a escala são ajustadas abaixo, mas inicializadas aqui
      speedX(-2.0f * sX),
      scale_x(sX),
      scale_y(sY),
      scored(false) // Flag para controlar se o pássaro já marcou ponto passando por este cano
{
    // Sobrescreve os valores recebidos para garantir uma escala padrão de design, se necessário.
    // Isso é útil se os assets foram desenhados para uma escala específica e queremos forçar essa escala.
    sX = 0.25f;
    sY = 0.25f;

    // Atualiza os fatores de escala internos com os valores forçados.
    scale_x = sX;
    scale_y = sY;

    // E também atualiza os valores dependentes da escala:
    this->width = 538.0f * scale_x; // Recalcula a largura com a nova escala
    // Recalcula a velocidade horizontal com a nova escala, para consistência de movimento
    speedX = -150.0f * scale_x;

    // Verifica se o sprite do cano foi carregado corretamente
    if (!pipeSprite) {
        std::cerr << "AVISO: Sprite do cano não carregado. Usando dimensões padrão." << std::endl;
    }
}

/**
 * @brief Atualiza o estado do cano a cada frame do jogo.
 * @param deltaTime O tempo decorrido desde a última atualização, para movimento baseado em tempo.
 */
void Pipe::update(float deltaTime) {
    // Move o cano horizontalmente para a esquerda com base na sua velocidade
    x += speedX * deltaTime;
}

/**
 * @brief Renderiza o par de canos na tela.
 * Desenha o cano superior e o cano inferior, aplicando as transformações necessárias.
 */
void Pipe::render() {
    // Arredonda a posição X para um pixel inteiro para evitar artefatos de renderização
    int drawX = static_cast<int>(std::round(this->x));

    if (pipeSprite) {
        int bmpW = al_get_bitmap_width(pipeSprite); // Deve ser 538 (largura original do asset)
        int bmpH = al_get_bitmap_height(pipeSprite); // Deve ser 3310 (altura original do asset)

        // Prevenção de divisão por zero ou dimensões inválidas
        if (bmpW <= 0) bmpW = 1;
        if (bmpH <= 0) bmpH = 1;

        // Calcula a largura e altura do cano após aplicar a escala
        float pipeDrawWidth = static_cast<float>(bmpW) * scale_x;
        float pipeDrawHeight = static_cast<float>(bmpH) * scale_y;

        // --- CANO DE CIMA ---
        // Calcula a coordenada Y da parte inferior do cano de cima
        float topPipeBottomY = getTopPipeBottomY();
        // Calcula a coordenada Y do topo do cano de cima (onde ele começa a ser desenhado)
        float topPipeY = topPipeBottomY - pipeDrawHeight;

        // Desenha o cano de cima, escalado, na posição correta e espelhado verticalmente
        al_draw_scaled_bitmap(
            pipeSprite,
            0, 0, bmpW, bmpH, // Região do bitmap de origem
            drawX, topPipeY, // Posição de destino na tela
            pipeDrawWidth, pipeDrawHeight, // Dimensões de destino
            ALLEGRO_FLIP_VERTICAL // Inverte verticalmente para o cano superior
        );

        // --- CANO DE BAIXO ---
        // Calcula a coordenada Y da parte superior do cano de baixo
        float bottomPipeTopY = getBottomPipeTopY();

        // Desenha o cano de baixo, escalado, na posição correta
        al_draw_scaled_bitmap(
            pipeSprite,
            0, 0, bmpW, bmpH, // Região do bitmap de origem
            drawX, bottomPipeTopY, // Posição de destino na tela
            pipeDrawWidth, pipeDrawHeight, // Dimensões de destino
            0 // Sem flags de inversão
        );

    } else {
        // Fallback: se o sprite não for carregado, desenha retângulos verdes
        al_draw_filled_rectangle(drawX, 0, drawX + this->width, getTopPipeBottomY(), al_map_rgb(0, 255, 0));
        al_draw_filled_rectangle(drawX, getBottomPipeTopY(), drawX + this->width, this->height, al_map_rgb(0, 255, 0));
    }
}

/**
 * @brief Obtém a coordenada Y do topo do cano superior.
 * @return Sempre 0.0f, pois o cano superior se estende do topo da tela.
 */
float Pipe::getTopPipeY() const {
    return 0.0f;
}

/**
 * @brief Obtém a coordenada Y da base do cano inferior.
 * @return A coordenada Y da base do cano inferior.
 */
float Pipe::getBottomPipeY() const {
    // A base do cano inferior está na altura da lacuna mais metade da altura da lacuna
    return gapY + (gapHeight / 2.0f);
}

/**
 * @brief Obtém a coordenada Y da parte inferior do cano superior.
 * Esta é a borda inferior do cano superior, ou seja, o topo da lacuna.
 * @return A coordenada Y da parte inferior do cano superior.
 */
float Pipe::getTopPipeBottomY() const {
    return gapY - (gapHeight / 2.0f);
}

/**
 * @brief Obtém a coordenada Y da parte superior do cano inferior.
 * Esta é a borda superior do cano inferior, ou seja, a base da lacuna.
 * @return A coordenada Y da parte superior do cano inferior.
 */
float Pipe::getBottomPipeTopY() const {
    return gapY + (gapHeight / 2.0f);
}

/**
 * @brief Verifica se o pássaro já marcou ponto passando por este cano.
 * @return True se o ponto já foi marcado, false caso contrário.
 */
bool Pipe::hasScored() const {
    return scored;
}

/**
 * @brief Define o status de pontuação do cano.
 * @param s True para marcar que o ponto foi feito, false caso contrário.
 */
void Pipe::setScored(bool s) {
    scored = s;
}

/**
 * @brief Define a velocidade horizontal de movimento do cano.
 * @param newSpeedX A nova velocidade no eixo X (normalmente um valor negativo para mover para a esquerda).
 */
void Pipe::setSpeed(float newSpeedX) {
    speedX = newSpeedX;
}