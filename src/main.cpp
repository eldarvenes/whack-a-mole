#include "config.h"
#include "game.h"
#include <Arduino.h>

Game game(3);

void setup() {
    //Serial.begin(9600);  // Start seriell kommunikasjon
    game.init();         // Initialiser spillet
}

void loop() {
    // Sjekk spilltilstanden
    if (game.getState() == Game::RUNNING) {
        game.update();  // Oppdater spillet kun når det kjører
    } else if (game.getState() == Game::GAME_OVER) {
        game.waitForRestart();  // Vent på at spilleren starter spillet på nytt
    }

    // Begrens logging av poengsummen
    static unsigned long lastLogTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastLogTime >= 1000) {  // Logg poengsummen hvert sekund
        Serial.print("Score: ");
        Serial.println(game.getScore());
        lastLogTime = currentTime;
    }
}