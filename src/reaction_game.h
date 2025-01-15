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
    void playCoolStartSound(Buzzer& buzzer); // Spill en kul startlyd
    bool waitingForNextRound = false; // Indikerer om vi venter på neste runde
    unsigned long nextRoundStartTime = 0; // Tidspunkt for når neste runde starter
    void waitForNextRoundStart(); // Vent på neste runde


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
    unsigned long lastReactionDisplayTime = 0; // Tidspunkt for siste visning av reaksjonstider
    int player1Mole;
    int player2Mole;
    
};

#endif // REACTION_GAME_H