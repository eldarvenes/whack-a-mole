#include "sevenSegmentDisplay.h"

// Konstruktør: Initialiser LedControl med data, klokke og CS-pinner
SevenSegmentDisplay::SevenSegmentDisplay(int dataPin, int clkPin, int csPin)
    : lc(dataPin, clkPin, csPin, 1) {  // "1" betyr ett display
}

// Initialiser 7-segments displayet
void SevenSegmentDisplay::init() {
    lc.shutdown(0, false);       // Aktiver displayet
    lc.setIntensity(0, 8);       // Sett lysstyrken (0–15)
    lc.clearDisplay(0);          // Fjern alle tall
}

void SevenSegmentDisplay::showScoreAndLives(int score, int lives) {
    static int lastScore = -1; // Husk forrige poengsum for å unngå blinking
    static int lastLives = -1; // Husk forrige antall liv

    // Bare oppdater displayet hvis verdiene faktisk har endret seg
    if (score != lastScore || lives != lastLives) {
        // Oppdater poeng (høyre side)
        int pos = 0; // Start på plass 0 (helt til høyre)
        int scoreDigits[4] = {0}; // Buffer for sifrene
        int digitCount = 0;

        // Ekstraher sifrene fra score (fra minst til mest signifikante)
        while (score > 0 && digitCount < 4) {
            scoreDigits[digitCount++] = score % 10; // Ekstraher minst signifikante sifre
            score /= 10;
        }

        // Skriv sifrene fra mest signifikante til minst (fyll fra høyre mot venstre)
        int startPos = 3 - (digitCount - 1); // Beregn startposisjon for mest signifikante
        for (int i = 0; i < digitCount; i++) {
            lc.setDigit(0, startPos + i, scoreDigits[i], false); // Fyller fra venstre til høyre
        }

        // Hvis ingen poeng er vist (score er 0), vis '0' på plass 3
        if (digitCount == 0) {
            lc.setDigit(0, 3, 0, false);
        }

        // Fjern eventuelle tidligere tall på høyre side
        for (int i = 0; i < startPos; i++) {
            lc.setChar(0, i, ' ', false); // Slå av ubrukte sifre
        }

        // Oppdater liv (venstre side)
        pos = 7; // Start på plass 7 (helt til venstre)
        int livesDigits[4] = {0}; // Buffer for sifrene
        digitCount = 0;

        // Ekstraher sifrene fra lives (fra minst til mest signifikante)
        while (lives > 0 && digitCount < 4) {
            livesDigits[digitCount++] = lives % 10; // Ekstraher minst signifikante sifre
            lives /= 10;
        }

        // Skriv sifrene fra mest signifikante til minst (fyll fra venstre mot høyre)
        startPos = 7 - (digitCount - 1); // Beregn startposisjon for mest signifikante
        for (int i = 0; i < digitCount; i++) {
            lc.setDigit(0, startPos + i, livesDigits[i], false); // Fyller fra venstre til høyre
        }

        // Fjern eventuelle tidligere tall på venstre side
        for (int i = startPos - 1; i >= 4; i--) {
            lc.setChar(0, i, ' ', false); // Slå av ubrukte sifre
        }

        // Oppdater siste verdier
        lastScore = score;
        lastLives = lives;
    }
}

void SevenSegmentDisplay::scramble() {
    unsigned long startTime = millis();
    unsigned long animationDuration = 3000; // Animasjon varer i 3 sekunder
    unsigned long updateInterval = 50;      // Oppdatering hvert 50 ms
    unsigned long lastUpdateTime = 0;

    while (millis() - startTime < animationDuration) {
        unsigned long currentTime = millis();
        if (currentTime - lastUpdateTime >= updateInterval) {
            // Oppdater displayet med tilfeldige tall
            for (int i = 0; i < 8; i++) {
                lc.setDigit(0, i, random(10), false); // Vis tilfeldig tall
            }

            // Spill en kort lyd med buzzeren (hvis ønskelig)
            // buzzer.playTone(440 + random(100), 20);

            lastUpdateTime = currentTime;
        }
    }

    lc.clearDisplay(0); // Rens displayet etter animasjon
}

void SevenSegmentDisplay::startAnimation() {
    unsigned long startTime = millis();
    unsigned long animationDuration = 3000; // Animasjon varer i 3 sekunder
    unsigned long updateInterval = 100;    // Oppdatering hvert 100 ms
    unsigned long lastUpdateTime = 0;
    int currentIndex = 0;
    bool reverse = false; // Indikerer retning på animasjonen

    while (millis() - startTime < animationDuration) {
        unsigned long currentTime = millis();
        if (currentTime - lastUpdateTime >= updateInterval) {
            // Slå av alle lys
            for (int i = 0; i < 8; i++) {
                lc.setDigit(0, i, 0, false); // Slå av alle segmenter
            }

            // Tenn lys på currentIndex
            lc.setDigit(0, currentIndex, random(1, 10), false);

            // Spill en kort lyd med buzzeren for hvert steg
            //buzzer.playTone(880 + (currentIndex * 20), 50);
            //delay(50);
            //buzzer.stop();

            // Oppdater currentIndex for animasjonen
            if (!reverse) {
                currentIndex++;
                if (currentIndex >= 7) reverse = true; // Bytt retning
            } else {
                currentIndex--;
                if (currentIndex <= 0) reverse = false; // Bytt retning igjen
            }

            lastUpdateTime = currentTime;
        }
    }

    lc.clearDisplay(0); // Rens displayet etter animasjon
}


void SevenSegmentDisplay::showText(const char* text) {
    Serial.println("init-display");
    lc.clearDisplay(0); // Rens displayet først

    // Maks 8 tegn kan vises samtidig
    for (int i = 0; i < 8 && text[i] != '\0'; i++) {
        char c = text[i];
        if (c >= '0' && c <= '9') {
            lc.setDigit(0, i, c - '0', false); // Vis tall
        } else {
            switch (c) {
                case 'M': lc.setRow(0, i, 0b01100011); break; // Tilpasset 'M'
                case 'E': lc.setChar(0, i, 'E', false); break;
                case 'O': lc.setChar(0, i, 'o', false); break;
                case 'R': lc.setRow(0, i, 0b01010000); break; // Tilpasset 'R'
                case 'Y': lc.setRow(0, i, 0b01011000); break; // Tilpasset 'Y'
                case ' ': lc.setChar(0, i, ' ', false); break; // Blank plass
                default:  lc.setChar(0, i, '-', false); break; // Ukjente tegn
            }
        }
    }
}