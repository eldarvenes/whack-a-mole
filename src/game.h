#ifndef GAME_H
#define GAME_H

#include "abstract_game.h"
#include "mole.h"
#include "config.h"
#include "buzzer.h"
#include "SevenSegmentDisplay.h"

class Game : public AbstractGame {
public:
    Game(Buzzer& buzzer, SevenSegmentDisplay& display);
    //Game(int buzzerPin);
    void init() override;       // Initialiser spillet
    void update() override;     // Oppdater spillet
    void endGame() override;    // Avslutt spillet
    GameState getState() const override; // Hent spilltilstanden

    int getScore() const;

private:
    Buzzer& buzzer;
    SevenSegmentDisplay& display;
    GameState gameState;        // Nåværende tilstand i spillet (RUNNING/GAME_OVER)
    Mole* moles[NUM_MOLES];
    unsigned long moleDisplayTime[NUM_MOLES]; // Holder styr på visningstiden for hver mole
    unsigned long lastMoleTime;
    int score;
    int buzzerPin;
    bool buzzerActive;            
    unsigned long buzzerOffTime;
    int lives;

    void playGameOverMelody();  // Spill melodien for game over
    void waitForRestart();      // Vent på at spilleren starter spillet på nytt
};

#endif // GAME_H