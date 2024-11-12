#include "Buzzer.h"
#include <Arduino.h>

Buzzer::Buzzer(int pin) : pin(pin) {}

void Buzzer::init() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Buzzer::playSound(int duration, int intensity) {
    analogWrite(pin, intensity);  // Justerer PWM til gitt duty cycle
    delay(duration);
    analogWrite(pin, 0);          // Slår av buzzer etterpå
}

void Buzzer::playErrorSound() {
    // Spiller en kort, lav lyd for feil
    analogWrite(pin, 80);  // Lav intensitet for feillyd
    delay(50);             // Kort varighet
    analogWrite(pin, 0);   // Slår av etterpå
}

void Buzzer::stop() {
    digitalWrite(pin, LOW);
}