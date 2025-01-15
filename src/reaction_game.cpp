#include "reaction_game.h"
#include <Arduino.h>
#include "config.h"

ReactionGame::ReactionGame(Buzzer& buzzer, SevenSegmentDisplay& display)
    : buzzer(buzzer), display(display), gameState(GAME_OVER),
      lightOn(false), activeMole(-1), startTime(0), reactionTime(0),
      roundCountPlayer1(0), roundCountPlayer2(0), player1LightOn(false), player2LightOn(false),
      player1StartTime(0), player2StartTime(0) {
    memset(reactionTimesPlayer1, 0, sizeof(reactionTimesPlayer1));
    memset(reactionTimesPlayer2, 0, sizeof(reactionTimesPlayer2));
}

void ReactionGame::init() {
    gameState = RUNNING;
    display.startAnimation();
    roundCount = 0; // Nullstill runde-teller
    player1LightOn = false;
    player2LightOn = false;
    memset(reactionTimesPlayer1, 0, sizeof(reactionTimesPlayer1));
    memset(reactionTimesPlayer2, 0, sizeof(reactionTimesPlayer2));

    display.init();
    Serial.println("Reaction Game started!");

    // Tenne LED-er for spillvalg
    pinMode(player1MolePins[0], OUTPUT);
    pinMode(player2MolePins[0], OUTPUT);
    digitalWrite(player1MolePins[0], HIGH);
    digitalWrite(player2MolePins[0], HIGH);

    // Vent på brukervalget
    Serial.println("Press Player 1 button for ONE_PLAYER or Player 2 button for TWO_PLAYER.");
    while (true) {
        if (digitalRead(player1ButtonPins[0]) == HIGH) {
            gameMode = ONE_PLAYER;
            Serial.println("ONE_PLAYER mode selected!");
            break;
        } else if (digitalRead(player2ButtonPins[0]) == HIGH) {
            gameMode = TWO_PLAYER;
            Serial.println("TWO_PLAYER mode selected!");
            break;
        }
    }

    // Slukk LED-ene etter valg
    digitalWrite(player1MolePins[0], LOW);
    digitalWrite(player2MolePins[0], LOW);

    // Spill en startlyd
    playCoolStartSound(buzzer);
    delay(1000); // Vent litt før vi starter
    startRound();
}

void ReactionGame::startRound() {
    Serial.print("Starting round ");
    Serial.println(roundCount);

    // Sjekk om spillet er ferdig
    if (roundCount >= 5) {
        Serial.println("Max rounds reached. Ending game.");
        if (gameMode == ONE_PLAYER) {
            displayAverageReactionTime();
        } else if (gameMode == TWO_PLAYER) {
            displayAverageReactionTimesTwoPlayer();
        }
        endGame();
        return;
    }

    // Sett opp ny runde
    lightOn = false;
    waitingForNextRound = true;
    nextRoundStartTime = millis() + random(2000, 6000); // Planlegg ny runde

    if (gameMode == ONE_PLAYER) {
        Serial.println("ONE_PLAYER round set up. Waiting for light to turn on...");
    } else if (gameMode == TWO_PLAYER) {
        Serial.println("TWO_PLAYER round set up. Waiting for lights to turn on...");
    }
}

void ReactionGame::update() {
    unsigned long currentTime = millis();

    // Sjekk om vi venter på neste runde
    if (waitingForNextRound) {
        Serial.print("Waiting for next round. Current time: ");
        Serial.print(currentTime);
        Serial.print(", Next round start time: ");
        Serial.println(nextRoundStartTime);

        if (currentTime >= nextRoundStartTime) {
            waitingForNextRound = false;

            if (gameMode == ONE_PLAYER) {
                lightOn = true;
                activeMole = random(NUM_MOLES); // Velg en tilfeldig LED
                digitalWrite(molePins[activeMole], HIGH);
                startTime = currentTime;
                buzzer.playTone(1000, 200);
            } else if (gameMode == TWO_PLAYER) {
                player1LightOn = true;
                player2LightOn = true;

                player1Mole = random(2);
                player2Mole = random(2);

                digitalWrite(player1MolePins[player1Mole], HIGH);
                digitalWrite(player2MolePins[player2Mole], HIGH);

                Serial.print("Player 1 Mole: ");
                Serial.println(player1Mole);
                Serial.print("Player 2 Mole: ");
                Serial.println(player2Mole);

                player1StartTime = currentTime;
                player2StartTime = currentTime;

                buzzer.playTone(1000, 200);
            }
        }
        return;
    }

    // Håndter to-spiller-modus
    if (gameMode == TWO_PLAYER) {
        bool bothPlayersFinished = true;

        // Sjekk spiller 1
        if (player1LightOn) {
            bothPlayersFinished = false; // Spilleren er ikke ferdig
            for (int i = 0; i < 2; i++) {
                if (digitalRead(player1ButtonPins[i]) == HIGH) {
                    if (i == player1Mole) {
                        digitalWrite(player1MolePins[i], LOW); // Slukk LED-en
                        unsigned long reactionTime = currentTime - player1StartTime;
                        reactionTimesPlayer1[roundCount] = reactionTime;

                        Serial.print("Player 1 Reaction Time (Round ");
                        Serial.print(roundCount);
                        Serial.print("): ");
                        Serial.println(reactionTime);

                        player1LightOn = false; // Marker at spiller 1 er ferdig
                    } else {
                        Serial.println("Player 1 pressed the wrong button!");
                    }
                }
            }
        }

        // Sjekk spiller 2
        if (player2LightOn) {
            bothPlayersFinished = false; // Spilleren er ikke ferdig
            for (int i = 0; i < 2; i++) {
                if (digitalRead(player2ButtonPins[i]) == HIGH) {
                    if (i == player2Mole) {
                        digitalWrite(player2MolePins[i], LOW); // Slukk LED-en
                        unsigned long reactionTime = currentTime - player2StartTime;
                        reactionTimesPlayer2[roundCount] = reactionTime;

                        Serial.print("Player 2 Reaction Time (Round ");
                        Serial.print(roundCount);
                        Serial.print("): ");
                        Serial.println(reactionTime);

                        player2LightOn = false; // Marker at spiller 2 er ferdig
                    } else {
                        Serial.println("Player 2 pressed the wrong button!");
                    }
                }
            }
        }

        // Sjekk om begge spillerne er ferdige
        if (bothPlayersFinished) {
            Serial.println("Both players finished their round.");
            display.showScoreAndLives(
                reactionTimesPlayer2[roundCount],
                reactionTimesPlayer1[roundCount]
            );

            roundCount++; // Øk felles runde-teller
            Serial.println("Round count: " + roundCount);
            if (roundCount >= 5) {
                Serial.println("Max rounds reached. Ending game.");
                endGame();
            } else {
                startRound(); // Start neste runde
            }
        }
    }
}

void ReactionGame::endGame() {
    Serial.println("Reaction game over!");
    gameState = GAME_OVER;

    unsigned long totalPlayer1 = 0;
    unsigned long totalPlayer2 = 0;

    // Beregn totalsum for spiller 1 og spiller 2
    for (int i = 0; i < 5; i++) {
        totalPlayer1 += reactionTimesPlayer1[i];
        totalPlayer2 += reactionTimesPlayer2[i];
    }

    // Beregn gjennomsnitt for spillerne
    unsigned long averagePlayer1 = totalPlayer1 / 5;
    unsigned long averagePlayer2 = totalPlayer2 / 5;

    // Vis gjennomsnittene på displayet
    display.showScoreAndLives(averagePlayer2, averagePlayer1); // Forutsetter at displayet støtter to verdier


    int melody[] = {440, 330, 220, 220};
    int durations[] = {300, 300, 400, 500};
    for (int i = 0; i < 4; i++) {
        buzzer.playTone(melody[i], durations[i]);
        delay(durations[i]);
    }
    buzzer.stop();

    // Sørg for å resette runde-teller
    roundCount = 0;
}

void ReactionGame::waitForRestart() {
    Serial.println("Game over! Returning to game selection...");

    buzzer.playSound(200, 100);
    delay(200);
    buzzer.stop();

    delay(2000);
    gameState = GameState::GAME_OVER;
}

void ReactionGame::displayAverageReactionTime() {
    unsigned long total = 0;
    for (int i = 0; i < 5; i++) {
        total += reactionTimes[i];
        Serial.print("Round ");
        Serial.print(i + 1);
        Serial.print(" Reaction Time: ");
        Serial.println(reactionTimes[i]);
    }
    unsigned long average = total / 5;
    Serial.print("Average Reaction Time: ");
    Serial.println(average);

    display.showScoreAndLives(average, 0); // Vis gjennomsnittet
}

AbstractGame::GameState ReactionGame::getState() const {
    return gameState;
}

void ReactionGame::displayAverageReactionTimesTwoPlayer() {
    unsigned long totalPlayer1 = 0;
    unsigned long totalPlayer2 = 0;

    // Beregn totalsum for spiller 1
    for (int i = 0; i < 5; i++) {
        totalPlayer1 += reactionTimesPlayer1[i];
        Serial.print("Player 1 Round ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(reactionTimesPlayer1[i]);
    }

    // Beregn totalsum for spiller 2
    for (int i = 0; i < 5; i++) {
        totalPlayer2 += reactionTimesPlayer2[i];
        Serial.print("Player 2 Round ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(reactionTimesPlayer2[i]);
    }

    // Beregn gjennomsnitt
    unsigned long averagePlayer1 = totalPlayer1 / 5;
    unsigned long averagePlayer2 = totalPlayer2 / 5;

    // Vis gjennomsnittene på Serial Monitor
    Serial.print("Player 1 Average Reaction Time: ");
    Serial.println(averagePlayer1);
    Serial.print("Player 2 Average Reaction Time: ");
    Serial.println(averagePlayer2);

    // Oppdater displayet med begge spillernes gjennomsnitt
    display.showScoreAndLives(averagePlayer2, averagePlayer1); // Forutsetter at displayet støtter to verdier
}

void ReactionGame::playCoolStartSound(Buzzer& buzzer) {
    const int melody[] = {440, 523, 587, 659, 880, 784, 659, 440}; // Toner i Hz, fallende til stigende
    const int durations[] = {150, 150, 150, 200, 200, 200, 150, 300}; // Varighet i ms

    for (size_t i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
        buzzer.playTone(melody[i], durations[i]); // Spill tone
        delay(durations[i] + 50); // Kort pause mellom tonene
    }

    buzzer.stop();
}