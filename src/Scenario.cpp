/**
 * @file Scenario.cpp
 * @brief Scenarioimplementação do projeto Traveling Dragon.
 */


#include "Scenario.hpp"
#include <allegro5/allegro_primitives.h> // Para desenhar formas primitivas (fallback)
#include <random> // Para geração de números aleatórios
#include <algorithm> // Para std::remove_if e std::max/min
#include <iostream> // Para saída de avisos
#include <cmath> // Para std::abs

// Constantes de design para escala e velocidade dos canos
/// @brief Espaçamento horizontal padrão entre os canos, baseado no design.
const float Scenario::PIPE_SPACING_DESIGN = 420.0f;
/// @brief Velocidade horizontal inicial dos canos por frame, baseada no design.
const float Scenario::INITIAL_PIPE_DESIGN_SPEED_PER_FRAME = -150.0f;
/// @brief Aumento de velocidade dos canos por ponto, baseado no design.
const float Scenario::PIPE_SPEED_INCREASE_PER_SCORE_PER_FRAME = -0.05f;

// Inicializa o gerador de números aleatórios de forma estática, para que seja único.
static std::mt19937 rng(std::random_device{}());

/**
 * @brief Construtor da classe Scenario.
 *
 * Inicializa todos os elementos do cenário, como o pássaro, os canos, a pontuação,
 * e configura os parâmetros de jogo como velocidade e sons.
 *
 * @param bg Ponteiro para o bitmap de fundo do cenário.
 * @param bird_bmp Ponteiro para o bitmap da folha de sprites do pássaro.
 * @param pipe_bmp Ponteiro para o bitmap da imagem do cano.
 * @param fontlarge Ponteiro para a fonte grande para exibir a pontuação.
 * @param screenW Largura da tela do jogo.
 * @param screenH Altura da tela do jogo.
 * @param pointSound Ponteiro para o som de ponto (quando o pássaro passa por um cano).
 * @param dieSound Ponteiro para o som de morte (colisão).
 */
Scenario::Scenario(ALLEGRO_BITMAP* bg, ALLEGRO_BITMAP* bird_bmp, ALLEGRO_BITMAP* pipe_bmp, ALLEGRO_FONT* fontlarge, 
                   int screenW, int screenH, ALLEGRO_SAMPLE* pointSound, ALLEGRO_SAMPLE* dieSound)
    : background(bg),
      birdBitmap(bird_bmp),
      pipeBitmap(pipe_bmp),
      fontlarge(fontlarge),
      SCREEN_W(static_cast<float>(screenW)), // Converte para float para cálculos precisos
      SCREEN_H(static_cast<float>(screenH)), // Converte para float para cálculos precisos
      // Inicializa o pássaro com a imagem e dimensões da tela, aplicando escala
      bird(bird_bmp, (float)screenW, (float)screenH, (float)screenW / 1280.0f, (float)screenH / 720.0f),
      pipeSpawnTimer(0.0f),
      pipeSpawnInterval(0.0f),
      score(0),
      gameOver(false),
      backgroundScrollOffset(0.0f),
      currentPipeSpeed(INITIAL_PIPE_DESIGN_SPEED_PER_FRAME), // Inicializa com a velocidade de design
      somPoint(pointSound), // Atribui o som de ponto
      somDie(dieSound),     // Atribui o som de morte
      scoredPointFlag(false), // Flag para indicar se um ponto foi marcado neste frame
      totalPipesSpawnedThisLevel(0) // Contador de canos gerados na fase atual
{
    // Define as dimensões de design para calcular os fatores de escala.
    const float DESIGN_W = 1280.0f;
    const float DESIGN_H = 720.0f;

    scale_x = SCREEN_W / DESIGN_W; // Calcula o fator de escala horizontal
    scale_y = SCREEN_H / DESIGN_H; // Calcula o fator de escala vertical

    bird.reset();           // Reseta a posição e estado inicial do pássaro
    bird.setRotacionar(false); // Desativa a rotação do pássaro no início do jogo (tela inicial)

    // Ajusta a velocidade dos canos e o intervalo de spawn com base na escala X
    currentPipeSpeed = INITIAL_PIPE_DESIGN_SPEED_PER_FRAME * scale_x;
    pipeSpawnInterval = (PIPE_SPACING_DESIGN * scale_x) / std::abs(currentPipeSpeed);

    // Altura da lacuna entre os canos, ajustada pela escala Y
    float gap_h = 250.0f * scale_y;
    // Posição X inicial do primeiro cano, começando mais à direita da tela
    float startX = SCREEN_W * 0.75f;

    // Gera o número inicial de canos para preencher a tela no começo
    for (int i = 0; i < MAX_PIPES; ++i) {
        // Define limites para a posição Y da lacuna dos canos (para evitar que saiam da tela)
        float min_gap_y = 100.0f * scale_y;
        float max_gap_y = SCREEN_H - 100.0f * scale_y;
        std::uniform_real_distribution<float> dist_gap_y(min_gap_y, max_gap_y);
        float gap_y = dist_gap_y(rng); // Gera um Y aleatório para a lacuna

        // Calcula a posição X de cada cano, espaçando-os uniformemente
        float pipe_x = startX + i * (PIPE_SPACING_DESIGN * scale_x);
        // Adiciona um novo objeto Pipe ao vetor de canos
        pipes.emplace_back(pipe_x, gap_y, gap_h, pipeBitmap, SCREEN_H, scale_x, scale_y);
        ++totalPipesSpawnedThisLevel; // Incrementa o contador de canos gerados
    }

    pipeSpawnTimer = pipeSpawnInterval; // Inicializa o timer para o próximo spawn de cano
}

/**
 * @brief Destrutor da classe Scenario.
 * Não precisa fazer nada especial aqui, pois os recursos ALLEGRO_BITMAP* e ALLEGRO_SAMPLE*
 * são gerenciados externamente (provavelmente por um AssetManager).
 */
Scenario::~Scenario() {}

/**
 * @brief Reseta o estado do cenário para iniciar uma nova partida.
 * Limpa os canos existentes, reseta o pássaro, a pontuação e os timers.
 */
void Scenario::reset() {
    bird.reset(); // Reseta o pássaro para a posição inicial
    pipes.clear(); // Remove todos os canos existentes
    gameOver = false; // Reseta o estado de game over
    backgroundScrollOffset = 0.0f; // Reseta o offset de rolagem do fundo
    // currentPipeSpeed = INITIAL_PIPE_DESIGN_SPEED_PER_FRAME * scale_x; // Comentado, talvez para manter a velocidade atual
    pipeSpawnInterval = (PIPE_SPACING_DESIGN * scale_x) / std::abs(currentPipeSpeed); // Recalcula o intervalo de spawn
    totalPipesSpawnedThisLevel = 0; // Reseta o contador de canos gerados

    float gap_h = 250.0f * scale_y; // Altura da lacuna entre os canos, ajustada pela escala
    float startX = SCREEN_W * 0.75f; // Posição X inicial para o primeiro cano

    // Gera um novo conjunto inicial de canos para a nova partida
    for (int i = 0; i < MAX_PIPES; ++i) {
        // Limites para a posição Y da lacuna, um pouco mais restritivos para o reset
        float min_gap_y = 200.0f * scale_y;
        float max_gap_y = SCREEN_H - 200.0f * scale_y;
        std::uniform_real_distribution<float> dist_gap_y(min_gap_y, max_gap_y);
        float gap_y = dist_gap_y(rng); // Gera um Y aleatório para a lacuna

        float pipe_x = startX + i * (PIPE_SPACING_DESIGN * scale_x);
        pipes.emplace_back(pipe_x, gap_y, gap_h, pipeBitmap, SCREEN_H, scale_x, scale_y);
        ++totalPipesSpawnedThisLevel; // Incrementa o contador
    }

    pipeSpawnTimer = pipeSpawnInterval; // Reinicia o timer para o próximo spawn
    scoredPointFlag = false; // Reseta a flag de ponto marcado
}

/**
 * @brief Verifica se o jogo terminou.
 * @return True se o jogo está em Game Over, false caso contrário.
 */
bool Scenario::isGameOver() const {
    return gameOver;
}

/**
 * @brief Retorna a pontuação atual do jogador.
 * @return A pontuação.
 */
int Scenario::getScore() const {
    return score;
}

/**
 * @brief Retorna uma referência para o objeto Bird.
 * Permite que outras partes do código acessem e manipulem o pássaro.
 * @return Uma referência para o objeto Bird.
 */
Bird& Scenario::getBird() {
    return bird;
}

/**
 * @brief Verifica se um ponto foi marcado no frame atual.
 * @return True se um ponto foi marcado, false caso contrário.
 */
bool Scenario::hasScoredPoint() const {
    return scoredPointFlag;
}

/**
 * @brief Reseta a flag de ponto marcado.
 * Deve ser chamado após processar um ponto para que a flag possa ser definida novamente no próximo ponto.
 */
void Scenario::resetPointFlag() {
    scoredPointFlag = false;
}

/**
 * @brief Verifica a colisão entre o pássaro e um cano.
 * Aplica margens para uma detecção de colisão mais justa (menos sensível nas bordas do pássaro).
 *
 * @param b O objeto Bird a ser verificado.
 * @param p O objeto Pipe a ser verificado.
 * @return True se houver colisão, false caso contrário.
 */
bool Scenario::checkCollision(const Bird& b, const Pipe& p) {
    // Reduz o "hitbox" do pássaro para tornar a colisão mais justa
    float marginLeftRight = b.getWidth() * 0.2f;   // 20% nas laterais
    float marginTop = b.getHeight() * 0.35f;   // Reduz mais o topo (35%)
    float marginBottom = b.getHeight() * 0.13f;  // Só 13% embaixo (mais justa)

    // Coordenadas do "hitbox" ajustado do pássaro
    float birdX1 = b.getX() + marginLeftRight;
    float birdY1 = b.getY() + marginTop;
    float birdX2 = b.getX() + b.getWidth() - marginLeftRight;
    float birdY2 = b.getY() + b.getHeight() - marginBottom;

    // Coordenadas dos canos
    float pipeX1 = p.getX();
    float pipeX2 = p.getX() + p.getWidth();
    float pipeTopY2 = p.getTopPipeBottomY();    // Fundo do cano de cima (topo da lacuna)
    float pipeBottomY1 = p.getBottomPipeTopY(); // Topo do cano de baixo (base da lacuna)

    // Verifica colisão com o cano de cima
    bool collidedWithTop = birdX1 < pipeX2 && birdX2 > pipeX1 && birdY1 < pipeTopY2;
    // Verifica colisão com o cano de baixo
    bool collidedWithBottom = birdX1 < pipeX2 && birdX2 > pipeX1 && birdY2 > pipeBottomY1;

    // Retorna verdadeiro se colidiu com qualquer um dos canos
    return collidedWithTop || collidedWithBottom;
}

/**
 * @brief Gera um novo par de canos e o adiciona à lista.
 * Se `infinitePipes` for falso, limita o número total de canos gerados.
 */
void Scenario::spawnPipe() {
    // Se o modo de canos infinitos não estiver ativo e o limite de canos foi atingido, não gera mais.
    if (!infinitePipes && totalPipesSpawnedThisLevel >= MAX_PIPES) return;

    // Define os limites superior e inferior para a posição Y da lacuna
    float min_gap_y = 200.0f * scale_y;
    float max_gap_y = SCREEN_H - 200.0f * scale_y;
    std::uniform_real_distribution<float> dist_gap_y(min_gap_y, max_gap_y);
    float gap_y = dist_gap_y(rng); // Gera uma posição Y aleatória para o centro da lacuna

    // Aplica uma variação aleatória extra (ruído) para tornar o posicionamento menos previsível
    std::uniform_real_distribution<float> random_offset(-80.0f * scale_y, 80.0f * scale_y);
    gap_y += random_offset(rng);

    // Garante que o `gap_y` permaneça dentro dos limites válidos após adicionar o ruído
    gap_y = std::max(min_gap_y, std::min(gap_y, max_gap_y));

    float gap_h = 250.0f * scale_y; // Altura da lacuna (fixa)

    float pipe_x;
    if (!pipes.empty()) {
        // Se já existem canos, o novo cano é gerado a partir do último, espaçado corretamente
        const Pipe& lastPipe = pipes.back();
        pipe_x = lastPipe.getX() + (PIPE_SPACING_DESIGN * scale_x);
    } else {
        // Se não há canos, o primeiro cano começa na borda direita da tela
        pipe_x = SCREEN_W;
    }

    // Adiciona o novo cano à lista
    pipes.emplace_back(pipe_x, gap_y, gap_h, pipeBitmap, SCREEN_H, scale_x, scale_y);
    ++totalPipesSpawnedThisLevel; // Incrementa o contador de canos gerados
}

/**
 * @brief Remove os canos que saíram completamente da tela à esquerda.
 * Isso ajuda a manter a lista de canos pequena e otimiza o desempenho.
 */
void Scenario::cleanUpPipes() {
    // Usa `std::remove_if` para mover os canos "velhos" para o final do vetor
    pipes.erase(std::remove_if(pipes.begin(), pipes.end(),
                                // Lambda que retorna true se o cano está completamente fora da tela à esquerda
                                [](const Pipe& p) { return p.getX() + p.getWidth() < 0; }),
                pipes.end()); // Remove os elementos do final do vetor
}

/**
 * @brief Atualiza o estado de todos os elementos do cenário a cada frame do jogo.
 * Gerencia o movimento do pássaro, canos, colisões, pontuação e rolagem do fundo.
 * @param deltaTime O tempo decorrido desde a última atualização, para movimento baseado em tempo.
 */
void Scenario::update(float deltaTime) {
    if (gameOver) return; // Se o jogo acabou, não atualiza mais nada.

    float marginTop = bird.getHeight() * 0.35f; // Margem superior do pássaro (para colisão com o teto)

    bird.update(deltaTime); // Atualiza o estado do pássaro

    // Recalcula o intervalo de spawn dos canos e decremente o timer
    pipeSpawnInterval = (PIPE_SPACING_DESIGN * scale_x) / std::abs(currentPipeSpeed);
    pipeSpawnTimer--;

    // Se o timer de spawn zerou, gera um novo cano e reinicia o timer
    if (pipeSpawnTimer <= 0.0f) {
        spawnPipe();
        pipeSpawnTimer = pipeSpawnInterval;
    }

    // Itera sobre todos os canos para atualizar seu estado e verificar colisões/pontuação
    for (auto& p : pipes) {
        p.setSpeed(currentPipeSpeed); // Define a velocidade do cano
        p.update(deltaTime); // Atualiza o cano (movimenta-o)

        // Verifica se o pássaro passou pelo cano e ainda não pontuou com ele
        if (!p.hasScored() && bird.getX() > p.getX() + p.getWidth() / 2) {
            score++; // Incrementa a pontuação
            p.setScored(true); // Marca o cano como pontuado
            scoredPointFlag = true; // Ativa a flag para indicar que um ponto foi feito neste frame
            if (somPoint) al_play_sample(somPoint, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); // Toca o som de ponto
        }

        // Verifica colisão entre o pássaro e o cano
        if (checkCollision(bird, p)) {
            gameOver = true; // Define o jogo como Game Over
            if (somDie) al_play_sample(somDie, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); // Toca o som de morte
            return; // Sai da função update, pois o jogo acabou
        }
    }

    // Verifica colisão do pássaro com o chão ou o teto
    if (bird.getY() + bird.getHeight() >= SCREEN_H || bird.getY() + marginTop <= 0) {
        gameOver = true; // Define o jogo como Game Over
        if (somDie) al_play_sample(somDie, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); // Toca o som de morte
        return; // Sai da função update
    }

    cleanUpPipes(); // Remove os canos que saíram da tela

    // Atualiza o offset de rolagem do background
    if (background != nullptr) {
        int bg_w = al_get_bitmap_width(background);
        int bg_h = al_get_bitmap_height(background);

        if (bg_w <= 0) bg_w = 1; // Prevenção de divisão por zero
        if (bg_h <= 0) bg_h = 1;

        // Calcula a escala para preencher a tela, mantendo a proporção do fundo
        float bg_scale_to_fill = std::max(SCREEN_W / (float)bg_w, SCREEN_H / (float)bg_h);
        float final_bg_width = bg_w * bg_scale_to_fill; // Largura final do fundo

        const float BACKGROUND_SCROLL_FIXED_SPEED_PER_FRAME = -1.0f; // Velocidade de rolagem do fundo (fixa)
        backgroundScrollOffset += BACKGROUND_SCROLL_FIXED_SPEED_PER_FRAME; // Move o fundo

        // Se o fundo rolou completamente para fora da tela, reseta o offset para criar um loop contínuo
        while (backgroundScrollOffset <= -final_bg_width) {
            backgroundScrollOffset += final_bg_width;
        }
    }
}

/**
 * @brief Renderiza todos os elementos do cenário na tela.
 * Desenha o fundo, os canos e o pássaro, além da pontuação e mensagens de Game Over.
 */
void Scenario::render() {
    // Desenha o fundo do cenário
    if (background) {
        int bg_w = al_get_bitmap_width(background);
        int bg_h = al_get_bitmap_height(background);

        if (bg_w <= 0) bg_w = 1; // Prevenção de divisão por zero
        if (bg_h <= 0) bg_h = 1;

        // Calcula a escala e dimensões finais do fundo
        float bg_scale_to_fill = std::max(SCREEN_W / (float)bg_w, SCREEN_H / (float)bg_h);
        float final_bg_width = bg_w * bg_scale_to_fill;
        float final_bg_height = bg_h * bg_scale_to_fill;

        // Desenha duas cópias do fundo para criar um efeito de rolagem contínuo
        al_draw_scaled_bitmap(background, 0, 0, bg_w, bg_h,
                              backgroundScrollOffset, 0, final_bg_width, final_bg_height, 0);
        al_draw_scaled_bitmap(background, 0, 0, bg_w, bg_h,
                              backgroundScrollOffset + final_bg_width, 0, final_bg_width, final_bg_height, 0);
    } else {
        // Fallback: se o background for nulo, pinta a tela de preto
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    // Renderiza todos os canos
    for (auto& p : pipes) {
        p.render();
    }

    bird.render(); // Renderiza o pássaro

    // Exibe a pontuação na tela
    if (fontlarge) {
        std::string pontuacao = "PONTOS: " + std::to_string(score);
        float x = SCREEN_W / 2;
        float y = 50 * scale_y;

        ALLEGRO_COLOR corContorno = al_map_rgb(0, 0, 0); // Cor para o contorno do texto (sombra)
        ALLEGRO_COLOR corTexto = al_map_rgb(255, 255, 255); // Cor principal do texto

        // Desenha o contorno do texto para dar mais destaque (efeito de sombra)
        for (int dx = -3; dx <= 3; ++dx) {
            for (int dy = -3; dy <= 3; ++dy) {
                if (dx == 0 && dy == 0) continue; // Pula o centro
                al_draw_text(fontlarge, corContorno, x + dx, y + dy, ALLEGRO_ALIGN_CENTER, pontuacao.c_str());
            }
        }

        // Desenha o texto principal da pontuação (branco centralizado)
        al_draw_text(fontlarge, corTexto, x, y, ALLEGRO_ALIGN_CENTER, pontuacao.c_str());
    }

    // Se o jogo acabou, exibe a mensagem de Game Over
    if (gameOver && fontlarge) {
        al_draw_text(fontlarge, al_map_rgb(255, 0, 0), SCREEN_W / 2, SCREEN_H / 2 - 20 * scale_y, ALLEGRO_ALIGN_CENTER, "GAME OVER");
        al_draw_text(fontlarge, al_map_rgb(255, 255, 255), SCREEN_W / 2, SCREEN_H / 2 + 20 * scale_y, ALLEGRO_ALIGN_CENTER, "Pressione ESC para voltar ao menu");
    }
}

/**
 * @brief Altera os bitmaps de fundo e cano do cenário e reseta o jogo.
 * @param newBackground Novo bitmap para o fundo.
 * @param newPipe Novo bitmap para os canos.
 */
void Scenario::changeBackgroundAndPipe(ALLEGRO_BITMAP* newBackground, ALLEGRO_BITMAP* newPipe) {
    background = newBackground; // Atualiza o bitmap de fundo
    pipeBitmap = newPipe;     // Atualiza o bitmap do cano
    reset(); // Reseta o cenário para aplicar as novas texturas
}

/**
 * @brief Aumenta a velocidade dos canos em uma porcentagem.
 * Recalcula o intervalo de spawn dos canos para manter a consistência.
 * @param percent A porcentagem de aumento (ex: 10 para aumentar em 10%).
 */
void Scenario::increaseSpeedByPercent(float percent) {
    float factor = 1.0f + (percent / 100.0f); // Calcula o fator de aumento
    currentPipeSpeed *= factor; // Aplica o aumento na velocidade atual
    // Recalcula o intervalo de spawn para manter a distância entre os canos consistente
    pipeSpawnInterval = (PIPE_SPACING_DESIGN * scale_x) / std::abs(currentPipeSpeed);
}