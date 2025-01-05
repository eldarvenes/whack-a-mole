#ifndef ABSTRACT_GAME_H
#define ABSTRACT_GAME_H

class AbstractGame {
public:
    enum GameState {
        RUNNING,
        GAME_OVER
    };
    virtual void init() = 0;            // Initialiser spillet
    virtual void update() = 0;          // Oppdater spillet
    virtual void endGame() = 0;         // Avslutt spillet
    virtual GameState getState() const = 0;
    virtual void waitForRestart() = 0;
    virtual ~AbstractGame() {}          // Virtuell destruktor for sikkerhet
};

#endif