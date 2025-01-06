#include "game.h"
#include <Arduino.h>

Game::Game(Buzzer& buzzer, SevenSegmentDisplay& display) 
    : buzzer(buzzer), display(display), lastMoleTime(0), score(0), buzzerActive(false), buzzerOffTime(0), lives(3) {
    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i] = new Mole(molePins[i], buttonPins[i]);
    }

    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i] = new Mole(molePins[i], buttonPins[i]);
        moleDisplayTime[i] = 0;  // Start med alle moles "skjult"
    }
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);  // Start med buzzer av
}

void Game::init() {
     Serial.println("Starter Whack-a-Mole-spillet!");
    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i]->hide();
        moleDisplayTime[i] = 0;
    }

    // Tilbakestill spillvariabler
    score = 0;
    lives = 5;
    gameState = GameState::RUNNING;
}

void Game::update() {
    unsigned long currentTime = millis();

    // Håndter buzzer tidsavbrudd
    if (buzzerActive && currentTime >= buzzerOffTime) {
        digitalWrite(buzzerPin, LOW);
        buzzerActive = false;
    }

    // Sjekk om det er tid for en ny mole
    static unsigned long nextMoleDelay = random(200, 2000);
    if (currentTime - lastMoleTime > nextMoleDelay) {
        for (int i = 0; i < NUM_MOLES; i++) {
            moles[i]->hide();
        }

        int randomMole = random(NUM_MOLES);
        moles[randomMole]->show();
        moleDisplayTime[randomMole] = currentTime;
        lastMoleTime = currentTime;
        nextMoleDelay = random(200, 2000);
    }

    // Debounce-logikk for knappetrykk
    static bool previousButtonState[NUM_MOLES] = {false};

    for (int i = 0; i < NUM_MOLES; i++) {
        bool currentButtonState = digitalRead(buttonPins[i]) == HIGH;

        if (moles[i]->isVisible()) {
            if (currentButtonState && !previousButtonState[i]) {
                moles[i]->hide();
                tone(buzzerPin, 1000, 200);
                buzzerActive = true;
                buzzerOffTime = currentTime + 200;
                score++;
            }
        } else if (currentButtonState && !previousButtonState[i]) {
            lives--;

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
        tone(buzzerPin, melody[i], durations[i]);
        delay(durations[i] * 1.3);
        noTone(buzzerPin);
    }

    waitForRestart();
}

void Game::waitForRestart() {
    Serial.println("Game over! Returning to game selection...");

    // Vent litt før tilbakeføring for visuell indikasjon
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