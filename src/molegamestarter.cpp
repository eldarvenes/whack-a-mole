#include "moleGameStarter.h"
#include <Arduino.h>

MoleGameStarter::MoleGameStarter(Mole* moles[], int numMoles)
    : moles(moles), numMoles(numMoles) {}

void MoleGameStarter::startSequence(int buzzerPin) {
    pinMode(buzzerPin, OUTPUT);

    // Første del: Blink sakte
    const unsigned long slowBlinkDuration = 5000;  // 5 sekunder for sakte blinking
    const unsigned long slowBlinkDelay = 250;     // 1 sekund mellom hvert blink
    unsigned long startTime = millis();

    Serial.println("Starting slow blink sequence...");
    while (millis() - startTime < slowBlinkDuration) {
        for (int i = 0; i < numMoles; i++) {
            moles[i]->show();  // Slå på mole
            //tone(buzzerPin, 440);  // Spill en lav tone (440 Hz)
            delay(slowBlinkDelay);  // Vent
            moles[i]->hide();  // Slå av mole
            noTone(buzzerPin);  // Slå av buzzeren
        }
    }

    // Andre del: Blink raskere
    const unsigned long fastBlinkDuration = 5000;  // 3 sekunder for rask blinking
    startTime = millis();

    Serial.println("Starting fast blink sequence...");
    while (millis() - startTime < fastBlinkDuration) {
        unsigned long timeElapsed = millis() - startTime;

        // Beregn dynamisk forsinkelse, fra 750 ms til 50 ms
        unsigned long dynamicDelay = map(timeElapsed, 0, fastBlinkDuration, 750, 50);

        // Slå på alle LED-er og spill en tone
        for (int i = 0; i < numMoles; i++) {
            moles[i]->show();
        }
        tone(buzzerPin, 880);  // Spill en høyere tone (880 Hz)
        delay(dynamicDelay);   // Vent i den dynamiske forsinkelsen

        // Slå av alle LED-er og buzzeren
        for (int i = 0; i < numMoles; i++) {
            moles[i]->hide();
        }
        noTone(buzzerPin);     // Slå av buzzeren
        delay(dynamicDelay);   // Vent igjen for å skape blink
    }

    // Tredje del: Vent uten noen moles tent
    const unsigned long finalWaitDuration = 1000;  // 3 sekunder
    Serial.println("Final wait...");
    delay(finalWaitDuration);

    Serial.println("Start sequence complete! Game ready to begin.");
}