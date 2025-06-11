#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "Bird.hpp"
#include "Pipe.hpp"
using namespace std;

const float FPS = 60;
const int SCREEN_W = 800;
const int SCREEN_H = 600;
const float SCROLL_SPEED = 100.0f;
const float PIPE_INTERVAL = 2.5f;
const float GAP_HEIGHT = 200.0f;

int main() {
    srand(time(0));

    if (!al_init()) {
        cout << "nao iniciou allegro" << endl;
        return -1;
    }

    al_install_keyboard();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display) return -1;

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

    ALLEGRO_FONT* font = al_load_font("bin/ARIAL.TTF", 24, 0);
    ALLEGRO_BITMAP* bird_bmp = al_load_bitmap("bin/bird.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("bin/background.png");
    ALLEGRO_BITMAP* pipe_bmp = al_load_bitmap("bin/pipe.png");

    if (!font || !bird_bmp || !bg || !pipe_bmp) {
        cout << "erro ao carregar arquivos" << endl;
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_start_timer(timer);

    float escalaTela = float(SCREEN_W) / 800.0f;
    Bird bird(bird_bmp, SCREEN_W / 3.0f, SCREEN_H / 2.0f, escalaTela);

    float deltaTime = 1.0f / FPS;
    float pipe_timer = 0;
    int score = 0;
    float bg_offset = 0; // deslocamento do fundo

    vector<Pipe> pipes;
    vector<bool> pontuado;

    bool playing = true;

    while (playing) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            bg_offset -= SCROLL_SPEED * deltaTime;

            int bg_w = al_get_bitmap_width(bg);
            int bg_h = al_get_bitmap_height(bg);
            float scale_x = float(SCREEN_W) / float(bg_w);
            float scale_y = float(SCREEN_H) / float(bg_h);
            float bg_scale = max(scale_x, scale_y);

            int img_largura = bg_w * bg_scale;
            if (bg_offset <= -img_largura)
                bg_offset += img_largura;

            bird.update(deltaTime);
            pipe_timer += deltaTime;

            if (pipe_timer >= PIPE_INTERVAL) {
                pipe_timer = 0;
                float pipe_w = 80 * escalaTela;
                float minY = 100 * escalaTela;
                float maxY = SCREEN_H - GAP_HEIGHT * escalaTela - minY;
                float gapY = minY + rand() % int(maxY - minY);

                pipes.emplace_back(SCREEN_W, 0, pipe_w, gapY, SCROLL_SPEED, true, pipe_bmp);
                pipes.emplace_back(SCREEN_W, gapY + GAP_HEIGHT * escalaTela, pipe_w, SCREEN_H - (gapY + GAP_HEIGHT * escalaTela), SCROLL_SPEED, false, pipe_bmp);
                pontuado.push_back(false);
            }

            for (auto& pipe : pipes)
                pipe.update(deltaTime);

            for (size_t i = 0; i < pipes.size(); i += 2) {
                if (!pontuado[i / 2] && pipes[i].getX() + pipes[i].getWidth() < bird.getX()) {
                    score++;
                    pontuado[i / 2] = true;
                }
            }

            for (auto& pipe : pipes) {
                float px1 = pipe.getX();
                float px2 = px1 + pipe.getWidth();
                float py1 = pipe.getY();
                float py2 = py1 + pipe.getHeight();

                float bx1 = bird.getX() - bird.getWidth() / 2;
                float bx2 = bird.getX() + bird.getWidth() / 2;
                float by1 = bird.getY() - bird.getHeight() / 2;
                float by2 = bird.getY() + bird.getHeight() / 2;

                bool colidiuX = bx1 < px2 && bx2 > px1;
                bool colidiuY = by1 < py2 && by2 > py1;

                if (colidiuX && colidiuY) {
                    playing = false;
                    break;
                }
            }

            pipes.erase(remove_if(pipes.begin(), pipes.end(), [](Pipe& p) {
                return p.isOffScreen();
            }), pipes.end());

            while (pontuado.size() > pipes.size() / 2) {
                pontuado.erase(pontuado.begin());
            }

            al_clear_to_color(al_map_rgb(255, 255, 255));

            al_draw_scaled_bitmap(bg, 0, 0, bg_w, bg_h, bg_offset, 0, img_largura, SCREEN_H, 0);
            al_draw_scaled_bitmap(bg, 0, 0, bg_w, bg_h, bg_offset + img_largura, 0, img_largura, SCREEN_H, 0);

            for (auto& pipe : pipes)
                pipe.render();

            bird.render();

            al_draw_textf(font, al_map_rgb(0, 0, 0), 10, 10, 0, "Pontos: %d", score);
            al_flip_display();
        }

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE:
                    playing = false;
                    break;
                case ALLEGRO_KEY_SPACE:
                    bird.flap();
                    break;
            }
        }

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            playing = false;
        }
    }

    al_rest(1.5);
    al_destroy_bitmap(bg);
    al_destroy_bitmap(bird_bmp);
    al_destroy_bitmap(pipe_bmp);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}
