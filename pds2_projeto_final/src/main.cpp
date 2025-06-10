#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

#include <iostream>
using namespace std;

const float FPS = 30;
// pode mudar o tamanho da tela aq q o resto se ajusta de boa
const int SCREEN_W = 800;
const int SCREEN_H = 600;

int main() {
    if (!al_init()) {
        cout << "nao deu pra iniciar o allegro" << endl;
        return -1;
    }

    al_init_primitives_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display) {
        cout << "nao criou a tela" << endl;
        return -1;
    }

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

    if (!event_queue || !timer) {
        cout << "nao criou a fila ou o timer" << endl;
        al_destroy_display(display);
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_start_timer(timer);

    ALLEGRO_FONT* font_arial = al_load_font("bin/ARIAL.TTF", 24, 0);
    if (!font_arial) {
        cout << "fonte nao carregou" << endl;
        al_destroy_display(display);
        al_destroy_event_queue(event_queue);
        al_destroy_timer(timer);
        return -1;
    }

    ALLEGRO_BITMAP* bird_bmp = al_load_bitmap("bin/bird.png");
    if (!bird_bmp) {
        cout << "imagem do bird deu erro" << endl;
        al_destroy_display(display);
        al_destroy_event_queue(event_queue);
        al_destroy_timer(timer);
        al_destroy_font(font_arial);
        return -1;
    }

    ALLEGRO_BITMAP* background_bmp = al_load_bitmap("bin/background.jpg");
    if (!background_bmp) {
        cout << "background nao carregou" << endl;
        al_destroy_bitmap(bird_bmp);
        al_destroy_display(display);
        al_destroy_event_queue(event_queue);
        al_destroy_timer(timer);
        al_destroy_font(font_arial);
        return -1;
    }

    bool playing = true;
    int offset_x = 0, offset_y = 0;
    string last_command = "None";

    // pega o tamanho real do bird pra poder escalar
    int bird_orig_w = al_get_bitmap_width(bird_bmp);
    int bird_orig_h = al_get_bitmap_height(bird_bmp);

    // escala o bird pra ficar proporcional
    float bird_scale = 1 * (float(SCREEN_W) / 800.0f);

    while (playing) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            // limpa a tela com branco antes de desenhar
            al_clear_to_color(al_map_rgb(255, 255, 255));

            // desenha o fundo e escala pra caber na tela toda
            int bg_w = al_get_bitmap_width(background_bmp);
            int bg_h = al_get_bitmap_height(background_bmp);
            float scale_x = float(SCREEN_W) / float(bg_w);
            float scale_y = float(SCREEN_H) / float(bg_h);
            float bg_scale = max(scale_x, scale_y); // usa o maior pra n sobrar buraco
            al_draw_scaled_bitmap(background_bmp,
                                  0, 0, bg_w, bg_h,
                                  0, 0, bg_w * bg_scale, bg_h * bg_scale,
                                  0);

            // coloca o bird no meio da tela (com deslocamento)
            float bird_w = bird_orig_w * bird_scale;
            float bird_h = bird_orig_h * bird_scale;
            float bird_x = SCREEN_W / 2.0f + offset_x;
            float bird_y = SCREEN_H / 2.0f + offset_y;

            // desenha o bird escalado
            al_draw_scaled_bitmap(bird_bmp,
                                  0, 0, bird_orig_w, bird_orig_h,
                                  bird_x - bird_w / 2, bird_y - bird_h / 2,
                                  bird_w, bird_h,
                                  0);

            // texto do comando que foi apertado por último
            int font_size = int(24 * (float(SCREEN_H) / 600.0f)); // se mudar a tela ele ajusta tb
            al_draw_text(font_arial, al_map_rgb(0, 0, 0),
                         5, 10, ALLEGRO_ALIGN_LEFT,
                         ("Last command: " + last_command).c_str());

            al_flip_display();
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE:
                    playing = false;
                    break;
                case ALLEGRO_KEY_UP:
                    offset_y -= 20;
                    last_command = "Up";
                    break;
                case ALLEGRO_KEY_DOWN:
                    offset_y += 20;
                    last_command = "Down";
                    break;
                case ALLEGRO_KEY_LEFT:
                    offset_x -= 20;
                    last_command = "Left";
                    break;
                case ALLEGRO_KEY_RIGHT:
                    offset_x += 20;
                    last_command = "Right";
                    break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            playing = false;
        }
    }

    cout << "fechando o prog..." << endl;
    al_rest(2.0);

    al_destroy_bitmap(background_bmp);
    al_destroy_bitmap(bird_bmp);
    al_destroy_font(font_arial);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}
