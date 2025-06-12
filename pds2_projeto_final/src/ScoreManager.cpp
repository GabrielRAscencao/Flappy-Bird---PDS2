#include "ScoreManager.hpp"

ScoreManager::ScoreManager() {
    reset();
}

void ScoreManager::reset() {
    score = 0;
}

void ScoreManager::increment() {
    score++;
}

int ScoreManager::getScore() const {
    return score;
}

