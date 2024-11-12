#include "game.h"
#include <Arduino.h>
#include "config.h"
#include "buzzer.h"

Game::Game(int buzzerPin) 
    : buzzerPin(buzzerPin), lastMoleTime(0), score(0), buzzerOffTime(0), buzzerActive(false) {
    
    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i] = new Mole(molePins[i], buttonPins[i]);
        moleDisplayTime[i] = 0;  // Start med alle moles "skjult"
    }
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);  // Start med buzzer av
}

void Game::init() {
    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i]->hide();
        moleDisplayTime[i] = 0;
    }
    score = 0;  // Tilbakestiller poengsummen
}

void Game::update() {
    unsigned long currentTime = millis();

    // Håndter buzzer tidsavbrudd (ikke-blokkerende)
    if (buzzerActive && currentTime >= buzzerOffTime) {
        digitalWrite(buzzerPin, LOW);
        buzzerActive = false;
    }

    // Skjul alle moles før vi viser en ny, slik at kun én mole er synlig om gangen
    if (currentTime - lastMoleTime > GAME_SPEED) {
        for (int i = 0; i < NUM_MOLES; i++) {
            moles[i]->hide();
        }

        int randomMole = random(NUM_MOLES);
        moles[randomMole]->show();
        moleDisplayTime[randomMole] = currentTime; // Start tidsmåling for den mole som vises
        lastMoleTime = currentTime;
    }

    // Oppdater hver mole, sjekk om de er truffet eller har vært synlige for lenge
    for (int i = 0; i < NUM_MOLES; i++) {
        if (moles[i]->isVisible()) {
            if (moles[i]->isHit()) {
                moles[i]->hide();  // Skjul mole hvis truffet
                digitalWrite(buzzerPin, HIGH);  // Aktiver buzzer for treff
                buzzerOffTime = currentTime + 200;  // Sett buzzerens av-tid til 200 ms frem
                buzzerActive = true;
                score++;  // Øk poengsummen for hvert treff
            } 
            // Skjul mole etter `MOLE_DISPLAY_DURATION` om den har vært synlig for lenge
            else if (currentTime - moleDisplayTime[i] > MOLE_DISPLAY_DURATION) {
                moles[i]->hide();
            }
        } else if (moles[i]->isHit()) {
            // Spill en feillyd hvis en mole trykkes uten å være synlig
            digitalWrite(buzzerPin, HIGH);  // Aktiver feillyd
            buzzerOffTime = currentTime + 100;  // Kortere tid for feillyd
            buzzerActive = true;
        }
    }
}

int Game::getScore() const {
    return score;
}