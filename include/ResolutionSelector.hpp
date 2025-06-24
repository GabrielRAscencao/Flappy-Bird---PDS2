/**
 * @file ResolutionSelector.hpp
 * @brief ResolutionSelectorheader do projeto Traveling Dragon.
 */

#ifndef RESOLUTION_SELECTOR_HPP
#define RESOLUTION_SELECTOR_HPP

#include <string>     // Para usar std::string
#include <optional>   // Para usar std::optional, que permite retornar um valor ou a ausência dele

/**
 * @brief Enumeração para os diferentes modos de exibição da janela do jogo.
 */
enum WindowMode {
    WINDOWED = 0,   ///< @brief Janela com bordas, redimensionável (padrão).
    FULLSCREEN = 1, ///< @brief Modo tela cheia exclusivo (ocupa a tela inteira, geralmente com resolução fixa).
    BORDERLESS = 2  ///< @brief Janela sem bordas que ocupa a tela inteira, comportando-se como uma janela maximizada.
};

/**
 * @brief Estrutura que armazena as configurações de resolução e modo de janela.
 */
struct ResolutionConfig {
    int width;      ///< @brief Largura da resolução desejada (em pixels).
    int height;     ///< @brief Altura da resolução desejada (em pixels).
    WindowMode mode; ///< @brief O modo de exibição da janela (WINDOWED, FULLSCREEN, BORDERLESS).
};

/**
 * @brief Exibe uma interface para o usuário selecionar a resolução e o modo de janela.
 *
 * Esta função interage com o usuário para que ele escolha as configurações de vídeo
 * preferidas antes de iniciar o jogo principal.
 *
 * @return Um `std::optional<ResolutionConfig>`. Se o usuário selecionar uma configuração,
 * retorna um objeto `ResolutionConfig`. Se o usuário cancelar ou fechar o seletor,
 * retorna `std::nullopt` (indicando que nenhuma configuração foi selecionada).
 */
std::optional<ResolutionConfig> showResolutionSelector();

#endif