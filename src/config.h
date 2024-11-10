#ifndef CONFIG_H
#define CONFIG_H

// Antall muldvarper
#define NUM_MOLES 5

// Pin-konfigurasjoner
const int molePins[NUM_MOLES] = {2, 3, 4, 5, 6}; 
const int buttonPins[NUM_MOLES] = {7, 8, 9, 10, 11};

// Timing-konfigurasjoner
#define MOLE_DISPLAY_TIME 1000
#define GAME_SPEED 500

#endif