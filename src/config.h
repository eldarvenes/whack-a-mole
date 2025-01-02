#ifndef CONFIG_H
#define CONFIG_H

// Antall moles (Knapper og LED-er)
#define NUM_MOLES 5

// Pin-konfigurasjoner
const int molePins[NUM_MOLES] = {9, 13, 11, 10, 12};      // Grønn, Kvit, Blå, Gul, Rød LED
const int buttonPins[NUM_MOLES] = {2, 6, 7, 4, 5};       // Grønn, Kvit, Blå, Gul, Rød bryter

// Timing-konfigurasjoner (kan justeres etter behov)
#define MOLE_DISPLAY_TIME 3000           // Tid for at en mole vises (ms)
#define MOLE_DISPLAY_DURATION 3000       // Varighet mole er aktiv (ms)
#define GAME_SPEED 2000                   // Hastighet på spillet (ms)

// Pinner for 7-segments display (MAX7219-driver)
const int dataPin = 8;
const int clkPin = 0;
const int csPin = 1;

#endif