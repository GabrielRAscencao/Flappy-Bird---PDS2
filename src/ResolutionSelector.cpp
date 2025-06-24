/**
 * @file ResolutionSelector.cpp
 * @brief ResolutionSelectorimplementação do projeto Traveling Dragon.
 */


#include "ResolutionSelector.hpp"
#include <allegro5/allegro.h> // Core Allegro
#include <allegro5/allegro_font.h> // Para fontes
#include <allegro5/allegro_ttf.h> // Para fontes TrueType
#include <allegro5/allegro_primitives.h> // Para desenhar formas primitivas (botões)
#include <vector> // Para armazenar as opções de resolução
#include <string> // Para strings de texto
#include <optional> // Para retornar um valor opcional (se o usuário selecionar ou cancelar)
#include <cstdio> // Para fprintf (saída de erro)

/**
 * @brief Estrutura interna para armazenar detalhes de cada opção de resolução na tela.
 */
struct Option {
    ResolutionConfig config; /**< A configuração de resolução (largura, altura, modo). */
    std::string label;       /**< O texto que será exibido para a opção (ex: "1920x1080 (Tela Cheia)"). */
    float x, y, w, h;        /**< Posição (x, y) e dimensões (largura, altura) da área clicável do botão. */
};

/**
 * @brief Exibe uma janela de seleção de resolução para o usuário.
 *
 * Esta função inicializa partes básicas do Allegro, cria uma janela temporária
 * para a seleção e aguarda a interação do usuário.
 *
 * @return Um `std::optional<ResolutionConfig>` contendo a configuração selecionada
 * se o usuário escolher uma opção, ou `std::nullopt` se a janela for fechada
 * ou a seleção for cancelada (ex: ESC).
 */
std::optional<ResolutionConfig> showResolutionSelector() {
    // Inicializa os módulos básicos do Allegro. Se falhar, retorna nullopt.
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro\n");
        return std::nullopt;
    }
    // Instala os add-ons de mouse, teclado, fonte e primitivas.
    al_install_mouse();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    // Cria um display temporário para a tela de seleção de resolução.
    // As dimensões são fixas para esta tela.
    ALLEGRO_DISPLAY* disp = al_create_display(600, 550);
    if (!disp) {
        fprintf(stderr, "Falha ao criar display\n");
        return std::nullopt;
    }

    // Carrega a fonte para o texto das opções.
    ALLEGRO_FONT* font = al_load_ttf_font("assets/editundo.ttf", 20, 0);
    if (!font) {
        fprintf(stderr, "Falha ao carregar fonte\n");
        al_destroy_display(disp); // Destrói o display antes de sair
        return std::nullopt;
    }

    // Cria a fila de eventos e o timer para a renderização.
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60); // Timer para 60 FPS

    // Registra as fontes de eventos relevantes.
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    // Define as opções de resolução disponíveis.
    std::vector<Option> options = {
        {{1280, 720, WINDOWED}, "1280x720 (Janela)"},
        {{1600, 900, WINDOWED}, "1600x900 (Janela)"},
        {{1920, 1080, WINDOWED}, "1920x1080 (Janela)"},
        {{1280, 720, FULLSCREEN}, "1280x720 (Tela Cheia)"},
        {{1600, 900, FULLSCREEN}, "1600x900 (Tela Cheia)"},
        {{1920, 1080, FULLSCREEN}, "1920x1080 (Tela Cheia)"},
        {{1280, 720, BORDERLESS}, "1280x720 (Sem Borda)"},
        {{1600, 900, BORDERLESS}, "1600x900 (Sem Borda)"},
        {{1920, 1080, BORDERLESS}, "1920x1080 (Sem Borda)"},
    };

    // Calcula a posição e tamanho de cada opção (botão) na tela.
    for (size_t i = 0; i < options.size(); ++i) {
        options[i].x = 50;
        options[i].y = 50 + i * 50; // Cada opção 50 pixels abaixo da anterior
        options[i].w = 500;
        options[i].h = 40;
    }

    int selected = 0; // Índice da opção atualmente selecionada (padrão: primeira)
    bool running = true; // Flag para controlar o loop principal da tela de seleção
    bool redraw = true; // Flag para indicar se a tela precisa ser redesenhada
    ResolutionConfig chosen = options[0].config; // Configuração escolhida pelo usuário (padrão: primeira opção)

    al_start_timer(timer); // Inicia o timer

    // Loop principal da tela de seleção de resolução
    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev); // Espera por um evento

        switch (ev.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                running = false;
                chosen = {-1, -1, WINDOWED}; // Define valores inválidos para indicar cancelamento
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    running = false;
                    chosen = {-1, -1, WINDOWED}; // Define valores inválidos para indicar cancelamento
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    selected = (selected + 1) % static_cast<int>(options.size()); // Move seleção para baixo
                    redraw = true; // Força redesenho
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
                    selected = (selected - 1 + static_cast<int>(options.size())) % static_cast<int>(options.size()); // Move seleção para cima
                    redraw = true; // Força redesenho
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    chosen = options[selected].config; // Seleciona a opção atual
                    running = false; // Sai do loop
                }
                break;

            case ALLEGRO_EVENT_MOUSE_AXES: // Movimento do mouse
                for (size_t i = 0; i < options.size(); ++i) {
                    // Verifica se o mouse está sobre uma opção
                    if (ev.mouse.x >= options[i].x && ev.mouse.x <= options[i].x + options[i].w &&
                        ev.mouse.y >= options[i].y && ev.mouse.y <= options[i].y + options[i].h) {
                        if (static_cast<int>(i) != selected) {
                            selected = static_cast<int>(i); // Atualiza a opção selecionada pelo mouse
                            redraw = true; // Força redesenho
                        }
                        break; // Já encontrou, pode sair do loop
                    }
                }
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: // Clique do mouse
                for (size_t i = 0; i < options.size(); ++i) {
                    // Verifica se clicou em uma opção
                    if (ev.mouse.x >= options[i].x && ev.mouse.x <= options[i].x + options[i].w &&
                        ev.mouse.y >= options[i].y && ev.mouse.y <= options[i].y + options[i].h) {
                        chosen = options[i].config; // Define a configuração escolhida
                        running = false; // Sai do loop
                        break; // Já encontrou, pode sair do loop
                    }
                }
                break;

            case ALLEGRO_EVENT_TIMER:
                redraw = true; // O timer força o redesenho para a animação (se houver, como o hover)
                break;
        }

        // Se precisa redesenhar e a fila de eventos está vazia (otimização)
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;

            al_clear_to_color(al_map_rgb(30, 30, 30)); // Limpa a tela com um fundo escuro

            // Desenha cada opção de resolução como um botão
            for (size_t i = 0; i < options.size(); ++i) {
                ALLEGRO_COLOR bgColor = (static_cast<int>(i) == selected) ? al_map_rgb(80, 80, 30) : al_map_rgb(50, 50, 50); // Cor de fundo do botão (destacado se selecionado)
                ALLEGRO_COLOR textColor = (static_cast<int>(i) == selected) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255); // Cor do texto (destacado se selecionado)

                // Desenha o retângulo preenchido do botão
                al_draw_filled_rectangle(options[i].x, options[i].y, options[i].x + options[i].w, options[i].y + options[i].h, bgColor);
                // Desenha o texto da opção
                al_draw_text(font, textColor, options[i].x + 10, options[i].y + 10, 0, options[i].label.c_str());
            }
            al_flip_display(); // Troca os buffers para exibir o que foi desenhado
        }
    }

    // Limpeza: destrói os recursos do Allegro antes de sair da função.
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(disp);

    // Retorna nullopt se o usuário fechou a janela ou cancelou a seleção.
    if (chosen.width == -1 || chosen.height == -1)
        return std::nullopt;

    return chosen; // Retorna a configuração de resolução escolhida
}