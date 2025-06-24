/**
 * @file Menu.cpp
 * @brief Menuimplementação do projeto Traveling Dragon.
 */


#include "Menu.hpp"
#include <allegro5/allegro_primitives.h> // Para desenho de formas primitivas
#include <allegro5/allegro_ttf.h> // Para fontes TTF
#include <allegro5/allegro_font.h> // Para manipulação de fontes
#include <algorithm> // Para std::clamp e std::max
#include <cctype> // Para isalnum
#include <iostream> // Para saída de console

/// @brief Duração em segundos que as mensagens de aviso ficam visíveis.
const double Menu::WARNING_DURATION = 3.0;

/**
 * @brief Construtor da classe Menu.
 * @param fontNormal Ponteiro para a fonte normal do jogo.
 * @param fontGrande Ponteiro para a fonte maior do jogo (para botões, título).
 * @param bg Ponteiro para o bitmap de fundo do menu.
 */
Menu::Menu(ALLEGRO_FONT* fontNormal, ALLEGRO_FONT* fontGrande, ALLEGRO_BITMAP* bg)
    : font(fontNormal), fontlarge(fontGrande), background(bg), apelido(""), selectedAction(0),
      inputActive(false), cursorVisible(true), cursorTimer(0.0), scroll(0.0f),
      currentWarningMessage(""), warningDisplayTimer(0.0), somHover(nullptr)
{
    // Obtém as dimensões atuais da tela para cálculo de escala e posicionamento
    SCREEN_W = static_cast<float>(al_get_display_width(al_get_current_display()));
    SCREEN_H = static_cast<float>(al_get_display_height(al_get_current_display()));

    // Define as dimensões de design para cálculo dos fatores de escala
    const float DESIGN_W = 1280.0f;
    const float DESIGN_H = 720.0f;
    scale_x = SCREEN_W / DESIGN_W;
    scale_y = SCREEN_H / DESIGN_H;

    buttonFont = fontlarge; // A fonte dos botões será a fonte maior

    buttonWidth = 300.0f * scale_x; // Largura padrão dos botões, escalada
    buttonHeight = 60.0f * scale_y; // Altura padrão dos botões, escalada

    // Calcula a posição Y base para os botões (inferior da tela)
    float baseY = SCREEN_H - 150.0f * scale_y;
    // Calcula o espaçamento entre os botões
    float spacing = 1.1f * buttonHeight;

    float centerX = SCREEN_W / 2.0f; // Centro horizontal da tela

    // Define as posições (X, Y) centrais de cada botão
    buttonPositions = {
        {centerX, baseY}, // Botão "Sair" (inferior)
        {centerX, baseY - spacing}, // Botão "Configurações"
        {centerX, baseY - spacing * 2}, // Botão "Ranking"
        {centerX, baseY - spacing * 3} // Botão "Jogar" (superior)
    };

    // Inicializa o vetor de estados anteriores de hover dos botões como falsos
    previousHoverStates = std::vector<bool>(4, false);
}

/**
 * @brief Atualiza o estado do menu a cada frame.
 * Gerencia o cursor do campo de apelido e o temporizador de mensagens de aviso.
 * @param deltaTime Tempo decorrido desde a última atualização.
 */
void Menu::update(double deltaTime) {
    updateCursor(deltaTime); // Atualiza o estado do cursor

    // Gerencia o temporizador da mensagem de aviso
    if (warningDisplayTimer > 0) {
        warningDisplayTimer -= deltaTime;
        if (warningDisplayTimer <= 0) {
            currentWarningMessage = ""; // Limpa a mensagem quando o tempo acaba
        }
    }
}

/**
 * @brief Atualiza o estado do cursor de digitação.
 * Faz o cursor piscar se o campo de entrada estiver ativo.
 * @param deltaTime Tempo decorrido desde a última atualização.
 */
void Menu::updateCursor(double deltaTime) {
    if (!inputActive) {
        cursorVisible = false; // Cursor invisível se o input não estiver ativo
        cursorTimer = 0.0; // Reseta o timer
        return;
    }
    cursorTimer += deltaTime;
    if (cursorTimer >= 0.5) { // Troca a visibilidade a cada 0.5 segundos
        cursorVisible = !cursorVisible;
        cursorTimer = 0.0;
    }
}

/**
 * @brief Renderiza todos os elementos do menu na tela.
 * Inclui o fundo, o campo de entrada de apelido e os botões.
 */
void Menu::render() {
    // Desenha o fundo do menu, escalando para preencher a tela
    if (background) {
        int bg_w = al_get_bitmap_width(background);
        int bg_h = al_get_bitmap_height(background);
        float bg_scale = std::max(SCREEN_W / (float)bg_w, SCREEN_H / (float)bg_h);
        al_draw_scaled_bitmap(background, 0, 0, bg_w, bg_h, 0, 0, bg_w * bg_scale, bg_h * bg_scale, 0);
    } else {
        // Fallback: se não houver background, pinta a tela de azul
        al_clear_to_color(al_map_rgb(50, 50, 150));
    }

    const float inputY = SCREEN_H * 0.33f; // Posição Y do campo de apelido
    const float MIN_INPUT_WIDTH = 50.0f * scale_x; // Largura mínima do campo
    const float MAX_INPUT_WIDTH = 300.0f * scale_x; // Largura máxima do campo
    const float PADDING_X = 15.0f * scale_x; // Preenchimento horizontal do texto no campo
    const float PADDING_Y = 10.0f * scale_y; // Preenchimento vertical do texto no campo

    // O texto a ser medido para ajustar a largura do campo de entrada
    std::string textToMeasure = apelido.empty() && !inputActive ? "Digite seu apelido" : apelido;
    float currentTextWidth = al_get_text_width(font, textToMeasure.c_str());
    // A largura da caixa do input se ajusta ao texto, mas com limites
    float boxW = std::clamp(currentTextWidth + PADDING_X * 2, MIN_INPUT_WIDTH, MAX_INPUT_WIDTH);
    float boxH = al_get_font_line_height(font) + PADDING_Y * 2; // Altura da caixa do input
    float boxX = SCREEN_W / 2.0f - boxW / 2.0f; // Posição X centralizada da caixa
    float boxY = inputY; // Posição Y da caixa

    // Desenha o fundo preto translúcido do campo de apelido
    al_draw_filled_rounded_rectangle(boxX, boxY, boxX + boxW, boxY + boxH, 10 * scale_x, 10 * scale_y, al_map_rgba_f(0, 0, 0, 0.5));

    // Desenha o contorno preto opaco do campo de apelido
    al_draw_rounded_rectangle(boxX, boxY, boxX + boxW, boxY + boxH, 10 * scale_x, 10 * scale_y, al_map_rgb(0, 0, 0), 2 * scale_x);

    float textY = boxY + (boxH - al_get_font_line_height(font)) / 2.0f; // Posição Y do texto dentro do campo

    // Exibe o texto placeholder ou o apelido digitado
    if (apelido.empty() && !inputActive) {
        al_draw_text(font, al_map_rgb(255, 255, 255), boxX + PADDING_X, textY, 0, "Digite seu apelido");
    } else {
        al_draw_text(font, al_map_rgb(255, 255, 255), boxX + PADDING_X, textY, 0, apelido.c_str());

        // Desenha o cursor piscando se o campo estiver ativo
        if (inputActive && cursorVisible) {
            float cursorX = boxX + PADDING_X + al_get_text_width(font, apelido.c_str());
            float cursorY1 = boxY + PADDING_Y / 2;
            float cursorY2 = boxY + boxH - PADDING_Y / 2;
            al_draw_filled_rectangle(cursorX, cursorY1, cursorX + 2 * scale_x, cursorY2, al_map_rgb(255, 255, 255));
        }
    }

    ALLEGRO_MOUSE_STATE mouse;
    al_get_mouse_state(&mouse); // Obtém o estado atual do mouse

    // Desenha cada botão do menu
    for (int i = 0; i < 4; ++i) {
        float bx = buttonPositions[i][0]; // Posição X do botão
        float by = buttonPositions[i][1]; // Posição Y do botão
        float halfW = buttonWidth / 2.0f; // Metade da largura do botão
        float halfH = buttonHeight / 2.0f; // Metade da altura do botão

        // Verifica se o mouse está sobre o botão atual
        bool hover = (mouse.x >= bx - halfW && mouse.x <= bx + halfW && mouse.y >= by - halfH && mouse.y <= by + halfH);

        // Toca o som de hover se o estado de hover mudou de falso para verdadeiro
        if (hover && !previousHoverStates[i]) {
            if (somHover) al_play_sample(somHover, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
        }
        previousHoverStates[i] = hover; // Atualiza o estado anterior de hover

        // Desenha o texto do botão com efeitos de hover
        drawButtonText(bx, by, getButtonLabel(i), hover);
    }

    // Se houver uma mensagem de aviso ativa, a exibe
    if (!currentWarningMessage.empty() && warningDisplayTimer > 0) {
        ALLEGRO_COLOR warningColor = al_map_rgb(255, 0, 0); // Cor vermelha para o aviso
        float warningTextWidth = al_get_text_width(font, currentWarningMessage.c_str());
        float warningBoxW = warningTextWidth + PADDING_X * 2;
        float warningBoxH = al_get_font_line_height(font) + PADDING_Y * 2;
        float warningBoxX = SCREEN_W / 2.0f - warningBoxW / 2.0f;
        float warningBoxY = boxY + boxH + 10 * scale_y; // Posição abaixo do campo de apelido

        // Desenha o fundo e o contorno da caixa de aviso
        al_draw_filled_rounded_rectangle(warningBoxX, warningBoxY, warningBoxX + warningBoxW, warningBoxY + warningBoxH,
                                         5 * scale_x, 5 * scale_y, al_map_rgba(0, 0, 0, 180));
        al_draw_rounded_rectangle(warningBoxX, warningBoxY, warningBoxX + warningBoxW, warningBoxY + warningBoxH,
                                  5 * scale_x, 5 * scale_y, warningColor, 2 * scale_x);
        // Desenha a mensagem de aviso
        al_draw_text(font, warningColor, SCREEN_W / 2.0f, warningBoxY + PADDING_Y, ALLEGRO_ALIGN_CENTER, currentWarningMessage.c_str());
    }
}

/**
 * @brief Desenha o texto de um botão com efeitos de sombra e hover.
 * @param x Posição X central do texto.
 * @param y Posição Y central do texto.
 * @param text O texto a ser exibido.
 * @param hover True se o mouse estiver sobre o botão, false caso contrário.
 */
void Menu::drawButtonText(float x, float y, const std::string& text, bool hover) {
    ALLEGRO_COLOR outline = al_map_rgb(0, 0, 0); // Cor do contorno/sombra
    // Cor do texto: branco com 60% de opacidade no hover
    ALLEGRO_COLOR textColor = al_map_rgba(255, 255, 255, hover ? 153 : 255); 

    const float PADDING_X = 20.0f * scale_x; // Preenchimento horizontal
    const float PADDING_Y = 10.0f * scale_y; // Preenchimento vertical

    float textWidth = al_get_text_width(buttonFont, text.c_str());
    float textHeight = al_get_font_line_height(buttonFont);

    float rectWidth = textWidth + 2 * PADDING_X;
    float rectHeight = textHeight + 2 * PADDING_Y;

    // Calcula as coordenadas do retângulo de fundo (para o efeito de hover)
    float rectLeft = x - rectWidth / 2.0f;
    float rectTop = y - rectHeight / 2.0f + 18.0f * scale_y;    // Ajusta a posição Y do retângulo
    float rectRight = x + rectWidth / 2.0f;
    float rectBottom = y + rectHeight / 2.0f + 20.0f * scale_y; // Ajusta a posição Y do retângulo

    if (hover) {
        // Desenha o retângulo de fundo translúcido no hover
        ALLEGRO_COLOR bgColor = al_map_rgba_f(0, 0, 0, 0.4); // Preto com 40% de opacidade
        al_draw_filled_rounded_rectangle(rectLeft, rectTop, rectRight, rectBottom, 10 * scale_x, 10 * scale_y, bgColor);
    }

    // Desenha o contorno/sombra do texto (múltiplas cópias deslocadas)
    for (int dx = -3; dx <= 3; ++dx) {
        for (int dy = -3; dy <= 3; ++dy) {
            if (dx == 0 && dy == 0) continue; // Pula o centro
            al_draw_text(buttonFont, outline, x + dx, y + dy, ALLEGRO_ALIGN_CENTER, text.c_str());
        }
    }

    // Desenha o texto principal do botão
    al_draw_text(buttonFont, textColor, x, y, ALLEGRO_ALIGN_CENTER, text.c_str());
}

/**
 * @brief Retorna o rótulo (texto) de um botão do menu com base no seu índice.
 * @param index O índice do botão (0 a 3).
 * @return O texto correspondente ao botão.
 */
std::string Menu::getButtonLabel(int index) const {
    switch(index) {
        case 0: return "Sair";
        case 1: return "Configurações";
        case 2: return "Ranking";
        case 3: return "Jogar";
        default: return ""; // Retorna vazio para índices inválidos
    }
}

/**
 * @brief Processa um clique do mouse na tela do menu.
 * Determina qual botão ou campo de entrada foi clicado e define a ação correspondente.
 * @param mx Coordenada X do mouse.
 * @param my Coordenada Y do mouse.
 */
void Menu::onClick(int mx, int my) {
    float halfW = buttonWidth / 2.0f;
    float halfH = buttonHeight / 2.0f;

    // Verifica se algum botão foi clicado
    for (int i = 0; i < 4; ++i) {
        float bx = buttonPositions[i][0];
        float by = buttonPositions[i][1];
        // Verifica se as coordenadas do mouse estão dentro da área do botão
        if (mx >= bx - halfW && mx <= bx + halfW && my >= by - halfH && my <= by + halfH) {
            switch(i) {
                case 3: // Botão "Jogar"
                    if (apelido.empty()) {
                        displayWarning("Apelido nao pode ser vazio!"); // Exibe aviso se o apelido estiver vazio
                    } else {
                        trimApelido(); // Remove espaços em branco do final do apelido antes de confirmar
                        selectedAction = 1; // Sinaliza para iniciar o jogo
                    }
                    return; // Sai da função após processar o clique
                case 2: selectedAction = 2; return; // Sinaliza para ir para a tela de Ranking
                case 1: selectedAction = 4; return; // Sinaliza para ir para a tela de Configurações
                case 0: selectedAction = 3; return; // Sinaliza para sair do jogo
            }
        }
    }

    // Define as dimensões do campo de apelido novamente para verificar o clique
    const float inputY = SCREEN_H * 0.33f;
    const float MIN_INPUT_WIDTH = 50.0f * scale_x;
    const float MAX_INPUT_WIDTH = 300.0f * scale_x;
    const float PADDING_X = 15.0f * scale_x;
    const float PADDING_Y = 10.0f * scale_y;

    std::string textToMeasure = apelido.empty() && !inputActive ? "Digite seu apelido" : apelido;
    float currentTextWidth = al_get_text_width(font, textToMeasure.c_str());
    float boxW = std::clamp(currentTextWidth + PADDING_X * 2, MIN_INPUT_WIDTH, MAX_INPUT_WIDTH);
    float boxH = al_get_font_line_height(font) + PADDING_Y * 2;
    float boxX = SCREEN_W / 2.0f - boxW / 2.0f;
    float boxY = inputY;

    // Verifica se o campo de apelido foi clicado
    if (mx >= boxX && mx <= boxX + boxW && my >= boxY && my <= boxY + boxH) {
        inputActive = true; // Ativa a entrada de texto
        currentWarningMessage = ""; // Limpa qualquer aviso existente
        warningDisplayTimer = 0.0; // Reseta o temporizador de aviso
        return;
    }

    inputActive = false; // Desativa a entrada de texto se clicou fora dos botões e do campo
}

/**
 * @brief Processa a entrada de caracteres do teclado no campo de apelido.
 * @param unichar O caractere Unicode digitado.
 * @param keycode O código da tecla Allegro pressionada.
 */
void Menu::onChar(int unichar, int keycode) {
    if (!inputActive) return; // Ignora se o campo de entrada não estiver ativo

    currentWarningMessage = ""; // Limpa qualquer aviso ao digitar
    warningDisplayTimer = 0.0; // Reseta o temporizador de aviso

    if (keycode == ALLEGRO_KEY_BACKSPACE) {
        if (!apelido.empty()) {
            apelido.pop_back(); // Remove o último caractere
        }
    } else if (keycode == ALLEGRO_KEY_ENTER) {
        inputActive = false; // Desativa a entrada de texto ao pressionar Enter
        trimApelido(); // Remove espaços em branco do final

        if (apelido.empty()) {
            displayWarning("Apelido nao pode ser vazio!"); // Exibe aviso se o apelido estiver vazio
        } else {
            selectedAction = 1; // Sinaliza para iniciar o jogo (como se o botão Jogar fosse clicado)
        }
    } else {
        // Limita o tamanho do apelido
        if (apelido.size() >= 15) return;
        // Permite apenas caracteres alfanuméricos, underscore, hífen e espaço
        if (!(isalnum(unichar) || unichar == '_' || unichar == '-' || unichar == ' ')) return;

        // Regras para o caractere de espaço:
        // - Não permite espaço no começo do apelido
        // - Não permite espaços repetidos
        if (unichar == ' ') {
            if (apelido.empty()) return; // Não permite começar com espaço
            if (apelido.back() == ' ') return; // Não permite espaços duplos
        }

        apelido.push_back(static_cast<char>(unichar)); // Adiciona o caractere ao apelido
    }
}

/**
 * @brief Reseta a ação selecionada no menu para "nenhuma ação".
 */
void Menu::resetAction() { 
    selectedAction = 0; // Define a ação como 0 (nenhuma ação)
}

/**
 * @brief Retorna o apelido digitado pelo jogador.
 * @return O apelido atual.
 */
std::string Menu::getApelido() const { 
    return apelido; 
}

/**
 * @brief Retorna a ação selecionada pelo jogador no menu.
 * @return Um inteiro representando a ação (ex: 1 para jogar, 2 para ranking, etc.).
 */
int Menu::getSelectedAction() const { 
    return selectedAction; 
}

/**
 * @brief Exibe uma mensagem de aviso na tela do menu por um tempo definido.
 * @param message A string da mensagem de aviso.
 */
void Menu::displayWarning(const std::string& message) {
    currentWarningMessage = message; // Define a mensagem
    warningDisplayTimer = WARNING_DURATION; // Inicia o temporizador
}

/**
 * @brief Define o estado de ativação do campo de entrada de apelido.
 * @param active True para ativar, false para desativar.
 */
void Menu::setInputActive(bool active) {
    inputActive = active;
}

/**
 * @brief Verifica se o campo de entrada de apelido está ativo.
 * @return True se ativo, false caso contrário.
 */
bool Menu::isInputActive() const {
    return inputActive;
}

/**
 * @brief Remove espaços em branco do final do apelido.
 * Garante que o apelido não termine com espaços.
 */
void Menu::trimApelido() {
    // Loop para remover espaços em branco do final da string
    while (!apelido.empty() && apelido.back() == ' ') {
        apelido.pop_back();
    }
}

/**
 * @brief Define o som que será reproduzido quando um botão do menu é hover.
 * @param som Ponteiro para o ALLEGRO_SAMPLE do som de hover.
 */
void Menu::setHoverSound(ALLEGRO_SAMPLE* som) {
    somHover = som;
}