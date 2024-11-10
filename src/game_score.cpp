#include "game_score.h"

GameScore::GameScore() : score(0) {} 

void GameScore::incrementScore() {
    score++;
}

int GameScore::getScore() const {
    return score;
}