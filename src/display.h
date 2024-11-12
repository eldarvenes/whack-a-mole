#ifndef DISPLAY_H
#define DISPLAY_H

#include <LedControl.h>

class Display {
private:
    LedControl lc;   
    int score;   

public:
    Display(int dataPin, int clkPin, int csPin);
    void init();                
    void showScore(int score);  
};

#endif