#ifndef GAMEENGINE_HPP
#define GAMEENGINE_HPP

#include "Scenario.hpp"
#include "PlayerManager.hpp"

// Forward declaration para Player para evitar inclusões circulares, caso Player.hpp não seja incluído por PlayerManager.hpp
class Player; 

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

    // --- NOVAS VARIÁVEIS PARA OS DADOS DA TELA DE GAME OVER ---
    int lastScore;
    int lastRecordPessoal;
    int lastRecordGeral;
    bool lastBateuRecordePessoal;
    bool lastBateuRecordeGeral;
    // ---------------------------------------------------------
};

#endif