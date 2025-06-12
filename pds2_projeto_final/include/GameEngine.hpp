#ifndef GAMEENGINE_HPP
#define GAMEENGINE_HPP

#include "Scenario.hpp"
#include "PlayerManager.hpp"

enum EstadoJogo {
    MENU,
    JOGANDO,
    GAME_OVER
};

class GameEngine {
public:
    // Construtor com parâmetros padrão para resolução
    GameEngine(int w = 1280, int h = 720);
    ~GameEngine();

    void run();

private:
    Scenario* scenario;
    PlayerManager* playerManager;

    int screenWidth;
    int screenHeight;
    float scaleX;
    float scaleY;
};

#endif
