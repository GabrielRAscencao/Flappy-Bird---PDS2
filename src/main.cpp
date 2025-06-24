/**
 * @file main.cpp
 * @brief mainimplementação do projeto Traveling Dragon.
 */


#include "GameEngine.hpp" // Inclui a classe principal do motor do jogo
#include "ResolutionSelector.hpp" // Inclui a função para seleção de resolução
#include <optional> // Para usar std::optional, que lida com valores que podem ou não estar presentes
#include <iostream> // Para saída de console

/**
 * @brief Função principal do programa.
 * @return 0 se o programa finalizar com sucesso, outro valor em caso de erro.
 */
int main() {
    // Inicializa a biblioteca Allegro. Isso deve ser feito antes de qualquer função Allegro
    al_init(); 

    // Chama a função para exibir o seletor de resolução.
    // std::optional é usado pois o usuário pode fechar a janela sem selecionar uma resolução.
    auto res = showResolutionSelector();

    // Verifica se o usuário fechou a janela ou apertou ESC no seletor de resolução
    if (!res.has_value()) {
        std::cout << "Resolução não selecionada. Encerrando o jogo.\n";
        return 0; // Sai do programa
    }

    // Se uma resolução foi selecionada, cria uma instância do GameEngine
    // com as dimensões e o modo de exibição escolhidos.
    GameEngine engine(res->width, res->height, res->mode);
    // Inicia o loop principal do jogo.
    engine.run();

    return 0; // Retorna 0 indicando que o programa terminou sem erros
}