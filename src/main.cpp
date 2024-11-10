#include "config.h"
#include "game.h"

#include <Arduino.h>

Game game(8);

void setup() {
    game.init();
}

void loop() {
    game.update();
    Serial.print("Score: ");
    Serial.println(game.getScore());

    delay(100);
}