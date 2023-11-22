#ifndef PLAYER_H
#define PLAYER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "NunchukController.h"

class Player
{
private:
    uint8_t x;
    uint16_t y;
    Adafruit_ILI9341 *LCD;
    NunchukController *nunchukController;

public:
    Player(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd, NunchukController *nunchukController);
    void drawPlayer();
    void clearPlayer();
    void moveLeft();
    void moveRight();
    void controlPlayer();
};

#endif
