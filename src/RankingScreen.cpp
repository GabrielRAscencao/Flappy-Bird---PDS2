/**
 * @file RankingScreen.cpp
 * @brief RankingScreenimplementação do projeto Traveling Dragon.
 */


#include "RankingScreen.hpp"
#include <allegro5/allegro_primitives.h> // Para desenho de formas primitivas
#include <iostream> // Para saída de avisos
#include <string> // Para manipulação de strings
#include <algorithm> // Para std::sort e std::max

/**
 * @brief Função de comparação para ordenar jogadores por pontuação decrescente.
 * Usada com `std::sort`.
 * @param a Primeiro jogador a comparar.
 * @param b Segundo jogador a comparar.
 * @return True se a pontuação de 'a' for maior que a de 'b', false caso contrário.
 */
static bool comparePlayersByScore(const Player& a, const Player& b) {
    return a.getMaiorPontuacao() > b.getMaiorPontuacao();
}

/**
 * @brief Construtor da classe RankingScreen.
 * @param f Ponteiro para a fonte a ser usada no texto.
 * @param rankingBackground Ponteiro para o bitmap de fundo da tela de ranking.
 * @param pm Ponteiro para o PlayerManager, que gerencia os dados dos jogadores.
 * @param screenW Largura da tela atual do jogo.
 * @param screenH Altura da tela atual do jogo.
 */
RankingScreen::RankingScreen(ALLEGRO_FONT* f, ALLEGRO_BITMAP* rankingBackground, PlayerManager* pm, int screenW, int screenH)
    : font(f),
      rankingBackground(rankingBackground),
      playerManager(pm),
      SCREEN_W(static_cast<float>(screenW)), // Converte para float para cálculos
      SCREEN_H(static_cast<float>(screenH)), // Converte para float para cálculos
      waitingForKeyPress(true) // Começa esperando por um input para sair
{
    // Dimensões de design para cálculo da escala
    const float DESIGN_W = 1280.0f;
    const float DESIGN_H = 720.0f;
    // Calcula os fatores de escala para ajustar o conteúdo à resolução atual
    scale_x = SCREEN_W / DESIGN_W;
    scale_y = SCREEN_H / DESIGN_H;

    // Verificações de segurança para garantir que os assets e gerenciadores foram passados
    if (!font) {
        std::cerr << "AVISO RANKINGSCREEN: Fonte nao carregada no construtor.\n";
    }
    if (!playerManager) {
        std::cerr << "AVISO RANKINGSCREEN: PlayerManager e nulo no construtor.\n";
    }
}

/**
 * @brief Atualiza o estado da tela de ranking.
 * @param deltaTime Tempo decorrido desde a última atualização (atualmente não usado).
 * Esta tela é estática, então não há lógica de atualização dinâmica aqui.
 */
void RankingScreen::update(float deltaTime) {
    // Nada pra atualizar no fundo estático, por enquanto.
    // Se tivéssemos elementos animados no ranking, seria aqui.
}

/**
 * @brief Renderiza a tela de ranking, exibindo os jogadores e suas pontuações.
 * @param currentPlayer Ponteiro para o jogador atual (pode ser nulo). Se presente,
 * a linha do jogador atual será destacada.
 * @param deltaTime Tempo decorrido desde a última atualização (não usado diretamente no render).
 */
void RankingScreen::render(Player* currentPlayer, float deltaTime) {
    // Desenha o fundo da tela de ranking
    if (rankingBackground) {
        int bg_w = al_get_bitmap_width(rankingBackground);
        int bg_h = al_get_bitmap_height(rankingBackground);
        // Calcula a escala para cobrir toda a tela, mantendo a proporção da imagem
        float bg_scale = std::max(SCREEN_W / (float)bg_w, SCREEN_H / (float)bg_h);
        al_draw_scaled_bitmap(rankingBackground, 0, 0, bg_w, bg_h, 0, 0, bg_w * bg_scale, bg_h * bg_scale, 0);
    } else {
        // Fallback: se o background for nulo, pinta a tela de preto
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    // Retorna se a fonte não estiver carregada para evitar erros de desenho
    if (!font) {
        std::cerr << "ERRO: Fonte do ranking nao carregada. (RankingScreen::render)\n";
        return;
    }

    // Desenha um painel preto semi-transparente arredondado para melhorar a legibilidade
    float panelX1 = 80 * scale_x;
    float panelY1 = 60 * scale_y;
    float panelX2 = SCREEN_W - 80 * scale_x;
    float panelY2 = SCREEN_H - 150 * scale_y;
    float cornerRadius = 10 * scale_x;

    al_draw_filled_rounded_rectangle(panelX1, panelY1, panelX2, panelY2,
                                     cornerRadius, cornerRadius, al_map_rgba(0, 0, 0, 180)); // 180 de alpha = ~70% opaco

    // Desenha o título do ranking
    al_draw_text(font, al_map_rgb(255, 255, 0), SCREEN_W / 2.0f, 90.0f * scale_y, ALLEGRO_ALIGN_CENTER, "RANKING - TOP 10");

    float y = 140.0f * scale_y; // Posição Y inicial para os cabeçalhos das colunas
    // Define as posições X para cada coluna, centralizadas ou alinhadas conforme necessário
    float colPos = SCREEN_W / 2.0f - 390.0f * scale_x;
    float colNome = SCREEN_W / 2.0f - 220.0f * scale_x;
    float colPontos = SCREEN_W / 2.0f + 110.0f * scale_x;
    float colPartidas = SCREEN_W / 2.0f + 255.0f * scale_x;

    // Desenha os cabeçalhos das colunas
    al_draw_text(font, al_map_rgb(200, 200, 200), colPos, y, 0, "POSIÇÃO");
    al_draw_text(font, al_map_rgb(200, 200, 200), colNome, y, 0, "NOME");
    al_draw_text(font, al_map_rgb(200, 200, 200), colPontos, y, 0, "PONTOS");
    al_draw_text(font, al_map_rgb(200, 200, 200), colPartidas, y, 0, "PARTIDAS");
    y += 40.0f * scale_y; // Avança a posição Y para a primeira linha de dados

    // Obtém a lista de jogadores do PlayerManager e a ordena por pontuação
    std::vector<Player> jogadores = playerManager->getJogadores();
    std::sort(jogadores.begin(), jogadores.end(), comparePlayersByScore);

    int colocacaoAtual = -1; // Variável para armazenar a colocação do jogador atual
    if (currentPlayer) {
        // Encontra a colocação do jogador atual na lista ordenada
        for (size_t i = 0; i < jogadores.size(); ++i) {
            if (jogadores[i].getApelido() == currentPlayer->getApelido()) {
                colocacaoAtual = (int)i + 1; // Colocação é o índice + 1
                break;
            }
        }
    }

    // Desenha os dados do TOP 10 jogadores
    for (size_t i = 0; i < jogadores.size() && i < 10; ++i) {
        const Player& p = jogadores[i];
        ALLEGRO_COLOR cor = al_map_rgb(255, 255, 255); // Cor padrão branca
        // Se for o jogador atual, destaca a linha com cor verde
        if (currentPlayer && p.getApelido() == currentPlayer->getApelido()) {
            cor = al_map_rgb(0, 255, 0);
        }
        // Desenha os dados de cada jogador nas colunas correspondentes
        al_draw_textf(font, cor, colPos, y, 0, "%d", (int)i + 1);
        al_draw_text(font, cor, colNome, y, 0, p.getApelido().c_str());
        al_draw_textf(font, cor, colPontos, y, 0, "%d", p.getMaiorPontuacao());
        al_draw_textf(font, cor, colPartidas, y, 0, "%d", p.getPartidas());
        y += 30.0f * scale_y; // Avança para a próxima linha
    }

    y += 20.0f * scale_y; // Espaçamento extra antes da mensagem do jogador atual

    // Mensagem sobre a colocação do jogador atual
    if (currentPlayer) {
        if (colocacaoAtual != -1) {
            std::string msg = "Sua colocação: " + std::to_string(colocacaoAtual);
            al_draw_text(font, al_map_rgb(0, 255, 0), SCREEN_W / 2.0f, y, ALLEGRO_ALIGN_CENTER, msg.c_str());
        } else {
            // Se o jogador não está no top 10 ou não tem pontos ainda
            if (currentPlayer->getPartidas() == 0) {
                al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_W / 2.0f, y, ALLEGRO_ALIGN_CENTER, "Você ainda não jogou!");
            } else {
                al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_W / 2.0f, y, ALLEGRO_ALIGN_CENTER, "Sem registro no ranking (score baixo).");
            }
        }
    } else {
        // Mensagem caso nenhum jogador atual seja passado (ex: acessando o ranking direto do menu sem jogar)
        al_draw_text(font, al_map_rgb(255, 0, 0), SCREEN_W / 2.0f, y, ALLEGRO_ALIGN_CENTER, "Você ainda não jogou!");
    }

    // Mensagem para o usuário voltar ao menu
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2.0f, SCREEN_H - 80.0f * scale_y, ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para voltar");
}

/**
 * @brief Lida com eventos de entrada na tela de ranking.
 * @param ev O evento Allegro a ser processado.
 * @return 1 se uma tecla for pressionada (sinal para voltar ao menu), 0 caso contrário.
 */
int RankingScreen::handleEvent(ALLEGRO_EVENT ev) {
    if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        waitingForKeyPress = false; // Sinaliza que uma tecla foi pressionada
        return 1; // Retorna 1 para indicar que a tela deve ser fechada/navegada
    }
    return 0; // Nenhum evento relevante processado
}

/**
 * @brief Reseta o estado da tela de ranking.
 * Útil para reexibir a tela.
 */
void RankingScreen::resetState() {
    waitingForKeyPress = true; // Reinicia o estado de espera por uma tecla
}