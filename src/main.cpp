#include "config.h"
#include "game.h"
#include "Memory.h"
#include "reaction_game.h"
#include "Buzzer.h"
#include "SevenSegmentDisplay.h"
#include <Arduino.h>

Buzzer buzzer(3);       // Buzzer for lydeffekter
SevenSegmentDisplay display(dataPin, clkPin, csPin);

// Opprett spillinstanser
Game whackAMoleGame(buzzer, display); // Spill 1
Memory memoryGame(buzzer, display);      // Spill 2 (Memory-spillet)
ReactionGame reactionGame(buzzer, display); // Spill 3 (Reaksjonsspill)


// Spilltyper
enum GameType { WHACK_A_MOLE, MEMORY, REACTION_GAME }; 
GameType selectedGame = WHACK_A_MOLE;

// Prototyper
void selectGame();

void setup() {
    //Serial.begin(9600);
    buzzer.init();

    // Sett opp knapper og lys
    for (int i = 0; i < 3; i++) {
        pinMode(buttonPins[i], INPUT);  // Sett knappene som INPUT
        pinMode(molePins[i], OUTPUT);  // Sett lysene som OUTPUT
        digitalWrite(molePins[i], LOW); // Slå av lysene
    }

    selectGame(); // Start spillvalg

    // Initialiser valgt spill
    switch (selectedGame) {
        case WHACK_A_MOLE:
            whackAMoleGame.init();
            break;
        case MEMORY:
            memoryGame.init();
            break;
        case REACTION_GAME:
            reactionGame.init();
            break;
    }
}

void selectGame() {
    // Slå på lys for å indikere valg
    for (int i = 0; i < 3; i++) {
        digitalWrite(molePins[i], HIGH);
    }
    digitalWrite(molePins[3], LOW); // Slå av rødt lys
    digitalWrite(molePins[4], LOW); // Slå av gult lys

    while (true) {
        for (int i = 0; i < 3; i++) {
            if (digitalRead(buttonPins[i]) == HIGH) { // Hvis en knapp trykkes
                // Slukk alle lys for å indikere at spill er valgt
                for (int j = 0; j < 3; j++) {
                    digitalWrite(molePins[j], LOW);
                }

                // Sett det valgte spillet
                delay(1000); // Vent litt før spillvalg
                selectedGame = static_cast<GameType>(i);
                return; // Avslutt funksjonen etter valg
            }
        }
    }
}

void loop() {
    AbstractGame* activeGame = nullptr;

    // Velg aktivt spill
    switch (selectedGame) {
        case WHACK_A_MOLE:
            activeGame = &whackAMoleGame;
            break;
        case MEMORY:
            activeGame = &memoryGame;
            break;
        case REACTION_GAME:
            activeGame = &reactionGame;
            break;
    }

    // Oppdater aktivt spill
    if (activeGame->getState() == AbstractGame::RUNNING) {
        activeGame->update();
    } else if (activeGame->getState() == AbstractGame::GAME_OVER) {
        activeGame->waitForRestart();
        selectGame(); // Gå tilbake til menyen

        // Initialiser valgt spill på nytt
        switch (selectedGame) {
            case WHACK_A_MOLE:
                whackAMoleGame.init();
                break;
            case MEMORY:
                memoryGame.init();
                break;
            case REACTION_GAME:
                reactionGame.init();
            break;
        }
    }
}