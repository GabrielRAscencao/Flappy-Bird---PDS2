/**
 * @file Utils.hpp
 * @brief Utils para save do projeto Traveling Dragon.
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <windows.h>
#include <filesystem>

/**
 * @brief Retorna o diretório onde o executável está localizado.
 */
inline std::string getExecutableDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path().string();
}

/**
 * @brief Retorna o caminho completo para o arquivo de jogadores.
 */
inline std::string getSaveFilePath() {
    return getExecutableDirectory() + "\\data\\players.txt";
}

#endif // UTILS_HPP
