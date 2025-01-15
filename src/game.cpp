#include "game.h"
#include <Arduino.h>
#include "molegamestarter.h"

int loopCounter = 0;

Game::Game(Buzzer& buzzer, SevenSegmentDisplay& display)
    : buzzer(buzzer), display(display), lastMoleTime(0), score(0), lives(3), gameState(GameState::GAME_OVER) {
    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i] = new Mole(molePins[i], buttonPins[i]);
        moleDisplayTime[i] = 0;  // Start med alle moles "skjult"
    }
}

void Game::init() {
    display.init();
    Serial.println("Starter Whack-a-Mole-spillet!");
    
    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i]->hide();
        moleDisplayTime[i] = 0;
    }
    MoleGameStarter starter(moles, NUM_MOLES);
    starter.startSequence(3);

    // Tilbakestill spillvariabler
    score = 0;
    lives = 5;
    gameState = GameState::RUNNING;

    // Spill en startlyd
    buzzer.playTone(880, 200);  // Høy tone for spillstart
    delay(200);
    buzzer.stop();
}

void Game::update() {
    loopCounter++;
    if (loopCounter % 100 == 0) {
        display.init(); // For å sikre at displayet er aktivt
    }

    unsigned long currentTime = millis();

    // Oppdater poeng og liv på displayet
    display.showScoreAndLives(score, lives);

    // Sjekk om det er tid for en ny mole
    static unsigned long nextMoleDelay = random(350, 2000);
    if (currentTime - lastMoleTime > nextMoleDelay) {
        for (int i = 0; i < NUM_MOLES; i++) {
            moles[i]->hide();
        }

        int randomMole = random(NUM_MOLES);
        moles[randomMole]->show();
        moleDisplayTime[randomMole] = currentTime;
        lastMoleTime = currentTime;
        nextMoleDelay = random(350, 2000);
    }

    // Debounce-logikk for knappetrykk
    static bool previousButtonState[NUM_MOLES] = {false};

    for (int i = 0; i < NUM_MOLES; i++) {
        bool currentButtonState = digitalRead(buttonPins[i]) == HIGH;

        if (moles[i]->isVisible()) {
            if (currentButtonState && !previousButtonState[i]) {
                moles[i]->hide();
                score++;

                // Spill trefflyd
                buzzer.playTone(1000, 200); // Høy tone for treff
                delay(200);
                buzzer.stop();
            }
        } else if (currentButtonState && !previousButtonState[i]) {
            lives--;

            // Spill bomlyd
            buzzer.playTone(400, 200); // Lav tone for bom
            delay(200);
            buzzer.stop();

            if (lives <= 0) {
                endGame();
                return;
            }
        }

        previousButtonState[i] = currentButtonState;
    }
}

void Game::endGame() {
    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i]->hide();
    }
    playGameOverMelody();
    gameState = GameState::GAME_OVER;
}

void Game::playGameOverMelody() {
    int melody[] = {494, 440, 392, 349, 330, 294, 262};
    int durations[] = {200, 200, 200, 200, 200, 200, 400};

    for (int i = 0; i < 7; i++) {
        buzzer.playTone(melody[i], durations[i]); // Bruk buzzer til melodien
        delay(durations[i] * 1.3);
    }
    buzzer.stop();

    waitForRestart();
}

void Game::waitForRestart() {
    Serial.println("Game over! Returning to game selection...");

    // Vis "Game Over" i 2 sekunder
    display.showScoreAndLives(score, 0);
    delay(2000);

    // Sett spillet i GAME_OVER-tilstand og returner til spillvelgeren
    gameState = GameState::GAME_OVER;
}

int Game::getScore() const {
    return score;
}

AbstractGame::GameState Game::getState() const {
    return gameState;
}