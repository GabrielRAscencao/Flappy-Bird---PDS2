/**
 * @file Bird.cpp
 * @brief Birdimplementação do projeto Traveling Dragon.
 */


#include "Bird.hpp"
#include <allegro5/allegro_primitives.h> // Necessário para desenho de formas primitivas (fallback)
#define _USE_MATH_DEFINES // Define M_PI para algumas libs C++
#include <cmath> // Para funções matemáticas como clamp
#include <algorithm> // Para std::clamp
#include <iostream> // Para saída de erros/avisos

// Garante que M_PI esteja definido para cálculos precisos
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/// @brief Rotação máxima para cima (em radianos) quando o pássaro está subindo.
const float Bird::MAX_UP_ROTATION = -30.0f * (M_PI / 180.0f);
/// @brief Rotação máxima para baixo (em radianos) quando o pássaro está caindo.
const float Bird::MAX_DOWN_ROTATION = 70.0f * (M_PI / 180.0f);
/// @brief Proporção inicial da altura da tela para a posição Y do pássaro.
const float Bird::INITIAL_Y_RATIO = 0.4f;
/// @brief Proporção do deslocamento horizontal do centro da tela para o pássaro.
const float Bird::CENTER_X_OFFSET_RATIO = 0.0f;

/**
 * @brief Construtor da classe Bird.
 * @param sheet Ponteiro para o bitmap da folha de sprites do pássaro.
 * @param screenW Largura da tela do jogo.
 * @param screenH Altura da tela do jogo.
 * @param sX Fator de escala X para o pássaro.
 * @param sY Fator de escala Y para o pássaro.
 */
Bird::Bird(ALLEGRO_BITMAP* sheet, float screenW, float screenH, float sX, float sY)
    : GameObject(0, // X inicial será ajustado no construtor
                     (screenH * Bird::INITIAL_Y_RATIO), // Y inicial baseado na proporção da tela
                     (sheet ? (al_get_bitmap_width(sheet) / 3.0f) * sX * 1.2f : 34.0f * sX * 1.2f), // Largura do pássaro
                     (sheet ? al_get_bitmap_height(sheet) * sY * 1.2f : 24.0f * sY * 1.2f)), // Altura do pássaro
      spriteSheet(sheet),
      screenHeight(screenH),
      screenWidth(screenW),
      scaleX(sX),
      scaleY(sY),
      velocityY(0),
      gravity(0.2f * sY), // Gravidade ajustada pela escala Y para consistência
      flapForce(-6.0f * sY), // Força do flap ajustada pela escala Y
      currentFrame(0),
      frameTimer(0.0),
      animationSpeed(8.0),
      rotationAngle(0.0f),
      targetRotationAngle(0.0f),
      rotationSpeed(0.25f),
      frameWidth(1),
      frameHeight(1),
      rotacionar(true)  // O pássaro começa com a rotação ativada por padrão
{
    // Verifica se a folha de sprites foi carregada corretamente
    if (!spriteSheet) {
        std::cerr << "ERRO CRITICO NO BIRD: spriteSheet e NULO! AssetManager falhou ou nao foi passado.\n";
        // Se a spriteSheet for nula, inicializa os frames como nulos para evitar acessos inválidos
        for (int i = 0; i < 3; ++i) { frames[i] = nullptr; }
    } else {
        // Calcula a largura e altura de cada frame individual na folha de sprites
        frameWidth = al_get_bitmap_width(spriteSheet) / 3;
        frameHeight = al_get_bitmap_height(spriteSheet);

        // Validação adicional para dimensões dos frames
        if (frameWidth <= 0 || frameHeight <= 0 || al_get_bitmap_width(spriteSheet) < (frameWidth * 3)) {
            std::cerr << "AVISO BIRD: Dimensoes invalidas para sub-bitmaps. Redefinindo para 1x1 e frames nulos.\n";
            frameWidth = 1;
            frameHeight = 1;
            for (int i = 0; i < 3; ++i) { frames[i] = nullptr; }
        } else {
            // Cria os sub-bitmaps para cada frame de animação
            for (int i = 0; i < 3; ++i) {
                frames[i] = al_create_sub_bitmap(spriteSheet, i * frameWidth, 0, frameWidth, frameHeight);
                if (!frames[i]) {
                    std::cerr << "ERRO BIRD: Nao foi possivel criar sub-bitmap para o frame " << i << ". Verifique a area do sub-bitmap e o spriteSheet.\n";
                }
            }
        }
    }

    // Centraliza o pássaro horizontalmente na tela, aplicando um offset se necessário
    this->x = (this->screenWidth / 2.0f) - (this->width / 2.0f) + (this->screenWidth * Bird::CENTER_X_OFFSET_RATIO);
}

/**
 * @brief Destrutor da classe Bird.
 * Libera a memória alocada para os sub-bitmaps dos frames.
 */
Bird::~Bird() {
    for (int i = 0; i < 3; ++i) {
        if (frames[i]) {
            al_destroy_bitmap(frames[i]);
            frames[i] = nullptr; // Define o ponteiro como nulo após a destruição
        }
    }
}

/**
 * @brief Reseta o estado do pássaro para as configurações iniciais do jogo.
 * Usado para reiniciar a fase.
 */
void Bird::reset() {
    // Reposiciona o pássaro no centro X e na altura inicial Y
    this->x = (this->screenWidth / 2.0f) - (this->width / 2.0f) + (this->screenWidth * Bird::CENTER_X_OFFSET_RATIO);
    this->y = (this->screenHeight * Bird::INITIAL_Y_RATIO);
    this->velocityY = 0; // Zera a velocidade vertical
    this->currentFrame = 0; // Volta para o primeiro frame de animação
    this->frameTimer = 0.0; // Reseta o temporizador da animação
    this->rotationAngle = 0.0f; // Zera o ângulo de rotação
    this->targetRotationAngle = 0.0f; // Reseta o ângulo alvo de rotação
}

/**
 * @brief Atualiza o estado do pássaro a cada frame do jogo.
 * @param deltaTime O tempo decorrido desde a última atualização (não usado diretamente, mas bom para consistência).
 */
void Bird::update(float deltaTime) {
    // Aplica a gravidade à velocidade vertical do pássaro
    this->velocityY += this->gravity;
    // Atualiza a posição Y do pássaro com base na velocidade vertical
    this->y += this->velocityY;

    // Limita a velocidade máxima de queda para evitar que o pássaro caia muito rápido
    const float MAX_FALL_VELOCITY = 10.0f * this->scaleY;
    if (this->velocityY > MAX_FALL_VELOCITY) {
        this->velocityY = MAX_FALL_VELOCITY;
    }

    // Atualiza o temporizador da animação e muda o frame quando atinge o limite
    this->frameTimer++;
    if (this->frameTimer >= this->animationSpeed) {
        this->currentFrame = (this->currentFrame + 1) % 3; // Cicla entre os 3 frames
        this->frameTimer = 0.0; // Reseta o temporizador
    }

    // Gerencia a rotação do pássaro com base na sua velocidade vertical, se a rotação estiver ativada
    if (rotacionar) {
        if (this->velocityY < 0) {
            // Se o pássaro está subindo, o alvo de rotação é para cima
            this->targetRotationAngle = Bird::MAX_UP_ROTATION;
        } else {
            // Se o pássaro está caindo ou parado, o alvo de rotação é para baixo
            this->targetRotationAngle = Bird::MAX_DOWN_ROTATION;
        }

        // Interpola suavemente o ângulo de rotação atual em direção ao ângulo alvo
        this->rotationAngle += (this->targetRotationAngle - this->rotationAngle) * this->rotationSpeed;
        // Garante que o ângulo de rotação permaneça dentro dos limites definidos
        this->rotationAngle = std::clamp(this->rotationAngle, Bird::MAX_UP_ROTATION, Bird::MAX_DOWN_ROTATION);
    } else {
        // Se a rotação está desativada, mantém o ângulo em 0
        this->rotationAngle = 0.0f;
    }
}

/**
 * @brief Renderiza o pássaro na tela.
 */
void Bird::render() {
    ALLEGRO_BITMAP* currentBitmap = this->frames[this->currentFrame];
    if (currentBitmap) {
        // Desenha o bitmap atual do pássaro, aplicando escala, rotação e posicionamento
        al_draw_scaled_rotated_bitmap(
            currentBitmap,
            al_get_bitmap_width(currentBitmap) / 2.0f, // Ponto de rotação X (centro do bitmap)
            al_get_bitmap_height(currentBitmap) / 2.0f, // Ponto de rotação Y (centro do bitmap)
            this->x + this->width / 2.0f, // Posição X central do objeto na tela
            this->y + this->height / 2.0f, // Posição Y central do objeto na tela
            this->width / al_get_bitmap_width(currentBitmap), // Escala X
            this->height / al_get_bitmap_height(currentBitmap), // Escala Y
            rotacionar ? this->rotationAngle : 0.0f, // Ângulo de rotação (se a rotação estiver ativada)
            0 // Flags (nenhuma neste caso)
        );
    } else {
        // Fallback: se o bitmap não estiver disponível, desenha um retângulo magenta
        al_draw_filled_rectangle(this->x, this->y, this->x + this->width, this->y + this->height, al_map_rgb(255, 0, 255));
    }
}

/**
 * @brief Aplica uma força para cima no pássaro, simulando o "flap" (voo).
 */
void Bird::flap() {
    this->velocityY = this->flapForce; // Define a velocidade vertical para a força do flap
    this->frameTimer = 0.0; // Reinicia o temporizador da animação
    this->currentFrame = 0; // Volta para o primeiro frame da animação de flap
}

/**
 * @brief Define se o pássaro deve rotacionar com base na sua velocidade.
 * @param on True para ativar a rotação, false para desativar.
 */
void Bird::setRotacionar(bool on) {
    this->rotacionar = on;
}

/**
 * @brief Verifica se a rotação do pássaro está ativada.
 * @return True se a rotação estiver ativada, false caso contrário.
 */
bool Bird::isRotacionando() const {
    return this->rotacionar;
}