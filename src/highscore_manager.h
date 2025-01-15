#include <EEPROM.h>
#include "config.h"

class HighscoreManager {
public:
    static void updateHighScore(int score, int eepromAddress) {
        int currentHighScore;
        EEPROM.get(eepromAddress, currentHighScore);

        if (score > currentHighScore) {
            EEPROM.put(eepromAddress, score);
            
        }
    }

    static int getHighScore(int eepromAddress) {
        int highScore;
        EEPROM.get(eepromAddress, highScore);
        return highScore;
    }

    static void resetAllHighScores() {
        EEPROM.put(WHACK_A_MOLE_HIGHSCORE_ADDR, 0);
        EEPROM.put(MEMORY_HIGHSCORE_ADDR, 0);
    }

    static void displayAllHighScores() {
        
        // returner highscore for alle spill
    }
};