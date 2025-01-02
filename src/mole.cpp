#include "mole.h"
#include <Arduino.h>

Mole::Mole(int ledPin, int buttonPin) : ledPin(ledPin), buttonPin(buttonPin), visible(false) {
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    //Serial.print("Mole created with LED pin ");
    //Serial.print(ledPin);
    //Serial.print(" and button pin ");
    //Serial.println(buttonPin);
}

void Mole::show() {
    visible = true;
    digitalWrite(ledPin, HIGH);
    //Serial.print("Mole on pin ");
    //Serial.print(ledPin);
    //Serial.println(" is now visible.");
}

void Mole::hide() {
    visible = false;
    digitalWrite(ledPin, LOW);
    //Serial.print("Mole on pin ");
    //Serial.print(ledPin);
    //Serial.println(" is now hidden.");
}

bool Mole::isVisible() {
    //Serial.print("Mole on pin ");
    //Serial.print(ledPin);
    //Serial.print(" visibility check: ");
    //Serial.println(visible ? "Visible" : "Hidden");
    return visible;
}

bool Mole::isHit() {
    bool buttonPressed = digitalRead(buttonPin) == HIGH;
    if (visible && buttonPressed) {
        Serial.print("Mole on pin ");
        Serial.print(buttonPin);
        Serial.println(" was hit!");
        return true;  // Treff når mole er synlig og knapp trykkes
    }
    return buttonPressed;  // Returner knappens status uavhengig av visible
}
