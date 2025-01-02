#include "game.h"
#include <Arduino.h>
#include "config.h"
#include "buzzer.h"
#include "moleGameStarter.h"
#include "sevenSegmentDisplay.h"

SevenSegmentDisplay display(dataPin, clkPin, csPin);
int loopCounter = 0;


Game::Game(int buzzerPin) 
    : buzzerPin(buzzerPin), lastMoleTime(0), score(0), buzzerOffTime(0), buzzerActive(false), lives(3) {

    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i] = new Mole(molePins[i], buttonPins[i]);
        moleDisplayTime[i] = 0;  // Start med alle moles "skjult"
    }
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);  // Start med buzzer av
    Serial.println("Game initialized.");
}

void Game::init() {
    //Serial.begin(9600);  // Start debugging
    for (int i = 0; i < NUM_MOLES; i++) {
        moles[i]->hide();
        moleDisplayTime[i] = 0;
        Serial.print("Mole ");
        Serial.print(i);
        Serial.println(" hidden.");
    }
    MoleGameStarter starter(moles, NUM_MOLES); 
    
    display.init();
    
    starter.startSequence(3); 
    score = 0;  // Tilbakestiller poengsummen
    lives = 5;  // Tilbakestiller antall liv
    gameState = RUNNING;
    Serial.println("Game initialized with score = 0.");
    Serial.print("Lives: ");
    Serial.println(lives);
}

void Game::update() {
    loopCounter++;
    if(loopCounter % 100 == 0) {
    display.init(); // for å sikre at displayet er aktivt
    
    }
    unsigned long currentTime = millis();
    display.showScoreAndLives(score, lives);
    
    // Håndter buzzer tidsavbrudd
    if (buzzerActive && currentTime >= buzzerOffTime) {
        digitalWrite(buzzerPin, LOW);
        buzzerActive = false;
    }

    // Sjekk om det er tid for en ny mole
    static unsigned long nextMoleDelay = random(200, 2000);
    if (currentTime - lastMoleTime > nextMoleDelay) {
        // Skjul alle moles før vi viser en ny
        for (int i = 0; i < NUM_MOLES; i++) {
            moles[i]->hide();
        }

        // Vis en tilfeldig mole
        int randomMole = random(NUM_MOLES);
        moles[randomMole]->show();
        moleDisplayTime[randomMole] = currentTime;
        lastMoleTime = currentTime;

        // Generer en ny tilfeldig forsinkelse for neste mole
        nextMoleDelay = random(200, 2000);

        Serial.print("Mole ");
        Serial.print(randomMole);
        Serial.println(" shown. Next mole in ");
        Serial.print(nextMoleDelay);
        Serial.println(" ms.");
    }

    // Debounce-logikk for knappetrykk
    static bool previousButtonState[NUM_MOLES] = {false};
    static unsigned long lastHitTime[NUM_MOLES] = {0};  // Tidsstempel for siste treff

    // For kvar mole
    for (int i = 0; i < NUM_MOLES; i++) {
        bool currentButtonState = digitalRead(buttonPins[i]) == HIGH;

        if (moles[i]->isVisible()) {
            if (currentButtonState && !previousButtonState[i]) {
                moles[i]->hide();

                // Spill "godlyd" ved treff
                tone(buzzerPin, 1000, 200);
                buzzerActive = true;
                buzzerOffTime = currentTime + 200;
                score++;

                Serial.print("Mole ");
                Serial.print(i);
                Serial.println(" hit! Score incremented.");

                lastHitTime[i] = currentTime;  // Lagre når denne mole ble truffet
            }
        } else if (currentButtonState && !previousButtonState[i]) {
            // Sjekk om det har gått nok tid siden siste treff
            if (currentTime - lastHitTime[i] > 500) {  // Ignorer knappetrykk kort tid etter treff
                tone(buzzerPin, 300);        // Spill en tone på 300 Hz
                delay(150);                  // Vent i 150 ms
                tone(buzzerPin, 200);        // Spill en lavere tone på 200 Hz
                delay(150);                  // Vent i 150 ms
                noTone(buzzerPin);           // Slå av lyden

                // Reduser liv
                lives--;
                Serial.println("Wrong button! Lost a life.");
                Serial.print("Lives remaining: ");
                Serial.println(lives);

                if (lives <= 0) {
                    endGame();
                    return;
                }
            }
        }

        // Oppdater forrige tilstand for denne knappen
        previousButtonState[i] = currentButtonState;
    }

    // Begrens logging for score og tid
    static unsigned long lastLogTime = 0;
    if (currentTime - lastLogTime >= 1000) {
        Serial.print("Score: ");
        Serial.println(score);
        Serial.print("Current time: ");
        Serial.println(currentTime);
        lastLogTime = currentTime;
    }
}

void Game::endGame() {

    for (int i = 0; i < NUM_MOLES; i++) {
            moles[i]->hide();
        }
    Serial.println("Game Over!");
    Serial.print("Final score: ");
    Serial.println(score);

    display.showScoreAndLives(score, 0);
    playGameOverMelody();
    gameState = GAME_OVER;  // Sett spilltilstanden til GAME_OVER
}

int Game::getScore() const {
    return score;
}

void Game::playGameOverMelody() {
    // Definer toner (frekvenser) og varigheter (ms)
    int melody[] = {494, 440, 392, 349, 330, 294, 262};  // Fallende toner (B4 -> C4)
    int durations[] = {200, 200, 200, 200, 200, 200, 400};  // Varighet for hver tone

    // Spill hver tone i melodien
    for (int i = 0; i < 7; i++) {
        tone(buzzerPin, melody[i], durations[i]);  // Spill tone
        delay(durations[i] * 1.3);  // Kort pause mellom toner
        noTone(buzzerPin);  // Slå av tonen
    }

    Serial.println("Game over melody played.");

    waitForRestart();
}

void Game::waitForRestart() {
    unsigned long startTime = 0;
    bool buttonsHeld = false;

    while (gameState == GAME_OVER) {
        // Les knappestatus
        bool button1Pressed = digitalRead(buttonPins[0]) == HIGH;  
        digitalWrite(9, HIGH);
        Serial.print("Button 1: ");
        Serial.println(button1Pressed ? "Pressed" : "Not Pressed");

        if (button1Pressed) {
            if (!buttonsHeld) {
                // Start timer når begge knappene trykkes
                buttonsHeld = true;
                startTime = millis();
                Serial.println("Both buttons pressed. Hold for 3 seconds to restart.");
            } else if (millis() - startTime >= 3000) {
                // Hvis knappene holdes i 3 sekunder
                Serial.println("Restarting game!");
                init();              // Restart spillet
                gameState = RUNNING; // Sett tilstanden tilbake til RUNNING
                return;              // Avslutt waitForRestart()
            }
        } else {
            // Nullstill hvis knappene slippes
            if (buttonsHeld) {
                Serial.println("Buttons released before 3 seconds.");
            }
            buttonsHeld = false;
        }

        delay(100);  // Unngå for rask polling
    }
}
Game::GameState Game::getState() const {
    return gameState;
}