/**
 * @file GameEngine.hpp
 * @brief GameEngineheader do projeto Traveling Dragon.
 */

#ifndef GAMEENGINE_HPP
#define GAMEENGINE_HPP

#include <allegro5/allegro.h>          // Para funcionalidades básicas do Allegro (display, queue, timer)
#include <vector>                      // Para usar std::vector (listas de assets, jogadores)
#include <string>                      // Para usar std::string (nomes, caminhos)
#include "PlayerManager.hpp"           // Gerenciador de jogadores e ranking
#include "Menu.hpp"                    // Tela do menu principal
#include "Scenario.hpp"                // Lógica e renderização do gameplay
#include "GameOverScreen.hpp"          // Tela de Game Over
#include "RankingScreen.hpp"           // Tela de ranking de jogadores
#include "ResolutionSelector.hpp"      // Seleção de resolução da janela
#include "ConfigScreen.hpp"            // Tela de configurações
#include "Utils.hpp"                   // Save do ranking


/**
 * @brief O coração do jogo, gerenciando o loop principal, estados e componentes.
 *
 * A classe GameEngine é responsável por inicializar o Allegro, carregar recursos,
 * controlar o fluxo entre as diferentes telas (menu, jogo, game over, ranking, configurações),
 * processar eventos de entrada e gerenciar o loop de atualização e renderização do jogo.
 */
class GameEngine {
public:
    /**
     * @brief Construtor da classe GameEngine.
     *
     * Inicializa o motor do jogo, configurando as dimensões da janela e o modo de exibição.
     * Realiza a inicialização básica do Allegro e seus add-ons.
     *
     * @param w Largura inicial da tela/janela.
     * @param h Altura inicial da tela/janela.
     * @param mode O modo de exibição da janela (WINDOWED, FULLSCREEN, BORDERLESS), padrão é WINDOWED.
     */
    GameEngine(int w, int h, WindowMode mode = WINDOWED);

    /**
     * @brief Destrutor da classe GameEngine.
     *
     * Responsável por liberar todos os recursos alocados pelo jogo,
     * como displays, timers, queues, fontes, bitmaps, streams de áudio e objetos de tela.
     */
    ~GameEngine();

    /**
     * @brief Inicia e executa o loop principal do jogo.
     *
     * Este método contém o ciclo de vida do jogo: processamento de eventos,
     * atualização da lógica e renderização, até que o jogo seja encerrado.
     */
    void run();

private:
    int screenWidth;            ///< @brief Largura atual da tela de exibição do jogo.
    int screenHeight;           ///< @brief Altura atual da tela de exibição do jogo.
    WindowMode windowMode;      ///< @brief O modo de exibição atual da janela (janela, tela cheia, sem borda).

    float resolucaoY;           ///< @brief Resolução de referência Y para escalonamento de elementos.
    float resolucaoX;           ///< @brief Resolução de referência X para escalonamento de elementos.

    ALLEGRO_DISPLAY* display;       ///< @brief Ponteiro para o display (janela) principal do Allegro.
    ALLEGRO_EVENT_QUEUE* queue;     ///< @brief Ponteiro para a fila de eventos do Allegro.
    ALLEGRO_TIMER* timer;           ///< @brief Ponteiro para o timer do Allegro, que controla o FPS.

    ALLEGRO_FONT* font;             ///< @brief Ponteiro para a fonte padrão usada no jogo.
    ALLEGRO_FONT* fontlarge;        ///< @brief Ponteiro para uma fonte maior, para títulos e destaques.

    ALLEGRO_BITMAP* bg;             ///< @brief Imagem de fundo padrão (ex: para o menu principal).
    ALLEGRO_BITMAP* rankingBackground; ///< @brief Imagem de fundo específica para a tela de ranking.
    ALLEGRO_BITMAP* gameOverBackground; ///< @brief Imagem de fundo específica para a tela de Game Over.
    ALLEGRO_BITMAP* birdBmp;        ///< @brief Bitmap (folha de sprites) do personagem pássaro.
    ALLEGRO_BITMAP* pipeBmp;        ///< @brief Bitmap da imagem dos canos (obstáculos).

    Menu* menu;                         ///< @brief Objeto que gerencia a tela do menu principal.
    Scenario* scenario;                 ///< @brief Objeto que gerencia o cenário de jogo (gameplay).
    GameOverScreen* gameOverScreen;     ///< @brief Objeto que gerencia a tela de Game Over.
    RankingScreen* rankingScreen;       ///< @brief Objeto que gerencia a tela de ranking.
    ConfigScreen* configScreen;         ///< @brief Objeto que gerencia a tela de configurações.

    PlayerManager* playerManager;       ///< @brief Objeto que gerencia o carregamento, salvamento e busca de jogadores.
    Player* currentPlayer;              ///< @brief Ponteiro para o jogador atualmente ativo na sessão.

    /**
     * @brief Enumeração para os possíveis estados globais do jogo.
     * Controla qual tela ou fase do jogo está ativa no momento.
     */
    enum Estado {
        MENU,           ///< @brief O jogo está na tela do menu principal.
        JOGANDO,        ///< @brief O jogo está em andamento (gameplay ativo).
        GAME_OVER,      ///< @brief O jogo está na tela de "Game Over".
        RANKING,        ///< @brief O jogo está na tela de ranking de jogadores.
        CONFIG_SCREEN,  ///< @brief O jogo está na tela de configurações.
        INICIANDO_JOGO  ///< @brief Estado temporário para iniciar uma nova partida ou nível.
    };
    Estado estadoAtual; ///< @brief A variável que armazena o estado atual do jogo.

    int lastScore;                      ///< @brief Pontuação alcançada na última partida jogada.
    int lastRecordPessoal;              ///< @brief O recorde pessoal do jogador na última partida (para exibição).
    int lastRecordGeral;                ///< @brief O recorde geral entre todos os jogadores na última partida (para exibição).
    bool lastBateuRecordePessoal;       ///< @brief Flag: true se o jogador bateu seu recorde pessoal na última partida.
    bool lastBateuRecordeGeral;         ///< @brief Flag: true se o jogador bateu o recorde geral na última partida.

    ALLEGRO_AUDIO_STREAM* musicaMenuRankingGameOver; ///< @brief Stream de áudio para a música das telas de menu, ranking e game over.
    ALLEGRO_AUDIO_STREAM* musicaEmJogo;              ///< @brief Stream de áudio para a música tocada durante o gameplay (OBS: pode estar em desuso, ver `musicLevels`).
    ALLEGRO_AUDIO_STREAM* musicaAtualTocando;        ///< @brief Ponteiro para o stream de áudio da música que está sendo tocada no momento.

    ALLEGRO_SAMPLE* somHover;       ///< @brief Sample de áudio para o efeito sonoro de mouse sobre botões.
    ALLEGRO_SAMPLE* somFlap;        ///< @brief Sample de áudio para o som do pássaro batendo as asas.
    ALLEGRO_SAMPLE* somDie;         ///< @brief Sample de áudio para o som de morte do pássaro.
    ALLEGRO_SAMPLE* somPoint;       ///< @brief Sample de áudio para o som de pontuação.
    ALLEGRO_SAMPLE* somTransition;  ///< @brief Sample de áudio para o som de transição de nível.

    bool fecharJogo;                ///< @brief Flag que, quando true, indica que o jogo deve ser encerrado.

    bool inLevelTransition;         ///< @brief Flag: true se uma transição de nível está em andamento.
    bool fadeOutPhase;              ///< @brief (OBS: Pode não estar sendo utilizado ou ser para uma fase específica de fade-out).
    bool isTransitionBlurActive;    ///< @brief Flag: true se o efeito de blur durante a transição de nível está ativo.
    float transitionBlurTimer;      ///< @brief Timer para controlar a duração do efeito de blur durante a transição.

    ALLEGRO_BITMAP* renderTarget;   ///< @brief Bitmap temporário usado como buffer de renderização para aplicar efeitos antes de desenhar no display.

    std::vector<ALLEGRO_BITMAP*> backgroundsLevels; ///< @brief Vetor de bitmaps para os diferentes fundos de cenário por nível.
    std::vector<ALLEGRO_BITMAP*> pipesLevels;       ///< @brief Vetor de bitmaps para as diferentes imagens de canos por nível.
    std::vector<ALLEGRO_AUDIO_STREAM*> musicLevels; ///< @brief Vetor de streams de áudio para as músicas de cada nível.

    int currentLevel;               ///< @brief O nível atual do jogo (começando de 0).
    /// @brief Duração em segundos do efeito de blur durante a transição entre níveis.
    const float TRANSITION_BLUR_DURATION = 1.5f;

    float scaleX;                   ///< @brief Fator de escalonamento horizontal aplicado à renderização.
    float scaleY;                   ///< @brief Fator de escalonamento vertical aplicado à renderização.

    /**
     * @brief Inicializa todos os add-ons necessários do Allegro.
     * Isso inclui inicialização de teclado, mouse, áudio, primitivas, imagens, fontes, etc.
     */
    void initializeAllegroAddons();

    /**
     * @brief Carrega todos os assets (imagens, fontes, sons) necessários para o jogo.
     * Este método é chamado na inicialização para preparar os recursos.
     */
    void loadGameAssets();

    /**
     * @brief Libera a memória de todos os assets carregados.
     * Este método é chamado no destrutor para evitar vazamentos de memória.
     */
    void destroyGameAssets();

    /**
     * @brief Inicia a reprodução de uma stream de áudio.
     * Primeiro para a música atual, se houver, e então começa a tocar a nova.
     * @param musicStream Ponteiro para o ALLEGRO_AUDIO_STREAM a ser tocado.
     */
    void playMusic(ALLEGRO_AUDIO_STREAM* musicStream);

    /**
     * @brief Para a música que está sendo tocada no momento.
     */
    void stopCurrentMusic();

    /**
     * @brief Processa um evento de entrada do Allegro (teclado, mouse, etc.).
     * Delega o evento para a tela ou componente apropriado com base no estado atual do jogo.
     * @param ev Uma referência para o evento Allegro a ser processado.
     */
    void handleInput(ALLEGRO_EVENT& ev);

    /**
     * @brief Atualiza a lógica de todos os componentes do jogo.
     *
     * Este método é chamado a cada frame e delega a atualização para a tela
     * ou componente ativo, além de gerenciar transições de nível e estados globais.
     *
     * @param deltaTime O tempo decorrido desde a última atualização (em segundos).
     */
    void update(double deltaTime);

    /**
     * @brief Renderiza a tela do menu principal.
     * @param deltaTime O tempo decorrido para possíveis animações de fundo no menu.
     */
    void renderMenu(float deltaTime);

    /**
     * @brief Renderiza o cenário de jogo (gameplay).
     * Desenha o pássaro, os canos, o fundo e a pontuação durante o jogo ativo.
     */
    void renderGame();

    /**
     * @brief Renderiza a tela de Game Over.
     * Exibe a pontuação final, recordes e opções de reiniciar/voltar ao menu.
     */
    void renderGameOver();

    /**
     * @brief Renderiza a tela de ranking.
     * Exibe a lista de jogadores e suas maiores pontuações.
     * @param deltaTime O tempo decorrido para possíveis animações de fundo no ranking.
     */
    void renderRanking(float deltaTime);

    /**
     * @brief Renderiza a tela de configurações.
     * @param deltaTime O tempo decorrido para possíveis animações de fundo.
     */
    void renderConfigScreen(float deltaTime);

    /**
     * @brief Função estática de comparação usada para ordenar jogadores por pontuação.
     * Essencial para a exibição correta do ranking.
     * @param a Uma referência constante para o primeiro objeto Player a ser comparado.
     * @param b Uma referência constante para o segundo objeto Player a ser comparado.
     * @return true se o jogador 'a' tem uma pontuação maior que o jogador 'b', false caso contrário.
     */
    static bool compararPontuacao(const Player& a, const Player& b);
};

#endif // GAMEENGINE_HPP