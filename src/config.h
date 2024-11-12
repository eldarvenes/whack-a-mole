#ifndef CONFIG_H
#define CONFIG_H

// Antall muldvarper
#define NUM_MOLES 2

// Pin-konfigurasjoner
const int molePins[NUM_MOLES] = {2, 3}; 
const int buttonPins[NUM_MOLES] = {4, 5};

// Timing-konfigurasjoner
#define MOLE_DISPLAY_TIME 1000
#define MOLE_DISPLAY_DURATION 1000
#define GAME_SPEED 500

#endif