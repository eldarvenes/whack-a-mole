// include/arduino_mocks.h
#ifndef ARDUINO_MOCKS_H
#define ARDUINO_MOCKS_H

#include <stdint.h>
#include <map>

#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0

// Simuler tilstand på pinner
std::map<int, int> pin_states;

void pinMode(int pin, int mode) {
    // Mock funksjonen, ingen implementering nødvendig for testing
}

void digitalWrite(int pin, int value) {
    pin_states[pin] = value;
}

int digitalRead(int pin) {
    return pin_states[pin];
}

unsigned long millis() {
    // En enkel teller for simulert tid, du kan utvide dette for bedre simulering
    static unsigned long simulated_time = 0;
    simulated_time += 100;
    return simulated_time;
}

#endif // ARDUINO_MOCKS_H