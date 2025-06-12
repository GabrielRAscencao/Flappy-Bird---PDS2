#include "GameEngine.hpp"
#include "Menu.hpp"
#include "GameOverScreen.hpp"
#include "Scenario.hpp"
#include "PlayerManager.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <algorithm> // Para std::sort no ranking

GameEngine::GameEngine(int w, int h)
    : screenWidth(w), screenHeight(h), scenario(nullptr),
      lastScore(0), lastRecordPessoal(0), lastRecordGeral(0),
      lastBateuRecordePessoal(false), lastBateuRecordeGeral(false) {
    playerManager = new PlayerManager("data/players.txt");
    playerManager->carregar();

    scaleX = (float)screenWidth / 800.0f; 
    scaleY = (float)screenHeight / 600.0f;
}

GameEngine::~GameEngine() {
    delete scenario;
    delete playerManager;
}

bool compararPontuacao(const Player& a, const Player& b) {
    return a.getMaiorPontuacao() > b.getMaiorPontuacao();
}

// Funcao exibirRanking (otimizada para reuso de recursos e fluidez)
// Agora recebe os bitmaps e a fonte já carregados
void exibirRanking(PlayerManager* pm, const std::string& jogadorAtual, int screenWidth, int screenHeight, ALLEGRO_BITMAP* bgRanking, ALLEGRO_FONT* fontRanking) {
    if (!fontRanking || !bgRanking) {
        std::cerr << "Erro: Recursos para ranking nao carregados. (internamente)\n";
        return;
    }

    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60); // 60 FPS
    
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_register_event_source(fila, al_get_mouse_event_source()); 
    al_start_timer(timer);

    std::vector<Player> jogadores = pm->getJogadores();
    std::sort(jogadores.begin(), jogadores.end(), compararPontuacao);

    float bgOffsetX = 0.0f;
    bool esperandoTecla = true;
    double lastTime = al_get_time();
    bool redraw = false; // Flag para controle de redesenho

    int posColX = screenWidth / 2 - 320;
    int nomeColX = screenWidth / 2 - 140;
    int scoreColX = screenWidth / 2 + 140;
    int partColX = screenWidth / 2 + 280;

    // --- CORREÇÃO: Declarando bgWidth e bgHeight aqui para escopo correto ---
    int bgWidth = al_get_bitmap_width(bgRanking);
    int bgHeight = al_get_bitmap_height(bgRanking);
    // --- FIM DA CORREÇÃO ---

    while (esperandoTecla) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev); // Espera por qualquer evento

        double currentTime = al_get_time();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime; // Atualiza o lastTime imediatamente

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            esperandoTecla = false; // Sai do ranking ao pressionar qualquer tecla ou clique
        }

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            redraw = true; // Sinaliza que é hora de redesenhar
            // Atualiza o offset do background APENAS no tick do timer para suavidade
            bgOffsetX -= 50.0f * deltaTime; // Movimento suave para a esquerda
            // Usando bgWidth já declarado
            if (bgOffsetX <= -bgWidth) {
                bgOffsetX += bgWidth;
            }
        }
        
        // --- Renderiza o ranking APENAS se 'redraw' for true e a fila de eventos estiver vazia ---
        if (redraw && al_is_event_queue_empty(fila)) {
            al_clear_to_color(al_map_rgb(255, 255, 255)); // Limpa a tela
            
            // Fundo animado (usando float para bgOffsetX)
            // Usando bgWidth e bgHeight já declarados
            al_draw_scaled_bitmap(bgRanking, 0, 0, bgWidth, bgHeight, bgOffsetX, 0, bgWidth, screenHeight, 0);
            al_draw_scaled_bitmap(bgRanking, 0, 0, bgWidth, bgHeight, bgOffsetX + bgWidth, 0, bgWidth, screenHeight, 0);

            // Fundo transparente para a tabela
            al_draw_filled_rounded_rectangle(80, 60, screenWidth - 80, screenHeight - 150, 10, 10, al_map_rgba(0, 0, 0, 180));

            // Título
            al_draw_text(fontRanking, al_map_rgb(255, 255, 0), screenWidth / 2, 90, ALLEGRO_ALIGN_CENTER, "RANKING - TOP 10");

            // Cabeçalhos da tabela
            int y = 140;
            al_draw_text(fontRanking, al_map_rgb(200, 200, 200), posColX, y, 0, "POS");
            al_draw_text(fontRanking, al_map_rgb(200, 200, 200), nomeColX, y, 0, "NOME");
            al_draw_text(fontRanking, al_map_rgb(200, 200, 200), scoreColX, y, 0, "PONTOS");
            al_draw_text(fontRanking, al_map_rgb(200, 200, 200), partColX, y, 0, "PARTIDAS");
            y += 40;

            int colocacaoAtual = -1;

            for (int i = 0; i < jogadores.size(); ++i) {
                const Player& p = jogadores[i];
                if (p.getApelido() == jogadorAtual) colocacaoAtual = i + 1;

                if (i < 10) {
                    ALLEGRO_COLOR cor = (p.getApelido() == jogadorAtual) ? al_map_rgb(0, 255, 0) : al_map_rgb(255, 255, 255);
                    al_draw_textf(fontRanking, cor, posColX, y, 0, "%2d.", i + 1);
                    al_draw_text(fontRanking, cor, nomeColX, y, 0, p.getNome().c_str());
                    al_draw_textf(fontRanking, cor, scoreColX, y, 0, "%d", p.getMaiorPontuacao());
                    al_draw_textf(fontRanking, cor, partColX, y, 0, "%d", p.getPartidas());
                    y += 30;
                }
            }

            y += 20;

            if (colocacaoAtual != -1) {
                std::string msg = "Sua colocacao: " + std::to_string(colocacaoAtual);
                al_draw_text(fontRanking, al_map_rgb(0, 255, 0), screenWidth / 2, y, ALLEGRO_ALIGN_CENTER, msg.c_str());
            } else {
                al_draw_text(fontRanking, al_map_rgb(255, 0, 0), screenWidth / 2, y, ALLEGRO_ALIGN_CENTER, "Voce ainda nao jogou!");
            }

            al_draw_text(fontRanking, al_map_rgb(255, 255, 255), screenWidth / 2, screenHeight - 80, ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para voltar");

            al_flip_display(); // Atualiza a tela
            redraw = false; // Reseta a flag de redesenho
        }
    }

    al_destroy_event_queue(fila);
    al_destroy_timer(timer);
    // Não destrua bgRanking ou fontRanking aqui, eles são passados e devem ser destruídos no main loop
}


void GameEngine::run() {
    if (!al_init()) {
        std::cerr << "Falha ao inicializar Allegro\n";
        return;
    }

    al_install_keyboard();
    al_install_mouse();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_init_primitives_addon();

    // --- Configurando display para modo janela 1280x720 ---
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE); // Apenas janela, permite redimensionar
    ALLEGRO_DISPLAY* display = al_create_display(screenWidth, screenHeight); // Usa screenWidth/Height do construtor (1280x720)
    // --- FIM DA CONFIGURAÇÃO DO DISPLAY ---

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60); // Timer para 60 FPS

    if (!display || !queue || !timer) {
        std::cerr << "Erro ao criar display, queue ou timer\n";
        return;
    }

    ALLEGRO_FONT* font = al_load_font("bin/Minecrafter.Reg.ttf", 24, 0);
    ALLEGRO_FONT* fontRanking = al_load_font("bin/ARIAL.ttf", 24, 0); // Carrega a fonte para o ranking aqui
    ALLEGRO_BITMAP* birdBmp = al_load_bitmap("bin/bird.png");
    ALLEGRO_BITMAP* pipeBmp = al_load_bitmap("bin/pipe.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("bin/background.png"); // Background para menu e gameover
    ALLEGRO_BITMAP* bgRanking = al_load_bitmap("bin/background.png"); // Background para ranking (pode ser o mesmo ou diferente)

    if (!font || !fontRanking || !birdBmp || !pipeBmp || !bg || !bgRanking) {
        std::cerr << "Erro ao carregar recursos (fontes ou bitmaps)\n";
        // Destrua recursos já carregados para evitar vazamentos antes de sair
        al_destroy_font(font);
        al_destroy_font(fontRanking);
        al_destroy_bitmap(birdBmp);
        al_destroy_bitmap(pipeBmp);
        al_destroy_bitmap(bg);
        al_destroy_bitmap(bgRanking);
        al_destroy_timer(timer);
        al_destroy_event_queue(queue);
        al_destroy_display(display);
        return;
    }

    Menu menu(font, bg, screenWidth, screenHeight);
    GameOverScreen gameover(font, bg);
    scenario = new Scenario(bg, birdBmp, pipeBmp, font, screenWidth, screenHeight, scaleX, scaleY);

    EstadoJogo estado = MENU;
    Player* jogador = nullptr; // Ponteiro para o jogador atual, gerenciado por PlayerManager

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    al_start_timer(timer);

    bool rodando = true;
    double lastTime = al_get_time();
    bool redraw = false; // Flag para controlar o redesenho

    while (rodando) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev); // Espera por um evento

        // --- Processamento de Eventos ---
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = false;
        }
        
        // Calcula o deltaTime AQUI para ser usado nas atualizações de estado
        double currentTime = al_get_time();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            redraw = true; // Um tick do timer significa que é hora de atualizar e desenhar um novo frame

            // Atualiza o estado do jogo baseado no timer
            switch (estado) {
                case MENU:
                    menu.update(deltaTime); // Update do menu (animacao de background, cursor)
                    break;
                case JOGANDO:
                    scenario->update(deltaTime);
                    // Verifica condições de Game Over após a atualização
                    if (scenario->getBird().getY() + scenario->getBird().getHeight() / 2 >= screenHeight) {
                        scenario->setGameOver();
                    }
                    if (scenario->isGameOver()) {
                        // Armazena os dados do Game Over
                        int score = scenario->getScore();
                        // Garante que o jogador não é nullptr antes de acessar
                        if (jogador) {
                             int recordAntigo = jogador->getMaiorPontuacao();
                             int recordGeral = 0; // Vai ser o recorde geral da lista carregada
                             for (auto& p : playerManager->getJogadores()) {
                                 if (p.getMaiorPontuacao() > recordGeral)
                                     recordGeral = p.getMaiorPontuacao();
                             }
                             jogador->adicionarPartida(score);
                             playerManager->salvar(); // Salva as alterações
                             this->lastScore = score;
                             this->lastRecordPessoal = recordAntigo;
                             this->lastRecordGeral = recordGeral;
                             this->lastBateuRecordePessoal = (score > recordAntigo);
                             this->lastBateuRecordeGeral = (score > recordGeral);
                        } else { // Caso não haja jogador logado (nunca deveria acontecer se forçar apelido)
                             this->lastScore = score;
                             this->lastRecordPessoal = 0;
                             this->lastRecordGeral = 0;
                             this->lastBateuRecordePessoal = false;
                             this->lastBateuRecordeGeral = false;
                        }
                       
                        estado = GAME_OVER; // Muda o estado
                    }
                    break;
                case GAME_OVER:
                    // A tela de Game Over não tem uma lógica de "update" contínua própria
                    // O importante é que render() seja chamado a cada frame via 'redraw'
                    break;
            }
        }
        // Processa eventos de input independentemente do timer
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (estado == MENU) {
                menu.onClick(ev.mouse.x, ev.mouse.y);
                // Verifica se a ação do menu mudou de estado para RANKING, JOGANDO ou SAIR
                int acao = menu.getSelectedAction();
                if (acao == 1) { // Jogar
                    if (menu.getApelido().empty()) {
                        // A própria classe Menu já trata o showWarning
                        // Nao muda o estado se apelido vazio
                    } else {
                        std::string nick = menu.getApelido();
                        jogador = playerManager->buscar(nick);
                        if (!jogador) {
                            playerManager->cadastrar(nick, nick);
                            jogador = playerManager->buscar(nick);
                        }
                        scenario->reset();
                        estado = JOGANDO;
                        menu.resetAction();
                    }
                } else if (acao == 2) { // Ranking
                    // Passa os recursos já carregados
                    exibirRanking(playerManager, menu.getApelido(), screenWidth, screenHeight, bgRanking, fontRanking);
                    menu.resetAction(); // Reseta a acao apos exibir o ranking
                } else if (acao == 3) { // Sair
                    rodando = false;
                }
            } else if (estado == GAME_OVER) {
                int acao = gameover.handleEvent(ev);
                if (acao == 1) { // Jogar novamente
                    scenario->reset();
                    estado = JOGANDO;
                } else if (acao == 2) { // Voltar ao menu
                    estado = MENU;
                }
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (estado == JOGANDO) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    estado = MENU;
                } else if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    scenario->getBird().flap();
                }
            }
        } else if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (estado == MENU) {
                menu.onChar(ev.keyboard.unichar, ev.keyboard.keycode);
            }
        }
        // Eventos ALLEGRO_EVENT_MOUSE_AXES são consumidos, mas não disparam redraw diretamente.
        // A posição do mouse é obtida dentro do render() via al_get_mouse_state() para hover.


        // --- Renderização ---
        // Desenha a tela APENAS se 'redraw' for true E a fila de eventos estiver vazia.
        // Isso evita redesenhos múltiplos dentro de um único tick do timer
        // e garante que a tela só seja desenhada quando a lógica for atualizada.
        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(255, 255, 255)); // Limpa a tela uma vez por frame

            switch (estado) {
                case MENU:
                    menu.render();
                    break;
                case JOGANDO:
                    scenario->render();
                    break;
                case GAME_OVER:
                    // Passa os dados do último jogo para a tela de Game Over
                    gameover.render(lastScore, lastRecordPessoal, lastRecordGeral, lastBateuRecordePessoal, lastBateuRecordeGeral);
                    break;
            }
            al_flip_display(); // Atualiza a tela uma vez por frame
            redraw = false;    // Reseta a flag
        }
    }

    // --- Destrói todos os recursos carregados ---
    al_destroy_bitmap(birdBmp);
    al_destroy_bitmap(pipeBmp);
    al_destroy_bitmap(bg);
    al_destroy_bitmap(bgRanking); // Destrói o background do ranking
    al_destroy_font(font);
    al_destroy_font(fontRanking); // Destrói a fonte do ranking
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}