#ifndef MOLEGAMESTARTER_H
#define MOLEGAMESTARTER_H

#include "mole.h"
#include "config.h"

class MoleGameStarter {
public:
    MoleGameStarter(Mole* moles[], int numMoles);  // Konstruktør
    void startSequence(int BuzzerPin);  // Metode for oppstartssekvens

private:
    Mole** moles;  // Array med mole-pekere
    int numMoles;  // Antall moles
};

#endif