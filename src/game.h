#ifndef GAME_H
#define GAME_H

#include "mole.h"
#include "config.h"

#define GAME_SPEED 1000  // Hastighet for å vise nye moles i millisekunder

class Game {
public:
    Game(int buzzerPin);

    void init();
    void update();
    int getScore() const;  // Legg til getScore()-metode

private:
    Mole* moles[NUM_MOLES];          // Array av mole-pekere
    int buzzerPin;                   // Pin for felles buzzer
    unsigned long lastMoleTime;      // Tidspunkt for siste mole-visning
    int score;                       // Poengsum for spillet

    int molePins[NUM_MOLES] = {2, 3, 4};     // LED-pins for hver mole
    int buttonPins[NUM_MOLES] = {5, 6, 7};   // Button-pins for hver mole
};

#endif // GAME_H