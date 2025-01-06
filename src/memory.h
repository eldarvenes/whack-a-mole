#ifndef MEMORY_H
#define MEMORY_H

#include "abstract_game.h"
#include "config.h"
#include "buzzer.h"
#include "SevenSegmentDisplay.h"

class Memory : public AbstractGame {
public:
    Memory(Buzzer& buzzer, SevenSegmentDisplay& display);

    // Overriding AbstractGame methods
    void init() override;       // Initialiser spillet
    void update() override;     // Oppdater spillet
    void endGame() override;    // Avslutt spillet
    void flashSequence(int upToStep); // Vis sekvensen så langt
    GameState getState() const override; // Hent spilltilstanden
    void waitForRestart() override;

private:
    Buzzer& buzzer;
    SevenSegmentDisplay& display;
    GameState gameState;        // Nåværende tilstand i spillet (RUNNING/GAME_OVER)
    int sequence[50];           // Maks 10 trinn i sekvensen
    int currentStep;            // Nåværende steg i sekvensen som vises
    int playerStep;             // Spillernes fremgang i å gjette sekvensen
    int sequenceLength;         // Lengden på sekvensen som vises
    bool waitingForInput;       // Om spillet venter på spillerens input
    unsigned long lastStepTime; // Tidsstempel for sist oppdatert steg
    const unsigned long stepDelay = 500; // Forsinkelse mellom sekvenslys

    void generateSequence();    // Generer en tilfeldig sekvens
    void showSequence();        // Vis sekvensen til spilleren
    void checkInput();          // Sjekk om spilleren gjettet riktig
};

#endif // MEMORY_H