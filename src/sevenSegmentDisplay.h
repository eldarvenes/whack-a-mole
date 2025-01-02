#ifndef SEVENSEGMENTDISPLAY_H
#define SEVENSEGMENTDISPLAY_H

#include <Arduino.h>
#include <LedControl.h>  // Bibliotek for 7-segment display (MAX7219)

class SevenSegmentDisplay {
public:
    SevenSegmentDisplay(int dataPin, int clkPin, int csPin);  // Konstruktør
    void init();                   // Initialiser displayet
    void showScoreAndLives(int score, int lives);  // Vis poeng og liv

private:
    LedControl lc;                 // Kontroll for MAX7219
};

#endif