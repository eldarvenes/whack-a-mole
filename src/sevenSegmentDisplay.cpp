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