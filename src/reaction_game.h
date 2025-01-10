#ifndef REACTION_GAME_H
#define REACTION_GAME_H

#include "abstract_game.h"
#include "Buzzer.h"
#include "SevenSegmentDisplay.h"
#include "config.h"

class ReactionGame : public AbstractGame {
enum GameMode {
    ONE_PLAYER,
    TWO_PLAYER
};

public:
    ReactionGame(Buzzer& buzzer, SevenSegmentDisplay& display); // Konstruktør
    void init() override;                                       // Initialiser spillet
    void update() override;                                     // Oppdater spillstatus
    void waitForRestart() override;                             // Vent på restart
    AbstractGame::GameState getState() const override;          // Hent spillstatus

private:
    void startRound();       // Start en ny runde
    void endGame();          // Avslutt spillet

    Buzzer& buzzer;          // Buzzer for lydeffekter
    SevenSegmentDisplay& display; // 7-segment display for poengvisning
    AbstractGame::GameState gameState;
    GameMode gameMode;
    bool lightOn;            // Indikerer om et lys er på
    int activeMole;          // Holder styr på aktiv knapp/lys
    unsigned long startTime; // Starttidspunkt for en runde
    unsigned long reactionTime; // Reaksjonstid for spilleren
    bool checkExitCondition(); // Sjekk om spillet skal avsluttes
    private:
    unsigned long reactionTimes[5]; // Lagre reaksjonstidene
    int roundCount;                 // Tell antall runder
    void displayAverageReactionTime();
    void displayAverageReactionTimesTwoPlayer();
    unsigned long reactionTimesPlayer1[5];
    unsigned long reactionTimesPlayer2[5];
    int roundCountPlayer1;
    int roundCountPlayer2;
    bool player1LightOn;
    bool player2LightOn;
    unsigned long player1StartTime;
    unsigned long player2StartTime;
    
};

#endif // REACTION_GAME_H