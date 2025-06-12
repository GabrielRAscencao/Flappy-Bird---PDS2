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
#include <algorithm>

GameEngine::GameEngine(int w, int h)
    : screenWidth(w), screenHeight(h), scenario(nullptr) {
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

void exibirRanking(PlayerManager* pm, const std::string& jogadorAtual, int screenWidth, int screenHeight) {
    ALLEGRO_BITMAP* bg = al_load_bitmap("bin/background.png");
    ALLEGRO_FONT* font = al_load_font("bin/ARIAL.ttf", 24, 0);
    if (!font || !bg) {
        std::cerr << "Erro ao carregar recursos para ranking\n";
        return;
    }

    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_start_timer(timer);

    std::vector<Player> jogadores = pm->getJogadores();
    std::sort(jogadores.begin(), jogadores.end(), compararPontuacao);

    float bgOffsetX = 0.0f;
    bool esperandoTecla = true;
    double lastTime = al_get_time();

    int posColX = screenWidth / 2 - 320;
    int nomeColX = screenWidth / 2 - 140;
    int scoreColX = screenWidth / 2 + 140;
    int partColX = screenWidth / 2 + 280;

    while (esperandoTecla) {
        ALLEGRO_EVENT ev;
        double currentTime = al_get_time();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (al_get_next_event(fila, &ev)) {
            if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
                esperandoTecla = false;
        }

        // Fundo animado
        bgOffsetX += 50.0f * deltaTime;
        int bgWidth = al_get_bitmap_width(bg);
        if (bgOffsetX >= bgWidth) bgOffsetX -= bgWidth;

        al_draw_scaled_bitmap(bg, 0, 0, bgWidth, al_get_bitmap_height(bg), -(int)bgOffsetX, 0, bgWidth, screenHeight, 0);
        al_draw_scaled_bitmap(bg, 0, 0, bgWidth, al_get_bitmap_height(bg), (int)(bgWidth - bgOffsetX), 0, bgWidth, screenHeight, 0);

        // Fundo transparente para a tabela
        al_draw_filled_rounded_rectangle(80, 60, screenWidth - 80, screenHeight - 150, 10, 10, al_map_rgba(0, 0, 0, 180));

        // Título
        al_draw_text(font, al_map_rgb(255, 255, 0), screenWidth / 2, 90, ALLEGRO_ALIGN_CENTER, "RANKING - TOP 10");

        // Cabeçalhos da tabela
        int y = 140;
        al_draw_text(font, al_map_rgb(200, 200, 200), posColX, y, 0, "POS");
        al_draw_text(font, al_map_rgb(200, 200, 200), nomeColX, y, 0, "NOME");
        al_draw_text(font, al_map_rgb(200, 200, 200), scoreColX, y, 0, "PONTOS");
        al_draw_text(font, al_map_rgb(200, 200, 200), partColX, y, 0, "PARTIDAS");
        y += 40;

        int colocacaoAtual = -1;

        for (int i = 0; i < jogadores.size(); ++i) {
            const Player& p = jogadores[i];
            if (p.getApelido() == jogadorAtual) colocacaoAtual = i + 1;

            if (i < 10) {
                ALLEGRO_COLOR cor = (p.getApelido() == jogadorAtual) ? al_map_rgb(0, 255, 0) : al_map_rgb(255, 255, 255);
                al_draw_textf(font, cor, posColX, y, 0, "%2d.", i + 1);
                al_draw_text(font, cor, nomeColX, y, 0, p.getNome().c_str());
                al_draw_textf(font, cor, scoreColX, y, 0, "%d", p.getMaiorPontuacao());
                al_draw_textf(font, cor, partColX, y, 0, "%d", p.getPartidas());
                y += 30;
            }
        }

        y += 20;

        if (colocacaoAtual != -1) {
            std::string msg = "Sua colocacao: " + std::to_string(colocacaoAtual);
            al_draw_text(font, al_map_rgb(0, 255, 0), screenWidth / 2, y, ALLEGRO_ALIGN_CENTER, msg.c_str());
        } else {
            al_draw_text(font, al_map_rgb(255, 0, 0), screenWidth / 2, y, ALLEGRO_ALIGN_CENTER, "Voce ainda nao jogou!");
        }

        al_draw_text(font, al_map_rgb(255, 255, 255), screenWidth / 2, screenHeight - 50, ALLEGRO_ALIGN_CENTER, "Pressione qualquer tecla para voltar");

        al_flip_display();
        al_rest(0.01);
    }

    al_destroy_event_queue(fila);
    al_destroy_timer(timer);
    al_destroy_bitmap(bg);
    al_destroy_font(font);
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

    ALLEGRO_DISPLAY* display = al_create_display(screenWidth, screenHeight);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);

    if (!display || !queue || !timer) {
        std::cerr << "Erro ao criar display, queue ou timer\n";
        return;
    }

    ALLEGRO_FONT* font = al_load_font("bin/Minecrafter.Reg.ttf", 24, 0);
    ALLEGRO_BITMAP* birdBmp = al_load_bitmap("bin/bird.png");
    ALLEGRO_BITMAP* pipeBmp = al_load_bitmap("bin/pipe.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("bin/background.png");

    if (!font || !birdBmp || !pipeBmp || !bg) {
        std::cerr << "Erro ao carregar recursos (fontes ou bitmaps)\n";
        return;
    }

    Menu menu(font, bg, screenWidth, screenHeight);
    GameOverScreen gameover(font, bg);
    scenario = new Scenario(bg, birdBmp, pipeBmp, font, screenWidth, screenHeight, scaleX, scaleY);

    EstadoJogo estado = MENU;
    Player* jogador = nullptr;

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    al_start_timer(timer);

    bool rodando = true;
    double lastTime = al_get_time();

    while (rodando) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = false;
            continue;
        }

        double currentTime = al_get_time();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        switch (estado) {
            case MENU:
                if (ev.type == ALLEGRO_EVENT_TIMER) {
                    menu.update(deltaTime);
                    al_clear_to_color(al_map_rgb(255, 255, 255));
                    menu.render();
                    al_flip_display();
                } else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                    menu.onClick(ev.mouse.x, ev.mouse.y);
                } else if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
                    menu.onChar(ev.keyboard.unichar, ev.keyboard.keycode);
                }

                {
                    int acao = menu.getSelectedAction();
                    if (acao == 1 && !menu.getApelido().empty()) {
                        std::string nick = menu.getApelido();
                        jogador = playerManager->buscar(nick);
                        if (!jogador) {
                            playerManager->cadastrar(nick, nick);
                            jogador = playerManager->buscar(nick);
                        }
                        scenario->reset();
                        estado = JOGANDO;
                        menu.resetAction();
                    } else if (acao == 2) {
                        exibirRanking(playerManager, menu.getApelido(), screenWidth, screenHeight);
                        menu.resetAction();
                    } else if (acao == 3) {
                        rodando = false;
                    }
                }
                break;

            case JOGANDO:
                if (ev.type == ALLEGRO_EVENT_TIMER) {
                    scenario->update(deltaTime);

                    if (scenario->getBird().getY() + scenario->getBird().getHeight() / 2 >= screenHeight) {
                        scenario->setGameOver();
                    }

                    al_clear_to_color(al_map_rgb(255, 255, 255));
                    scenario->render();
                    al_flip_display();

                    if (scenario->isGameOver()) {
                        int score = scenario->getScore();
                        int recordAntigo = jogador->getMaiorPontuacao();

                        int recordGeral = 0;
                        for (auto& p : playerManager->getJogadores()) {
                            if (p.getMaiorPontuacao() > recordGeral)
                                recordGeral = p.getMaiorPontuacao();
                        }

                        jogador->adicionarPartida(score);
                        playerManager->salvar();

                        estado = GAME_OVER;

                        gameover.render(score, recordAntigo, recordGeral, score > recordAntigo, score > recordGeral);
                        al_flip_display();
                    }
                } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                        estado = MENU;
                    } else if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                        scenario->getBird().flap();
                    }
                }
                break;

            case GAME_OVER:
                if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                    int acao = gameover.handleEvent(ev);
                    if (acao == 1) {
                        scenario->reset();
                        estado = JOGANDO;
                    } else if (acao == 2) {
                        estado = MENU;
                    }
                }
                break;
        }
    }

    al_destroy_bitmap(birdBmp);
    al_destroy_bitmap(pipeBmp);
    al_destroy_bitmap(bg);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}
