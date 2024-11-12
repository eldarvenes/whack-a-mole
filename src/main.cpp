#include "config.h"
#include "game.h"
#include "display.h"

#include <Arduino.h>

Display display(12, 11, 10);
Game game(9);

void setup() {
    display.init();
    game.init();
}

void loop() {
    game.update();
    Serial.print("Score: ");
    Serial.println(game.getScore());
    int currentScore = game.getScore();
    display.showScore(currentScore);
    delay(100);
}