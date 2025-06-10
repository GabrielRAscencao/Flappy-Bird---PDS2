#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
using namespace std;

const int SCREEN_W = 800;
const int SCREEN_H = 600;

int main() {
    cout << "Iniciando Allegro..." << endl;

    if (!al_init()) {
        cout << "erro ao iniciar allegro :(" << endl;
        return -1;
    }
    cout << "Allegro iniciado" << endl;

    al_init_primitives_addon();

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_W, SCREEN_H);
    if (!display) {
        cout << "erro ao criar display" << endl;
        return -1;
    }
    cout << "Janela criada" << endl;

    al_clear_to_color(al_map_rgb(255, 255, 255));
    al_flip_display();

    cout << "Janela exibida, esperando 3 segundos..." << endl;
    al_rest(3.0);

    al_destroy_display(display);
    cout << "Janela fechada, finalizando programa." << endl;
    return 0;
}
