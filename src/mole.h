#ifndef MOLE_H
#define MOLE_H

class Mole {
public:
    Mole(int ledPin, int buttonPin);
    void show();
    void hide();
    bool isVisible();
    bool isHit();

private:
    int ledPin;
    int buttonPin;
    bool visible;
};

#endif