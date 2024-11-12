#include "display.h"

Display::Display(int dataPin, int clkPin, int csPin) : lc(dataPin, clkPin, csPin, 1), score(0) {
}

void Display::init() {
    lc.shutdown(0, false);        // Start displayet
    lc.setIntensity(0, 8);        // Juster lysstyrken (0-15)
    lc.clearDisplay(0);           // Tøm displayet
}

void Display::showScore(int score) {
    this->score = score;

    // Del opp poengsummen i enkeltsifre for å vise på displayet
    for (int i = 0; i < 8; i++) {
        int digit = score % 10;   // Hent siste siffer i poengsummen
        lc.setDigit(0, i, digit, false);  // Vis sifferet på riktig segment
        score /= 10;              // Fjern siste siffer
    }
}