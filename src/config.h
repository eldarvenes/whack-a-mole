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

// Config for reaksjonsspill
const int player1MolePins[2] = {9, 13}; // To LED-er for spiller 1
const int player1ButtonPins[2] = {2, 6}; // To knapper for spiller 1

const int player2MolePins[2] = {12, 10}; // To LED-er for spiller 2
const int player2ButtonPins[2] = {5, 4}; // To knapper for spiller 2

#define WHACK_A_MOLE_HIGHSCORE_ADDR 0    // EEPROM-adresse for Whack-a-Mole
#define MEMORY_HIGHSCORE_ADDR 4         // EEPROM-adresse for Memory

#endif