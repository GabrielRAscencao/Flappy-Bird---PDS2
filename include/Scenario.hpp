/**
 * @file Scenario.hpp
 * @brief Scenarioheader do projeto Traveling Dragon.
 */

#ifndef SCENARIO_HPP
#define SCENARIO_HPP

#include "Bird.hpp"          // Para usar a classe Bird (o personagem do jogador)
#include "Pipe.hpp"          // Para usar a classe Pipe (os obstáculos)
#include <allegro5/allegro_font.h> // Para renderizar texto (como a pontuação)
#include <allegro5/allegro.h>      // Para funcionalidades básicas do Allegro
#include <allegro5/allegro_audio.h> // Para tocar sons (ponto, morte)
#include <vector>                  // Para usar std::vector (para armazenar os canos)
#include <cmath>                   // Para funções matemáticas, se necessário (ex: para calcular distâncias)

/**
 * @brief Gerencia todo o ambiente de jogo, incluindo o pássaro, os canos e a pontuação.
 *
 * A classe Scenario é responsável pela lógica principal do gameplay:
 * movimento de elementos, detecção de colisões, geração de canos,
 * atualização da pontuação e controle do estado de "Game Over".
 */
class Scenario {
public:
    /// @brief Espaçamento horizontal padrão entre os canos no design do jogo.
    static const float PIPE_SPACING_DESIGN;
    /// @brief Velocidade inicial dos canos no design do jogo, por frame.
    static const float INITIAL_PIPE_DESIGN_SPEED_PER_FRAME;
    /// @brief Aumento da velocidade dos canos por ponto marcado, por frame.
    static const float PIPE_SPEED_INCREASE_PER_SCORE_PER_FRAME;

    /**
     * @brief Construtor da classe Scenario.
     *
     * Inicializa o cenário do jogo com os recursos visuais (fundos, pássaro, canos),
     * a fonte para a pontuação, as dimensões da tela e os efeitos sonoros.
     *
     * @param bg Ponteiro para o bitmap de fundo do cenário.
     * @param bird_bmp Ponteiro para o bitmap (folha de sprites) do pássaro.
     * @param pipe_bmp Ponteiro para o bitmap do cano.
     * @param fontlarge Ponteiro para a fonte Allegro grande, usada para a pontuação.
     * @param screenW Largura da tela de exibição do jogo.
     * @param screenH Altura da tela de exibição do jogo.
     * @param pointSound Ponteiro para o ALLEGRO_SAMPLE do som de pontuação.
     * @param dieSound Ponteiro para o ALLEGRO_SAMPLE do som de morte do pássaro.
     */
    Scenario(ALLEGRO_BITMAP* bg, ALLEGRO_BITMAP* bird_bmp, ALLEGRO_BITMAP* pipe_bmp, ALLEGRO_FONT* fontlarge,
             int screenW, int screenH, ALLEGRO_SAMPLE* pointSound, ALLEGRO_SAMPLE* dieSound);

    /**
     * @brief Destrutor da classe Scenario.
     * Libera quaisquer recursos alocados dinamicamente (como objetos Pipe).
     */
    ~Scenario();

    /**
     * @brief Atualiza a lógica de todo o cenário a cada frame do jogo.
     *
     * Isso inclui o movimento do pássaro e dos canos, detecção de colisões,
     * atualização da pontuação e gerenciamento da geração de novos canos.
     *
     * @param deltaTime O tempo decorrido desde a última atualização (em segundos).
     */
    void update(float deltaTime);

    /**
     * @brief Altera os bitmaps usados para o fundo e os canos do cenário.
     * Usado principalmente para transições entre diferentes níveis ou fases visuais.
     * @param newBackground Ponteiro para o novo bitmap de fundo.
     * @param newPipe Ponteiro para o novo bitmap dos canos.
     */
    void changeBackgroundAndPipe(ALLEGRO_BITMAP* newBackground, ALLEGRO_BITMAP* newPipe);

    /**
     * @brief Renderiza (desenha) todos os elementos do cenário na tela.
     * Inclui o fundo, os canos, o pássaro e a pontuação.
     */
    void render();

    /**
     * @brief Reseta o estado do cenário para o início de uma nova partida.
     * Reinicializa o pássaro, limpa e gera novos canos, zera a pontuação e
     * redefine o estado de "Game Over".
     */
    void reset();

    /**
     * @brief Verifica se o jogo está no estado de "Game Over".
     * @return true se o pássaro colidiu ou saiu da tela, false caso contrário.
     */
    bool isGameOver() const;

    /**
     * @brief Retorna a pontuação atual do jogador.
     * @return O valor inteiro da pontuação.
     */
    int getScore() const;

    /**
     * @brief Retorna uma referência para o objeto Bird (o pássaro do jogador).
     * Permite que outras partes do sistema interajam diretamente com o pássaro.
     * @return Uma referência para o objeto Bird.
     */
    Bird& getBird();

    /**
     * @brief Verifica se um ponto acabou de ser marcado.
     * Usado para detectar o momento exato em que o pássaro atravessa um cano
     * e tocar um som de pontuação.
     * @return true se um ponto foi marcado no último frame, false caso contrário.
     */
    bool hasScoredPoint() const;

    /**
     * @brief Reseta a flag que indica que um ponto foi marcado.
     * Deve ser chamado após o som de pontuação ser tocado.
     */
    void resetPointFlag();

    /**
     * @brief Aumenta a velocidade de movimento dos canos em uma porcentagem específica.
     * Usado para aumentar a dificuldade do jogo com o tempo ou em novos níveis.
     * @param percent A porcentagem de aumento (ex: 0.10 para 10%).
     */
    void increaseSpeedByPercent(float percent);

    /**
     * @brief Define se os canos devem ser gerados indefinidamente.
     * Útil para o último nível, onde não há um número fixo de canos.
     * @param val Se true, gera canos sem parar; se false, respeita um limite.
     */
    void setInfinitePipes(bool val) { infinitePipes = val; }

    /**
     * @brief Define o nível atual do jogo.
     * Usado para controlar a lógica de geração de canos e transições entre níveis.
     * @param level O número do nível atual.
     */
    void setCurrentLevel(int level) { currentLevel = level; }

private:
    ALLEGRO_BITMAP* background;     ///< @brief O bitmap da imagem de fundo do cenário.
    ALLEGRO_BITMAP* birdBitmap;     ///< @brief O bitmap (folha de sprites) do pássaro.
    ALLEGRO_BITMAP* pipeBitmap;     ///< @brief O bitmap da imagem dos canos.
    ALLEGRO_FONT* fontlarge;        ///< @brief A fonte usada para exibir a pontuação.
    float SCREEN_W, SCREEN_H;       ///< @brief Largura e altura da tela de exibição do jogo.
    float scale_x, scale_y;         ///< @brief Fatores de escala aplicados aos elementos visuais para ajustar à resolução.

    /// @brief Número máximo de canos que podem existir simultaneamente na tela.
    static constexpr int MAX_PIPES = 15;

    Bird bird;                      ///< @brief O objeto do pássaro, personagem principal do jogo.
    std::vector<Pipe> pipes;        ///< @brief Vetor que armazena todos os objetos Pipe (canos) atualmente na tela.
    float pipeSpawnTimer;           ///< @brief Timer para controlar o intervalo entre a geração de novos canos.
    float pipeSpawnInterval;        ///< @brief O tempo (em segundos) entre a geração de cada novo cano.

    int score;                      ///< @brief A pontuação atual do jogador na partida.
    bool gameOver;                  ///< @brief Flag que indica se o jogo chegou ao fim (Game Over).
    float backgroundScrollOffset;   ///< @brief Deslocamento horizontal do fundo para criar o efeito de rolagem.

    float currentPipeSpeed;         ///< @brief A velocidade horizontal atual com que os canos se movem.

    ALLEGRO_SAMPLE* somPoint;       ///< @brief O sample de áudio para o som de pontuação.
    ALLEGRO_SAMPLE* somDie;         ///< @brief O sample de áudio para o som de morte do pássaro.
    bool scoredPointFlag;           ///< @brief Flag que é ativada quando um ponto é marcado, para tocar o som uma vez.

    /**
     * @brief Verifica se houve colisão entre o pássaro e um cano específico.
     * @param b O objeto Bird a ser verificado.
     * @param p O objeto Pipe a ser verificado.
     * @return true se houver colisão, false caso contrário.
     */
    bool checkCollision(const Bird& b, const Pipe& p);

    /**
     * @brief Gera um novo par de canos e o adiciona ao vetor `pipes`.
     * A posição e o tamanho do espaço são aleatórios dentro de limites.
     */
    void spawnPipe();

    /**
     * @brief Remove os canos do vetor `pipes` que já saíram completamente da tela.
     * Ajuda a gerenciar a memória e o desempenho.
     */
    void cleanUpPipes();

    int totalPipesSpawnedThisLevel; ///< @brief Contador de quantos canos foram gerados no nível atual.
    bool infinitePipes = false;     ///< @brief Se true, os canos são gerados sem um limite de "nível", apenas por tempo.

    int currentLevel = 0;           ///< @brief O nível atual do jogo, usado para ajustar a dificuldade e comportamento de geração de canos.
};

#endif // SCENARIO_HPP