#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
public:
    Buzzer(int pin);
    void playSound(int duration, int intensity = 128);
    void playErrorSound();
    void stop();
    void playTone(int frequency, int duration);                         
    void init();                         

private:
    int pin;
};

#endif