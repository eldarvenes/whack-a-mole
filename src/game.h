#ifndef GAME_H
#define GAME_H

#include "mole.h"
#include "config.h"

class Game {
public:
    Game(int buzzerPin);
    void init();
    void update();
    int getScore() const;

private:
    Mole* moles[NUM_MOLES];
    unsigned long moleDisplayTime[NUM_MOLES]; // Holder styr på visningstiden for hver mole
    unsigned long lastMoleTime;
    int score;
    int buzzerPin;
    bool buzzerActive;            
    unsigned long buzzerOffTime; 
};

#endif // GAME_H