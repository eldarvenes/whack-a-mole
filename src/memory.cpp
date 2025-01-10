#include "Memory.h"
#include <Arduino.h>

Memory::Memory(Buzzer& buzzer, SevenSegmentDisplay& display) 
    : buzzer(buzzer), display(display), gameState(GAME_OVER), currentStep(0), playerStep(0), sequenceLength(3), waitingForInput(false), lastStepTime(0) {}


void Memory::init() {
    display.init();
    generateSequence();
    currentStep = 0;
    playerStep = 0;
    sequenceLength = 3; // Start med en sekvens på 3 trinn
    waitingForInput = false;
    gameState = RUNNING;
    display.showScoreAndLives(0, 0);
    Serial.println("Memory game started. Watch the sequence!");

    // Spill en startlyd
    buzzer.playTone(880, 300); // Høy tone for å indikere start
    delay(300);
    buzzer.playTone(440, 300); // Lavere tone for å indikere sekvensstart
    buzzer.stop();
}

void Memory::generateSequence() {
    display.scramble();
    for (int i = 0; i < 50; i++) {
        sequence[i] = random(NUM_MOLES); // Generer tilfeldig sekvens mellom 0 og NUM_MOLES-1
    }
    Serial.print("Generated sequence: ");
    for (int i = 0; i < 50; i++) {
        Serial.print(sequence[i]);
        Serial.print(" ");
    }
    Serial.println();
}

void Memory::showSequence() {
    static bool sequenceInProgress = false;
    static unsigned long stepStartTime = 0;
    static bool lightOff = false;

    if (!sequenceInProgress) {
        currentStep = 0;
        sequenceInProgress = true;
        stepStartTime = millis();
        lightOff = false;
    }

    unsigned long currentTime = millis();

    if (currentStep < sequenceLength) {
        if (!lightOff && currentTime - stepStartTime > stepDelay) {
            // Slå av alle lys før neste lys tennes
            for (int i = 0; i < NUM_MOLES; i++) {
                digitalWrite(molePins[i], LOW);
            }
            lightOff = true; // Marker at lysene er slukket
            stepStartTime = currentTime; // Start ny forsinkelse for å slå på neste lys
        } else if (lightOff && currentTime - stepStartTime > 200) { // Kort pause før neste lys
            // Slå på det neste lyset i sekvensen
            digitalWrite(molePins[sequence[currentStep]], HIGH);

            // Spill en tone for hvert lys
            buzzer.playTone(440 + (sequence[currentStep] * 100), 200); // Differensier toner per lys
            delay(200);
            buzzer.stop();

            currentStep++;
            lightOff = false; // Klar til neste slukking
            stepStartTime = currentTime;
        }
    } else if (sequenceInProgress) {
        // Etter at siste lys har vært tent i full stepDelay, slukk det
        if (!lightOff && currentTime - stepStartTime > stepDelay) {
            for (int i = 0; i < NUM_MOLES; i++) {
                digitalWrite(molePins[i], LOW); // Slå av alle lys
            }
            sequenceInProgress = false;
            waitingForInput = true; // Gå til input-modus
        }
    }
}

void Memory::checkInput() {
    static unsigned long lastButtonPressTime = 0; // For debounce
    static bool buttonPreviouslyPressed[NUM_MOLES] = {false};

    unsigned long currentTime = millis();

    for (int i = 0; i < NUM_MOLES; i++) {
        bool buttonState = digitalRead(buttonPins[i]) == HIGH;

        if (buttonState && !buttonPreviouslyPressed[i] && (currentTime - lastButtonPressTime > 100)) {
            // Registrer knappetrykk
            lastButtonPressTime = currentTime;

            // Blink lyset for den trykkede knappen
            digitalWrite(molePins[i], HIGH);
            delay(50); // Kort blinktid, men vurder å gjøre dette ikke-blokkerende også
            digitalWrite(molePins[i], LOW);

            if (sequence[playerStep] == i) {
                Serial.print("Correct: ");
                Serial.println(i);
                buzzer.playTone(440 + (i * 100), 50); // Kort tone
                buzzer.stop();

                playerStep++;

                if (playerStep == sequenceLength) {
                    // Spilleren gjettet riktig hele sekvensen
                    Serial.println("Sequence complete! Adding a new step...");
                    
                    // Oppdater poengsummen
                    int pointsEarned = sequenceLength;
                    display.showScoreAndLives(pointsEarned, 0);
                    Serial.print("Score updated: ");
                    Serial.println(pointsEarned);

                    // Blink alle LED-er for å indikere suksess
                    for (int j = 0; j < 2; j++) {
                        buzzer.playTone(1200, 100);
                        for (int k = 0; k < NUM_MOLES; k++) {
                            digitalWrite(molePins[k], HIGH);
                        }
                        delay(100);
                        for (int k = 0; k < NUM_MOLES; k++) {
                            digitalWrite(molePins[k], LOW);
                        }
                        delay(100);
                    }
                    buzzer.stop();

                    // Forbered neste runde
                    sequenceLength++;
                    playerStep = 0;
                    currentStep = 0;
                    waitingForInput = false;
                }
            } else {
                // Spilleren gjettet feil
                Serial.println("Wrong button! Game over.");

                // Blink den riktige knappen
                int correctButton = sequence[playerStep];
                for (int blink = 0; blink < 5; blink++) {
                    digitalWrite(molePins[correctButton], HIGH);
                    delay(50);
                    digitalWrite(molePins[correctButton], LOW);
                    delay(50);
                }

                buzzer.playTone(400, 500);
                buzzer.stop();

                display.showScoreAndLives(0, 0);
                endGame();
            }
        }

        buttonPreviouslyPressed[i] = buttonState; // Oppdater knappens tilstand
    }
}

void Memory::update() {
    if (gameState == RUNNING) {
        if (!waitingForInput) {
            showSequence(); // Vis sekvensen
        } else {
            checkInput(); // Sjekk spillerens input
        }
    } else if (gameState == GAME_OVER) {
        waitForRestart(); // Vent på restart
    }
}

void Memory::endGame() {
    Serial.println("Memory game over!");

    // Spill en melodilinje for game over
    int melody[] = {440, 330, 220, 220};
    int durations[] = {300, 300, 400, 500};
    for (int i = 0; i < 4; i++) {
        buzzer.playTone(melody[i], durations[i]);
        delay(durations[i]);
    }
    buzzer.stop();

    flashSequence(sequenceLength);

    gameState = GAME_OVER;
}

void Memory::flashSequence(int upToStep) {
    Serial.println("Flashing sequence so far...");
    for (int i = 0; i < upToStep; i++) {
        // Tenn lyset i sekvensen
        digitalWrite(molePins[sequence[i]], HIGH);
        
        // Spill en tone for å indikere steget
        buzzer.playTone(440 + (sequence[i] * 100), 200); 
        delay(200); // Hold lyset tent
        
        // Slukk lyset
        digitalWrite(molePins[sequence[i]], LOW);
        buzzer.stop();
        delay(100); // Kort pause mellom lysene
    }
}
AbstractGame::GameState Memory::getState() const {
    return gameState;
}

void Memory::waitForRestart() {
    Serial.println("Game over! Returning to game selection...");

    // Spill en indikasjonstone
    buzzer.playSound(200, 100);
    delay(200);
    buzzer.stop();

    // Vent litt før tilbakeføring for visuell indikasjon
    delay(2000);

    // Sett spillet i GAME_OVER-tilstand og returner til spillvelgeren
    gameState = GameState::GAME_OVER;
}