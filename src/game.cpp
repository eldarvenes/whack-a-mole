#include "game.h"
#include <Arduino.h>

Game::Game(int buzzerPin) : buzzerPin(buzzerPin), lastMoleTime(0), score(0) {
    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i] = new Mole(molePins[i], buttonPins[i]);
    }
    pinMode(buzzerPin, OUTPUT);
}

void Game::init() {
    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i]->hide();
    }
    score = 0;  // Tilbakestiller poengsummen
}

void Game::update() {
    unsigned long currentTime = millis();

    // Sjekk om det er på tide å vise en ny mole
    if (currentTime - lastMoleTime > GAME_SPEED) {
        int randomMole = random(NUM_MOLES);
        moles[randomMole]->show();
        lastMoleTime = currentTime;
    }

    // Oppdater hver mole og sjekk om de er truffet
    for (int i = 0; i < NUM_MOLES; i++) {
        if (moles[i]->isVisible() && moles[i]->isHit()) {
            moles[i]->hide();  // Skjul mole hvis truffet
            digitalWrite(buzzerPin, HIGH);  // Aktiver buzzer
            delay(100);                     // Kort signal
            digitalWrite(buzzerPin, LOW);   // Deaktiver buzzer
            score++;  // Øk poengsummen for hvert treff
        }
    }
}

int Game::getScore() const {
    return score;
}