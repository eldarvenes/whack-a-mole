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
    roundCountPlayer1 = 0;
    roundCountPlayer2 = 0;
    player1LightOn = false;
    player2LightOn = false;
    memset(reactionTimesPlayer1, 0, sizeof(reactionTimesPlayer1));
    memset(reactionTimesPlayer2, 0, sizeof(reactionTimesPlayer2));

    display.init();
    Serial.println("Reaction Game started!");

    // Tenne LED-er for spillvalg
    pinMode(player1MolePins[0], OUTPUT); // Bruker spiller 1s LED for enkeltspiller
    pinMode(player2MolePins[0], OUTPUT); // Bruker spiller 2s LED for to spillere
    digitalWrite(player1MolePins[0], HIGH); // Tenne LED for enkeltspiller
    digitalWrite(player2MolePins[0], HIGH); // Tenne LED for to spillere

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
    delay (1000); // Vent litt før vi starter
    startRound();
}

void ReactionGame::startRound() {
    // Sjekk om spillet er ferdig
    if (roundCount >= 5) {
        if (gameMode == ONE_PLAYER) {
            displayAverageReactionTime();
        } else if (gameMode == TWO_PLAYER) {
            displayAverageReactionTimesTwoPlayer();
        }
        endGame();
        return;
    }

    // Vis reaksjonstider fra forrige runde hvis tilgjengelig
    if (roundCount > 0) {
        if (gameMode == ONE_PLAYER) {
            unsigned long lastReactionTime = reactionTimesPlayer1[roundCount - 1];
            display.showScoreAndLives(lastReactionTime, 0);
            Serial.print("Previous Reaction Time: ");
            Serial.println(lastReactionTime);
        } else if (gameMode == TWO_PLAYER) {
            unsigned long lastReactionTimePlayer1 = reactionTimesPlayer1[roundCount - 1];
            unsigned long lastReactionTimePlayer2 = reactionTimesPlayer2[roundCount - 1];
            display.showScoreAndLives(lastReactionTimePlayer2, lastReactionTimePlayer1);
            Serial.print("Player 1 Previous Reaction Time: ");
            Serial.println(lastReactionTimePlayer1);
            Serial.print("Player 2 Previous Reaction Time: ");
            Serial.println(lastReactionTimePlayer2);
        }
    }

    // Sett opp ny runde
    lightOn = false;
    waitingForNextRound = true;
    nextRoundStartTime = millis() + random(2000, 6000); // Planlegg ny runde
}

void ReactionGame::update() {
    unsigned long currentTime = millis();

    // Sjekk om vi venter på neste runde
    if (waitingForNextRound && currentTime >= nextRoundStartTime) {
        waitingForNextRound = false;

        if (gameMode == ONE_PLAYER) {
            // Start én spiller-runde
            lightOn = true;
            activeMole = random(NUM_MOLES); // Velg en tilfeldig LED
            digitalWrite(molePins[activeMole], HIGH);
            startTime = currentTime;
            buzzer.playTone(1000, 200); // Lyd når LED tennes
        } else if (gameMode == TWO_PLAYER) {
            // Start to spiller-runde
            player1LightOn = true;
            player2LightOn = true;

            int player1Mole = random(2);
            int player2Mole = random(2);

            digitalWrite(player1MolePins[player1Mole], HIGH);
            digitalWrite(player2MolePins[player2Mole], HIGH);

            player1StartTime = currentTime;
            player2StartTime = currentTime;

            buzzer.playTone(1000, 200); // Lyd når LED tennes
        }
        return;
    }

    // Håndter én spiller-modus
    if (gameMode == ONE_PLAYER && lightOn) {
        for (int i = 0; i < NUM_MOLES; i++) {
            if (digitalRead(buttonPins[i]) == HIGH) {
                digitalWrite(molePins[activeMole], LOW); // Slukk LED-en
                unsigned long reactionTime = currentTime - startTime;
                reactionTimesPlayer1[roundCount++] = reactionTime;

                Serial.print("Reaction Time (Round ");
                Serial.print(roundCount);
                Serial.print("): ");
                Serial.println(reactionTime);

                display.showScoreAndLives(reactionTime, 0); // Vis reaksjonstiden
                lightOn = false;

                startRound(); // Start neste runde
                return;
            }
        }
    }

    // Håndter to-spiller-modus
    if (gameMode == TWO_PLAYER) {
        bool bothPlayersFinished = false;

        // Sjekk spiller 1
        if (player1LightOn) {
            for (int i = 0; i < 2; i++) {
                if (digitalRead(player1ButtonPins[i]) == HIGH) {
                    digitalWrite(player1MolePins[i], LOW); // Slukk LED-en
                    unsigned long reactionTime = currentTime - player1StartTime;
                    reactionTimesPlayer1[roundCount] = reactionTime;

                    Serial.print("Player 1 Reaction Time (Round ");
                    Serial.print(roundCount);
                    Serial.print("): ");
                    Serial.println(reactionTime);

                    player1LightOn = false;
                }
            }
        }

        // Sjekk spiller 2
        if (player2LightOn) {
            for (int i = 0; i < 2; i++) {
                if (digitalRead(player2ButtonPins[i]) == HIGH) {
                    digitalWrite(player2MolePins[i], LOW); // Slukk LED-en
                    unsigned long reactionTime = currentTime - player2StartTime;
                    reactionTimesPlayer2[roundCount] = reactionTime;

                    Serial.print("Player 2 Reaction Time (Round ");
                    Serial.print(roundCount);
                    Serial.print("): ");
                    Serial.println(reactionTime);

                    player2LightOn = false;
                }
            }
        }

        // Sjekk om begge spillerne er ferdige
        if (!player1LightOn || !player2LightOn) {
            bothPlayersFinished = true;
        }

        if (bothPlayersFinished) {
            display.showScoreAndLives(
                reactionTimesPlayer2[roundCount],
                reactionTimesPlayer1[roundCount]
            );

            roundCount++; // Øk felles runde-teller
            startRound(); // Start neste runde
        }
    }
}

bool ReactionGame::checkExitCondition() {
    static unsigned long pressStartTime = 0;
    static bool buttonPreviouslyPressed = false;

    if (digitalRead(buttonPins[1]) == HIGH) { // Anta knapp 1 brukes til avslutning
        if (!buttonPreviouslyPressed) {
            pressStartTime = millis();
            buttonPreviouslyPressed = true;
        } else if (millis() - pressStartTime >= 2000) { // Holdt i minst 2 sekunder
            Serial.println("Exiting Reaction Game due to long button press.");
            return true;
        }
    } else {
        buttonPreviouslyPressed = false;
        pressStartTime = 0;
    }

    return false;
}

void ReactionGame::endGame() {
    Serial.println("Reaction game over!");

    int melody[] = {440, 330, 220, 220};
    int durations[] = {300, 300, 400, 500};
    for (int i = 0; i < 4; i++) {
        buzzer.playTone(melody[i], durations[i]);
        delay(durations[i]);
    }
    buzzer.stop();
    gameState = GAME_OVER;
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