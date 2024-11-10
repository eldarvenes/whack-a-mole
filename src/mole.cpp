#include "mole.h"
#include <Arduino.h>

Mole::Mole(int ledPin, int buttonPin) : ledPin(ledPin), buttonPin(buttonPin), visible(false) {
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
}

void Mole::show() {
    visible = true;
    digitalWrite(ledPin, HIGH);
}

void Mole::hide() {
    visible = false;
    digitalWrite(ledPin, LOW);
}

bool Mole::isVisible() {
    return visible;
}

bool Mole::isHit() {
    return visible && digitalRead(buttonPin) == LOW;
}