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
    buzzer.playTone(880, 300);
    delay(300);
    buzzer.playTone(440, 300);
    buzzer.stop();

    startRound();
}

void ReactionGame::startRound() {
    if (gameMode == ONE_PLAYER) {
        if (roundCountPlayer1 >= 5) {
            displayAverageReactionTime(); // Vis gjennomsnittet for én spiller
            endGame();
            return;
        }

        if (roundCountPlayer1 > 0) {
            // Vis reaksjonstiden for forrige runde
            unsigned long lastReactionTime = reactionTimesPlayer1[roundCountPlayer1 - 1];
            display.showScoreAndLives(lastReactionTime, 0); // Bare spillerens reaksjonstid vises
            Serial.print("Previous Reaction Time: ");
            Serial.println(lastReactionTime);
        }

        lightOn = false;
        delay(random(2000, 5000)); // Vent et tilfeldig antall millisekunder
        activeMole = random(NUM_MOLES); // Velg en tilfeldig LED
        digitalWrite(molePins[activeMole], HIGH);
        startTime = millis();
        lightOn = true;
        buzzer.playTone(1000, 200); // Lyd når LED tennes
        delay(200);
        buzzer.stop();
    } else if (gameMode == TWO_PLAYER) {
        if (roundCountPlayer1 >= 5 && roundCountPlayer2 >= 5) {
            displayAverageReactionTimesTwoPlayer(); // Vis gjennomsnittet for begge spillere
            endGame();
            return;
        }

        if (roundCountPlayer1 > 0) {
            unsigned long lastReactionTimePlayer1 = reactionTimesPlayer1[roundCountPlayer1 - 1];
            Serial.print("Player 1 Previous Reaction Time: ");
            Serial.println(lastReactionTimePlayer1);
        }

        if (roundCountPlayer2 > 0) {
            unsigned long lastReactionTimePlayer2 = reactionTimesPlayer2[roundCountPlayer2 - 1];
            Serial.print("Player 2 Previous Reaction Time: ");
            Serial.println(lastReactionTimePlayer2);
        }

        if (roundCountPlayer1 < 5) {
            int player1Mole = random(2); // Velg en tilfeldig LED for spiller 1
            digitalWrite(player1MolePins[player1Mole], HIGH);
            player1StartTime = millis();
            player1LightOn = true;
        }

        if (roundCountPlayer2 < 5) {
            int player2Mole = random(2); // Velg en tilfeldig LED for spiller 2
            digitalWrite(player2MolePins[player2Mole], HIGH);
            player2StartTime = millis();
            player2LightOn = true;
        }
    }
}

void ReactionGame::update() {
    if (gameState == RUNNING) {
        unsigned long currentTime = millis();

        // Håndter én spiller-modus
        if (gameMode == ONE_PLAYER && lightOn) {
            for (int i = 0; i < NUM_MOLES; i++) {
                if (digitalRead(buttonPins[i]) == HIGH) {
                    digitalWrite(molePins[activeMole], LOW); // Slukk LED-en
                    unsigned long reactionTime = currentTime - startTime;
                    reactionTimesPlayer1[roundCountPlayer1++] = reactionTime;
                    Serial.print("Reaction Time (Round ");
                    Serial.print(roundCountPlayer1);
                    Serial.print("): ");
                    Serial.println(reactionTime);

                    // Spill bekreftelsestone uten delay
                    //if (!buzzer.isPlaying()) {
                     //   buzzer.playTone(1200, 200);
                    //}

                    lightOn = false;
                    startTime = currentTime; // Start ny runde etter 1 sekund
                }
            }

            // Start ny runde etter 1 sekund
            if (!lightOn && currentTime - startTime >= 1000) {
                startRound();
            }
        } 
        // Håndter to-spiller-modus
        else if (gameMode == TWO_PLAYER) {
            // Sjekk knappetrykk for spiller 1
            if (player1LightOn) {
                for (int i = 0; i < 2; i++) {
                    if (digitalRead(player1ButtonPins[i]) == HIGH) {
                        digitalWrite(player1MolePins[i], LOW); // Slukk LED-en
                        unsigned long reactionTime = currentTime - player1StartTime;
                        reactionTimesPlayer1[roundCountPlayer1++] = reactionTime;
                        Serial.print("Player 1 Reaction Time (Round ");
                        Serial.print(roundCountPlayer1);
                        Serial.print("): ");
                        Serial.println(reactionTime);

                        player1LightOn = false;
                        player1StartTime = currentTime; // Start ny runde etter 1 sekund
                    }
                }
            }

            // Sjekk knappetrykk for spiller 2
            if (player2LightOn) {
                for (int i = 0; i < 2; i++) {
                    if (digitalRead(player2ButtonPins[i]) == HIGH) {
                        digitalWrite(player2MolePins[i], LOW); // Slukk LED-en
                        unsigned long reactionTime = currentTime - player2StartTime;
                        reactionTimesPlayer2[roundCountPlayer2++] = reactionTime;
                        Serial.print("Player 2 Reaction Time (Round ");
                        Serial.print(roundCountPlayer2);
                        Serial.print("): ");
                        Serial.println(reactionTime);

                        player2LightOn = false;
                        player2StartTime = currentTime; // Start ny runde etter 1 sekund
                    }
                }
            }

            // Start ny runde for begge spillere etter 1 sekund
            if (!player1LightOn && !player2LightOn &&
                currentTime - max(player1StartTime, player2StartTime) >= 1000) {
                startRound();
            }
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