/**
 * @file GameEngine.cpp
 * @brief GameEngineimplementação do projeto Traveling Dragon.
 */

#include "GameEngine.hpp"
#include "ConfigScreen.hpp" // Embora não haja um .hpp com esse nome exato no seu envio, incluí pela referência aqui.
#include <allegro5/allegro_audio.h>    // Para funcionalidades de áudio do Allegro
#include <allegro5/allegro_acodec.h>    // Para codecs de áudio do Allegro
#include <allegro5/allegro_primitives.h>// Para funções de desenho de primitivas (retângulos, etc.)
#include <allegro5/allegro_ttf.h>       // Para fontes TrueType
#include <allegro5/allegro_image.h>     // Para carregar e manipular imagens
#include <iostream>                     // Para saída de console (std::cerr, std::cout)
#include <sstream>                      // Para manipular strings (construção de caminhos de arquivo)

/**
 * @brief Construtor da classe GameEngine.
 *
 * Inicializa os membros da classe com valores padrão e configura as dimensões
 * da tela e o modo de janela. Também carrega o gerenciador de jogadores.
 *
 * @param w Largura da tela em pixels.
 * @param h Altura da tela em pixels.
 * @param mode Modo de exibição da janela (WINDOWED, FULLSCREEN, BORDERLESS).
 */
GameEngine::GameEngine(int w, int h, WindowMode mode)
    : screenWidth(w), screenHeight(h), windowMode(mode),
      resolucaoY(720.0f), resolucaoX(1280.0f), // Resolução de referência para escalonamento
      display(nullptr), queue(nullptr), timer(nullptr),
      font(nullptr), fontlarge(nullptr), bg(nullptr), rankingBackground(nullptr), gameOverBackground(nullptr), birdBmp(nullptr),
      pipeBmp(nullptr),
      menu(nullptr), scenario(nullptr), gameOverScreen(nullptr),
      rankingScreen(nullptr), configScreen(nullptr),
      playerManager(nullptr), currentPlayer(nullptr), estadoAtual(MENU),
      lastScore(0), lastRecordPessoal(0), lastRecordGeral(0),
      lastBateuRecordePessoal(false), lastBateuRecordeGeral(false),

      musicaMenuRankingGameOver(nullptr),
      musicaEmJogo(nullptr), // OBS: Parece não ser utilizada diretamente, 'musicLevels' é o foco.
      musicaAtualTocando(nullptr),

      somHover(nullptr),
      somFlap(nullptr),
      somDie(nullptr),
      somPoint(nullptr),
      somTransition(nullptr),

      fecharJogo(false),

      inLevelTransition(false),
      fadeOutPhase(true), // OBS: Variável 'fadeOutPhase' não parece ser utilizada no código atual.
      isTransitionBlurActive(false),
      transitionBlurTimer(0.0f),
      renderTarget(nullptr)
{
    // Calcula os fatores de escalonamento para ajustar os elementos visuais à resolução atual.
    scaleX = (float)screenWidth / resolucaoX;
    scaleY = (float)screenHeight / resolucaoY;

    // Instancia o gerenciador de jogadores e carrega os dados persistidos.
    playerManager = new PlayerManager(getSaveFilePath());
    playerManager->carregar();
}

/**
 * @brief Destrutor da classe GameEngine.
 *
 * Responsável por liberar todos os recursos alocados durante a execução do jogo,
 * garantindo que não haja vazamentos de memória. Isso inclui assets (imagens, sons, fontes),
 * objetos de telas e componentes do Allegro.
 */
GameEngine::~GameEngine() {
    // Primeiro, libera os recursos de jogo como imagens e sons.
    destroyGameAssets();

    // Depois, deleta os objetos de tela e gerenciadores.
    if (menu) { delete menu; menu = nullptr; }
    if (scenario) { delete scenario; scenario = nullptr; }
    if (gameOverScreen) { delete gameOverScreen; gameOverScreen = nullptr; }
    if (rankingScreen) { delete rankingScreen; rankingScreen = nullptr; }
    if (configScreen) { delete configScreen; configScreen = nullptr; }
    // O playerManager é deletado por último, pois pode ter sido usado por outras telas.
    if (playerManager) { delete playerManager; playerManager = nullptr; }

    // Por último, destrói os componentes fundamentais do Allegro.
    if (timer) { al_destroy_timer(timer); timer = nullptr; }
    if (queue) { al_destroy_event_queue(queue); queue = nullptr; }
    if (renderTarget) { al_destroy_bitmap(renderTarget); renderTarget = nullptr; }
    if (display) { al_destroy_display(display); display = nullptr; }

    // Desinstala o subsistema de áudio do Allegro.
    al_uninstall_audio();
}

/**
 * @brief Inicializa todos os add-ons necessários do Allegro.
 *
 * Este método configura o Allegro e seus subsistemas (teclado, mouse, áudio,
 * fontes, imagens, primitivas) e cria o display principal, a fila de eventos
 * e o timer do jogo. Em caso de falha, o programa é encerrado.
 */
void GameEngine::initializeAllegroAddons() {
    // Tenta inicializar o Allegro principal.
    if (!al_init()) {
        std::cerr << "Erro ao inicializar Allegro!\n";
        exit(-1); // Sai do programa em caso de erro.
    }

    // Instala o teclado e checa por erros.
    if (!al_install_keyboard()) {
        std::cerr << "Erro ao instalar o teclado!\n";
        exit(-1);
    } else {
        std::cout << "Teclado instalado com sucesso.\n";
    }

    // Instala o mouse e checa por erros.
    if (!al_install_mouse()) {
        std::cerr << "Erro ao instalar o mouse!\n";
        exit(-1);
    } else {
        std::cout << "Mouse instalado com sucesso.\n";
    }

    // Inicializa os add-ons de fonte (TTF), imagem e primitivas.
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_init_primitives_addon();

    // Tenta instalar o subsistema de áudio e inicializar o codec.
    if (!al_install_audio()) {
        std::cerr << "Erro ao instalar o áudio!\n";
        exit(-1);
    }
    al_init_acodec_addon();
    al_reserve_samples(16); // Reserva 16 "slots" para samples de áudio.

    // Define as flags do display baseado no modo de janela escolhido.
    int flags = 0;
    switch(windowMode) {
        case WINDOWED:
            flags = ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE; // Janela redimensionável.
            std::cout << "Modo de janela selecionado (WINDOWED).\n";
            break;
        case FULLSCREEN:
            flags = ALLEGRO_FULLSCREEN_WINDOW; // Tela cheia exclusiva.
            std::cout << "Modo de janela selecionado (FULLSCREEN).\n";
            break;
        case BORDERLESS:
            flags = ALLEGRO_OPENGL | ALLEGRO_NOFRAME; // Sem borda, otimizado para OpenGL.
            std::cout << "Modo de janela selecionado (BORDERLESS).\n";
            break;
        default: // Caso um modo inválido seja fornecido, usa o padrão.
            flags = ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE;
            std::cout << "Modo de janela padrão selecionado.\n";
            break;
    }

    // Configura as novas flags e tenta criar o display.
    al_set_new_display_flags(flags);
    display = al_create_display(screenWidth, screenHeight);
    if (!display) {
        std::cerr << "Falha ao criar display!\n";
        exit(-1);
    }

    // Carrega e define o ícone da janela, se disponível.
    ALLEGRO_BITMAP* icon = al_load_bitmap("assets/icon.ico");
    if (icon) {
        al_set_display_icon(display, icon);
        al_destroy_bitmap(icon); // Destrói o bitmap do ícone após configurá-lo.
    }

    // Cria a fila de eventos, o timer para 60 FPS e um bitmap de renderização (buffer).
    queue = al_create_event_queue();
    timer = al_create_timer(1.0 / 60.0); // Timer para uma taxa de atualização de 60 frames por segundo (FPS).
    renderTarget = al_create_bitmap(screenWidth, screenHeight); // Bitmap para renderização off-screen.

    // Registra as fontes de eventos na fila para que o jogo possa responder a eles.
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());

    std::cout << "Allegro inicializado com sucesso.\n";
}

/**
 * @brief Carrega todos os assets (imagens, fontes, sons) necessários para o jogo.
 *
 * Este método é chamado uma única vez na inicialização do GameEngine para carregar
 * todos os recursos gráficos e sonoros que serão utilizados pelas diferentes telas e componentes.
 */
void GameEngine::loadGameAssets() {
    // Carrega as fontes do jogo, escalando o tamanho com base na resolução.
    font = al_load_ttf_font("assets/editundo.ttf", static_cast<int>(24 * scaleY), 0);
    fontlarge = al_load_ttf_font("assets/editundo.ttf", static_cast<int>(42 * scaleY), 0);
    // Se a fonte grande não carregar, usa a fonte normal como alternativa.
    if (!fontlarge) {
        std::cerr << "Erro ao carregar fontlarge. Usando font como fallback.\n";
        fontlarge = font;
    }

    // Carrega as imagens principais do jogo.
    birdBmp = al_load_bitmap("assets/dragontest.png");
    bg = al_load_bitmap("assets/menu_background.png");
    rankingBackground = al_load_bitmap("assets/ranking_bg.png");
    gameOverBackground = al_load_bitmap("assets/gameover_bg.png");

    // Itera para carregar os assets específicos de cada nível (backgrounds, pipes e músicas).
    for (int i = 1; i <= 7; ++i) {
        std::stringstream pathBg, pathPipe, pathMusic;
        pathBg << "assets/background" << i << ".png";
        pathPipe << "assets/pipe" << i << ".png";
        pathMusic << "assets/level" << i << ".ogg";

        ALLEGRO_BITMAP* bgTemp = al_load_bitmap(pathBg.str().c_str());
        ALLEGRO_BITMAP* pipeTemp = al_load_bitmap(pathPipe.str().c_str());
        ALLEGRO_AUDIO_STREAM* musicTemp = al_load_audio_stream(pathMusic.str().c_str(), 4, 2048); // Stream de áudio com 4 buffers e buffer size de 2048.

        // Verifica se todos os assets do nível foram carregados com sucesso antes de adicioná-los.
        if (bgTemp && pipeTemp && musicTemp) {
            backgroundsLevels.push_back(bgTemp);
            pipesLevels.push_back(pipeTemp);
            musicLevels.push_back(musicTemp);
        } else {
            std::cerr << "Erro ao carregar arquivos do nível " << i << ": "
                      << (bgTemp ? "" : "background.png ") << (pipeTemp ? "" : "pipe.png ")
                      << (musicTemp ? "" : "music.ogg ") << "\n";
            // Em um jogo real, você poderia carregar assets padrão ou exibir uma mensagem de erro mais proeminente aqui.
        }
    }

    // Carrega a música de fundo para os menus e os samples de efeito sonoro.
    musicaMenuRankingGameOver = al_load_audio_stream("assets/menu.ogg", 4, 2048);
    somFlap = al_load_sample("assets/asas.wav");
    somDie = al_load_sample("assets/die.wav");
    somPoint = al_load_sample("assets/point.wav");
    somTransition = al_load_sample("assets/newlevel.wav");
    somHover = al_load_sample("assets/hover_button.wav");

    // Instancia os objetos das diferentes telas do jogo e configura o som de hover para o menu.
    menu = new Menu(font, fontlarge, bg);
    menu->setHoverSound(somHover);

    rankingScreen = new RankingScreen(fontlarge, rankingBackground, playerManager, screenWidth, screenHeight);
    gameOverScreen = new GameOverScreen(font, gameOverBackground);
    gameOverScreen->setHoverSound(somHover);

    configScreen = new ConfigScreen(fontlarge, rankingBackground, screenWidth, screenHeight); // Cria a tela de configurações.
    // O cenário é inicializado com os assets do primeiro nível (índice 0).
    scenario = new Scenario(backgroundsLevels[0], birdBmp, pipesLevels[0], fontlarge, screenWidth, screenHeight, somPoint, somDie);
}

/**
 * @brief Libera a memória de todos os assets carregados.
 *
 * Este método é chamado no destrutor da classe GameEngine para descarregar
 * todas as imagens, fontes e sons, prevenindo vazamentos de memória.
 */
void GameEngine::destroyGameAssets() {
    // Destrói os bitmaps principais, se não forem nulos.
    if (birdBmp) { al_destroy_bitmap(birdBmp); birdBmp = nullptr; }
    if (pipeBmp) { al_destroy_bitmap(pipeBmp); pipeBmp = nullptr; }
    if (bg) { al_destroy_bitmap(bg); bg = nullptr; }
    if (rankingBackground) { al_destroy_bitmap(rankingBackground); rankingBackground = nullptr; }
    if (gameOverBackground) { al_destroy_bitmap(gameOverBackground); gameOverBackground = nullptr; }

    // Destrói as fontes. É importante verificar se 'fontlarge' é diferente de 'font'
    // para não tentar destruir a mesma fonte duas vezes, caso 'fontlarge' tenha sido um fallback.
    if (font) { al_destroy_font(font); font = nullptr; }
    if (fontlarge && fontlarge != font) { al_destroy_font(fontlarge); fontlarge = nullptr; }

    // Destrói os streams de áudio e limpa o vetor.
    if (musicaMenuRankingGameOver) { al_destroy_audio_stream(musicaMenuRankingGameOver); musicaMenuRankingGameOver = nullptr; }
    // OBS: 'musicaEmJogo' não parece ser usada. Se usada, adicionar al_destroy_audio_stream aqui.
    for (auto& m : musicLevels) { al_destroy_audio_stream(m); }
    musicLevels.clear(); // Limpa o vetor após destruir os streams.

    // Destrói os bitmaps de backgrounds e pipes de cada nível e limpa os vetores.
    for (auto& b : backgroundsLevels) { al_destroy_bitmap(b); }
    backgroundsLevels.clear();
    for (auto& p : pipesLevels) { al_destroy_bitmap(p); }
    pipesLevels.clear();

    // Destrói os samples de som.
    if (somFlap) { al_destroy_sample(somFlap); somFlap = nullptr; }
    if (somDie) { al_destroy_sample(somDie); somDie = nullptr; }
    if (somPoint) { al_destroy_sample(somPoint); somPoint = nullptr; }
    if (somTransition) { al_destroy_sample(somTransition); somTransition = nullptr; }
    if (somHover) { al_destroy_sample(somHover); somHover = nullptr; }
}

/**
 * @brief Inicia a reprodução de uma stream de áudio.
 *
 * Primeiro para a música que estiver tocando, depois anexa a nova stream
 * ao mixer padrão do Allegro e inicia sua reprodução.
 *
 * @param musicStream Ponteiro para o ALLEGRO_AUDIO_STREAM a ser tocado.
 */
void GameEngine::playMusic(ALLEGRO_AUDIO_STREAM* musicStream) {
    if (musicStream) {
        // Para a música atual antes de tocar uma nova.
        stopCurrentMusic();
        al_attach_audio_stream_to_mixer(musicStream, al_get_default_mixer());
        al_set_audio_stream_playing(musicStream, true);
        musicaAtualTocando = musicStream; // Atualiza a referência para a música atual.
    }
}

/**
 * @brief Para a música que está sendo tocada no momento.
 *
 * Desanexa o stream do mixer e reseta a referência para 'musicaAtualTocando'.
 */
void GameEngine::stopCurrentMusic() {
    if (musicaAtualTocando) {
        al_set_audio_stream_playing(musicaAtualTocando, false); // Pausa a reprodução.
        al_detach_audio_stream(musicaAtualTocando); // Desanexa do mixer.
        musicaAtualTocando = nullptr; // Reseta a referência.
    }
}

/**
 * @brief Lida com os eventos de entrada (mouse e teclado) do jogo.
 *
 * Direciona os eventos para a tela ou componente apropriado com base no estado atual do jogo.
 * Gerencia a navegação entre as telas (Menu, Jogo, Game Over, Ranking, Configurações).
 *
 * @param ev Referência ao evento do Allegro a ser processado.
 */
void GameEngine::handleInput(ALLEGRO_EVENT& ev) {
    // Se o evento for um clique do mouse.
    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        // No estado de Menu:
        if (estadoAtual == MENU) {
            menu->onClick(ev.mouse.x, ev.mouse.y); // Passa o clique para o objeto Menu.
            int acao = menu->getSelectedAction(); // Obtém a ação selecionada no menu.

            if (acao == 1) { // Ação "Jogar"
                if (menu->getApelido().empty()) {
                    menu->displayWarning("Digite seu apelido para jogar!"); // Exibe alerta se o apelido estiver vazio.
                } else {
                    std::string nick = menu->getApelido();
                    currentPlayer = playerManager->buscar(nick); // Tenta encontrar o jogador existente.
                    if (!currentPlayer) {
                        playerManager->cadastrar(nick, nick); // Se não existir, cadastra um novo.
                        currentPlayer = playerManager->buscar(nick); // Busca novamente para obter o ponteiro.
                    }
                    if (currentPlayer) {
                        currentLevel = 0; // Reinicia o nível para o primeiro.
                        inLevelTransition = true; // Inicia a transição de nível.
                        transitionBlurTimer = 0.0f;
                        isTransitionBlurActive = false; // Desativa o blur inicialmente para começar fresco.
                        estadoAtual = INICIANDO_JOGO; // Muda para o estado de início do jogo.
                        menu->resetAction(); // Reseta a ação do menu para evitar cliques repetidos.
                        menu->setInputActive(false); // Desativa a entrada de texto do menu durante o jogo.
                    } else {
                        std::cerr << "Erro: Não foi possível obter o jogador para iniciar o jogo.\n";
                        menu->displayWarning("Erro ao criar jogador!");
                        menu->resetAction();
                        menu->setInputActive(false);
                    }
                }
            } else if (acao == 2) { // Ação "Ranking"
                std::string apelidoDigitado = menu->getApelido();
                // Define o jogador atual para a tela de ranking, se um apelido foi digitado.
                currentPlayer = apelidoDigitado.empty() ? nullptr : playerManager->buscar(apelidoDigitado);

                rankingScreen->resetState(); // Reseta o estado da tela de ranking.
                estadoAtual = RANKING; // Muda para o estado de ranking.
                menu->resetAction();
                menu->setInputActive(false);
            } else if (acao == 3) { // Ação "Sair"
                // Exibe uma mensagem de saída antes de fechar o jogo.
                al_set_target_backbuffer(display);
                al_clear_to_color(al_map_rgb(0,0,0));
                al_draw_text(fontlarge, al_map_rgb(255,255,255), screenWidth/2, screenHeight/2, ALLEGRO_ALIGN_CENTER, "Saindo do jogo...");
                al_flip_display();
                al_rest(1.0); // Espera um segundo para a mensagem ser lida.

                fecharJogo = true; // Define a flag para encerrar o loop principal.
                menu->resetAction();
                menu->setInputActive(false);
            } else if (acao == 4) { // Ação "Configurações"
                estadoAtual = CONFIG_SCREEN; // Muda para o estado da tela de configurações.
                menu->resetAction();
                menu->setInputActive(false);
            }
        } else if (estadoAtual == GAME_OVER) { // Se estiver na tela de Game Over.
            int acao = gameOverScreen->handleEvent(&ev); // Lida com o evento na tela de Game Over.
            if (acao == 1) { // Ação "Reiniciar"
                currentLevel = 0; // Reinicia o nível.
                inLevelTransition = false; // Garante que a transição não esteja ativa.
                isTransitionBlurActive = false;
                transitionBlurTimer = 0.0f;
                if (scenario) { delete scenario; scenario = nullptr; } // Deleta o cenário anterior.

                // Cria um novo cenário, reiniciando o jogo.
                scenario = new Scenario(backgroundsLevels[0], birdBmp, pipesLevels[0], fontlarge, screenWidth, screenHeight, somPoint, somDie);

                estadoAtual = JOGANDO; // Volta para o estado de jogo.
            } else if (acao == 2) { // Ação "Voltar ao Menu"
                estadoAtual = MENU; // Volta para o menu principal.
                menu->resetAction();
                menu->setInputActive(true); // Ativa a entrada de texto no menu novamente.
            }
        }
    }
    // Se o evento for uma tecla pressionada.
    else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        if (estadoAtual == JOGANDO) { // Se estiver jogando.
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { // Se a tecla ESC for pressionada.
                estadoAtual = MENU; // Volta para o menu.
                menu->resetAction();
                menu->setInputActive(true);
            } else if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) { // Se a tecla ESPAÇO for pressionada.
                if (scenario) {
                    scenario->getBird().flap(); // Faz o pássaro "voar".
                    if (somFlap) al_play_sample(somFlap, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); // Toca o som de "flap".
                }
            }
        } else if (estadoAtual == RANKING) { // Se estiver na tela de Ranking.
            int acaoRanking = rankingScreen->handleEvent(ev); // Lida com o evento na tela de ranking.
            if (acaoRanking == 1) { // Ação "Voltar"
                estadoAtual = MENU; // Volta para o menu.
                menu->resetAction();
                menu->setInputActive(true);
            }
        } else if (estadoAtual == CONFIG_SCREEN) { // Se estiver na tela de Configurações.
            int acaoConfig = configScreen->handleEvent(ev); // Lida com o evento na tela de configurações.
            if (acaoConfig == 1) { // Ação "Voltar"
                estadoAtual = MENU; // Volta para o menu.
                menu->resetAction();
                menu->setInputActive(true);
                configScreen->resetState(); // Reseta o estado da tela de configurações.
            }
        }
    }
    // Se o evento for um caractere digitado (para entrada de texto).
    else if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
        if (estadoAtual == MENU) {
            // Repassa o caractere digitado para o menu processar (para o campo de apelido).
            menu->onChar(ev.keyboard.unichar, ev.keyboard.keycode);
        }
    }
}

/**
 * @brief Atualiza o estado lógico do jogo.
 *
 * Este método é chamado a cada frame do jogo. Ele gerencia a lógica de cada estado
 * (Menu, Jogando, Game Over, Ranking, Configurações, Iniciar Jogo), incluindo
 * atualização de componentes, gerenciamento de músicas, transições de nível e
 * verificação de condições de fim de jogo.
 *
 * @param deltaTime O tempo decorrido desde a última atualização, em segundos.
 */
void GameEngine::update(double deltaTime) {
    // Ações diferentes para cada estado do jogo.
    switch (estadoAtual) {
        case MENU:
            // Garante que a música do menu esteja tocando e rebobina se necessário.
            if (musicaAtualTocando != musicaMenuRankingGameOver || !al_get_audio_stream_playing(musicaMenuRankingGameOver)) {
                stopCurrentMusic();
                al_rewind_audio_stream(musicaMenuRankingGameOver); // Volta a música para o início.
                playMusic(musicaMenuRankingGameOver);
            }
            menu->update(deltaTime); // Atualiza o estado do menu (ex: animação de botões).
            break;

        case INICIANDO_JOGO: {
            // Inicia a transição de blur se ainda não estiver ativa.
            if (!isTransitionBlurActive) {
                isTransitionBlurActive = true;
                transitionBlurTimer = 0.0f; // Reinicia o timer do blur.

                // Deleta e recria o cenário para garantir que ele esteja em seu estado inicial.
                if (scenario) { delete scenario; scenario = nullptr; }
                scenario = new Scenario(backgroundsLevels[0], birdBmp, pipesLevels[0], fontlarge, screenWidth, screenHeight, somPoint, somDie);
                
                stopCurrentMusic();
                // Inicia a música do primeiro nível.
                if (!musicLevels.empty()) {
                    al_rewind_audio_stream(musicLevels[0]);
                    playMusic(musicLevels[0]);
                } else {
                    std::cerr << "Erro: Nenhuma música de nível disponível para iniciar o jogo.\n";
                }
            }

            transitionBlurTimer += (float)deltaTime; // Atualiza o timer da transição.

            // Quando a duração da transição de blur for atingida.
            if (transitionBlurTimer >= TRANSITION_BLUR_DURATION) {
                estadoAtual = JOGANDO; // Muda para o estado de jogo ativo.
                isTransitionBlurActive = false; // Desativa o efeito de blur.
                transitionBlurTimer = 0.0f; // Reseta o timer.
                inLevelTransition = false; // Finaliza a transição de nível.
            }
            break;
        }

        case JOGANDO:
            if (inLevelTransition) { // Se estiver em transição de nível (entre níveis).
                if (!isTransitionBlurActive) {
                    isTransitionBlurActive = true;
                    transitionBlurTimer = 0.0f;
                }
                transitionBlurTimer += (float)deltaTime; // Aumenta o tempo da transição.
                if (transitionBlurTimer >= TRANSITION_BLUR_DURATION) { // Quando a transição termina.
                    currentLevel++; // Avança para o próximo nível.
                    // Garante que o nível não exceda o número de níveis disponíveis.
                    if (currentLevel >= (int)backgroundsLevels.size()) {
                        currentLevel = (int)backgroundsLevels.size() - 1; // Fica no último nível.
                    }

                    // Aumenta a velocidade do cenário e troca os assets para o novo nível.
                    scenario->increaseSpeedByPercent(10.0f);
                    scenario->changeBackgroundAndPipe(backgroundsLevels[currentLevel], pipesLevels[currentLevel]);
                    scenario->setCurrentLevel(currentLevel); // Atualiza o nível atual no cenário.

                    // Configura os pipes como infinitos apenas no último cenário (nível 6).
                    scenario->setInfinitePipes(currentLevel == 6);

                    stopCurrentMusic();
                    if (currentLevel < (int)musicLevels.size()) {
                         al_rewind_audio_stream(musicLevels[currentLevel]); // Rebobina a música do novo nível.
                         playMusic(musicLevels[currentLevel]); // Toca a música do novo nível.
                    } else {
                        std::cerr << "Erro: Nenhuma música para o nível " << currentLevel << ".\n";
                    }

                    inLevelTransition = false; // Finaliza a transição.
                    isTransitionBlurActive = false;
                    transitionBlurTimer = 0.0f;
                }
                break; // Sai do switch para não atualizar o jogo enquanto transiciona.
            }

            // Se não estiver em transição, gerencia a música do jogo ativo.
            if (currentLevel < (int)musicLevels.size()) {
                if (musicaAtualTocando != musicLevels[currentLevel] || !al_get_audio_stream_playing(musicLevels[currentLevel])) {
                    stopCurrentMusic();
                    al_rewind_audio_stream(musicLevels[currentLevel]);
                    playMusic(musicLevels[currentLevel]);
                }
            } else {
                std::cerr << "Erro: Índice de nível de música fora dos limites.\n";
            }
            
            if (scenario) {
                // Garante que os pipes sejam infinitos no último nível.
                scenario->setInfinitePipes(currentLevel == 6);

                scenario->update((float)deltaTime); // Atualiza a lógica do cenário do jogo.

                if (scenario->hasScoredPoint()) { // Se o jogador marcou um ponto.
                    if (somPoint) al_play_sample(somPoint, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); // Toca o som de ponto.
                    scenario->resetPointFlag(); // Reseta a flag de ponto.

                    int totalScore = scenario->getScore();
                    // Calcula o nível esperado com base na pontuação (15 pontos por nível).
                    int expectedLevel = totalScore / 15;

                    // Se o jogador deveria estar em um nível mais avançado e ainda há níveis para avançar.
                    if (expectedLevel > currentLevel && currentLevel + 1 < (int)backgroundsLevels.size()) {
                        inLevelTransition = true; // Inicia uma transição de nível.
                        transitionBlurTimer = 0.0f;
                        stopCurrentMusic();
                        if (somTransition) al_play_sample(somTransition, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); // Toca som de transição.
                    }
                }

                if (scenario->isGameOver()) { // Se o jogo acabou.
                    stopCurrentMusic(); // Para a música do jogo.

                    lastScore = scenario->getScore(); // Pega a pontuação final da partida.
                    // Obtém o recorde pessoal do jogador atual (ou 0 se não houver jogador).
                    lastRecordPessoal = currentPlayer ? currentPlayer->getMaiorPontuacao() : 0;

                    // Encontra o maior score geral entre todos os jogadores.
                    const auto& jogadores = playerManager->getJogadores();
                    int maiorScore = 0;
                    for (const auto& p : jogadores) {
                        if (p.getMaiorPontuacao() > maiorScore) {
                            maiorScore = p.getMaiorPontuacao();
                        }
                    }
                    lastRecordGeral = maiorScore;

                    // Verifica se o jogador bateu o recorde pessoal ou geral.
                    lastBateuRecordePessoal = (lastScore > lastRecordPessoal);
                    lastBateuRecordeGeral = (lastScore > lastRecordGeral);

                    if (currentPlayer) {
                        currentPlayer->adicionarPartida(lastScore); // Adiciona a partida ao histórico do jogador.
                        playerManager->salvar(); // Salva os dados atualizados dos jogadores.
                    }

                    estadoAtual = GAME_OVER; // Muda para o estado de Game Over.
                }

            } else { // Se o ponteiro para o cenário for nulo (indica um erro).
                std::cerr << "Erro: Scenario nulo no estado JOGANDO!\n";
                estadoAtual = MENU; // Volta para o menu para recuperação.
            }
            break;

        case GAME_OVER:
            // Garante que a música do menu/ranking/game over esteja tocando.
            if (musicaAtualTocando != musicaMenuRankingGameOver || !al_get_audio_stream_playing(musicaMenuRankingGameOver)) {
                stopCurrentMusic();
                al_rewind_audio_stream(musicaMenuRankingGameOver);
                playMusic(musicaMenuRankingGameOver);
            }
            // A tela de Game Over geralmente não precisa de 'update' complexo.
            break;

        case RANKING:
            // Garante que a música do menu/ranking/game over esteja tocando.
            if (musicaAtualTocando != musicaMenuRankingGameOver || !al_get_audio_stream_playing(musicaMenuRankingGameOver)) {
                stopCurrentMusic();
                al_rewind_audio_stream(musicaMenuRankingGameOver);
                playMusic(musicaMenuRankingGameOver);
            }
            rankingScreen->update(deltaTime); // Atualiza a tela de ranking (ex: scroll).
            break;

        case CONFIG_SCREEN:
            configScreen->update(deltaTime); // Atualiza a tela de configurações (ex: animações).
            break;
    }
}

/**
 * @brief Renderiza a tela do menu principal.
 * @param deltaTime O tempo decorrido, usado para possíveis animações de fundo no menu.
 */
void GameEngine::renderMenu(float deltaTime) {
    if (menu) menu->render();
}

/**
 * @brief Renderiza o cenário de jogo (gameplay).
 * Desenha o pássaro, os canos, o fundo e a pontuação durante o jogo ativo.
 * Também aplica o efeito de blur durante as transições de nível.
 */
void GameEngine::renderGame() {
    if (scenario) scenario->render();

    // Se o efeito de blur da transição estiver ativo, desenha um retângulo semi-transparente.
    if (isTransitionBlurActive) {
        float alpha = transitionBlurTimer / TRANSITION_BLUR_DURATION;
        // Limita o alpha para não exceder 1.0 ou ser negativo.
        if (alpha > 1.0f) alpha = 1.0f;
        if (alpha < 0.0f) alpha = 0.0f;
        al_draw_filled_rectangle(0, 0, screenWidth, screenHeight, al_map_rgba_f(0, 0, 0, alpha));
    }
}

/**
 * @brief Renderiza a tela de Game Over.
 * Exibe a pontuação final da partida, o recorde pessoal e o recorde geral,
 * além de informar se algum recorde foi batido.
 */
void GameEngine::renderGameOver() {
    if (gameOverScreen) {
        gameOverScreen->render(lastScore, lastRecordPessoal, lastRecordGeral, lastBateuRecordePessoal, lastBateuRecordeGeral);
    }
}

/**
 * @brief Renderiza a tela de Ranking.
 * Exibe a lista de jogadores e suas maiores pontuações, destacando o jogador atual.
 * @param deltaTime O tempo decorrido, usado para possíveis animações na tela de ranking.
 */
void GameEngine::renderRanking(float deltaTime) {
    if (rankingScreen) {
        rankingScreen->render(currentPlayer, deltaTime);
    }
}

/**
 * @brief Renderiza a tela de Configurações.
 * @param deltaTime O tempo decorrido, usado para possíveis animações na tela de configurações.
 */
void GameEngine::renderConfigScreen(float deltaTime) {
    if (configScreen) {
        configScreen->render();
    }
}

/**
 * @brief O loop principal de execução do jogo.
 *
 * Este método orquestra a inicialização do Allegro, o carregamento dos assets,
 * e o ciclo de vida do jogo, que inclui:
 * 1. Processamento de eventos (entrada do usuário, timer, fechamento da janela).
 * 2. Atualização da lógica do jogo (chamando `update`).
 * 3. Renderização dos elementos na tela (chamando `render` apropriado).
 * O loop continua até que a flag `fecharJogo` seja definida como true.
 */
void GameEngine::run() {
    initializeAllegroAddons(); // Inicializa o Allegro e seus add-ons.
    loadGameAssets();         // Carrega todos os recursos do jogo.

    al_start_timer(timer); // Inicia o timer para controlar a taxa de quadros (FPS).
    bool redraw = false;   // Flag para indicar se a tela precisa ser redesenhada.

    // Loop principal do jogo. Continua executando enquanto a flag 'fecharJogo' for falsa.
    while (!fecharJogo) {
        ALLEGRO_EVENT ev;
        // Espera por um evento na fila, o que economiza CPU quando não há eventos.
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            // Se for um evento do timer, é hora de atualizar a lógica do jogo.
            update(1.0 / 60.0); // Atualiza o jogo com base em um delta time de 60 FPS.
            redraw = true;     // Marca a flag para redesenhar a tela.
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // Se o usuário clicou no botão de fechar a janela.
            fecharJogo = true; // Define a flag para sair do loop principal.
        } else {
            // Qualquer outro evento (teclado, mouse) é passado para o handler de input.
            handleInput(ev);
        }

        // Só redesenha a tela se a flag 'redraw' estiver true e a fila de eventos estiver vazia.
        // Isso evita redesenhar múltiplas vezes para o mesmo frame, otimizando a performance.
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false; // Reseta a flag após o redesenho.
            al_set_target_bitmap(renderTarget); // Redireciona o desenho para o bitmap temporário.
            al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpa o buffer temporário com preto.

            // Chama a função de renderização correta baseado no estado atual do jogo.
            switch (estadoAtual) {
                case MENU:
                    renderMenu(1.0 / 60.0);
                    break;

                case INICIANDO_JOGO: {
                    // Renderiza o cenário ou o menu se o cenário ainda não foi criado.
                    if (scenario) {
                        scenario->render();
                    } else {
                        renderMenu(1.0f / 60.0f);
                    }

                    // Desenha o efeito de fade-out/in da transição.
                    float alpha = 1.0f - (transitionBlurTimer / TRANSITION_BLUR_DURATION);
                    // Garante que o valor de alpha esteja entre 0.0 e 1.0.
                    if (alpha < 0.0f) alpha = 0.0f;
                    if (alpha > 1.0f) alpha = 1.0f;
                    al_draw_filled_rectangle(0, 0, screenWidth, screenHeight, al_map_rgba_f(0, 0, 0, alpha));
                    break;
                }

                case JOGANDO:
                    renderGame();
                    break;
                case GAME_OVER:
                    renderGameOver();
                    break;
                case RANKING:
                    renderRanking(1.0 / 60.0);
                    break;
                case CONFIG_SCREEN:
                    renderConfigScreen(1.0 / 60.0);
                    break;
            }

            al_set_target_backbuffer(display); // Volta a renderizar para o display principal.
            // Desenha o conteúdo do bitmap temporário na tela, aplicando escalonamento se necessário.
            al_draw_scaled_bitmap(renderTarget, 0, 0,
                                  al_get_bitmap_width(renderTarget),
                                  al_get_bitmap_height(renderTarget),
                                  0, 0, screenWidth, screenHeight, 0);
            al_flip_display(); // Mostra o que foi desenhado na tela.
        }
    }
}