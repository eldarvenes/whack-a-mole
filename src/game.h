#ifndef GAME_H
#define GAME_H

#include "mole.h"
#include "config.h"

class Game {
public:
    enum GameState {
        RUNNING,
        GAME_OVER
    };
    Game(int buzzerPin);
    void init();
    void update();
    int getScore() const;
    void endGame();
    void playGameOverMelody();
    void waitForRestart();
    GameState getState() const;

private:
    GameState gameState;
    Mole* moles[NUM_MOLES];
    unsigned long moleDisplayTime[NUM_MOLES]; // Holder styr på visningstiden for hver mole
    unsigned long lastMoleTime;
    int score;
    int buzzerPin;
    bool buzzerActive;            
    unsigned long buzzerOffTime;
    int lives;
};

#endif // GAME_H