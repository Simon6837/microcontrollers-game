#ifndef PLAYER_H
#define PLAYER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "NunchukController.h"
#include "BulletList.h"

class Player
{
private:
    uint8_t x;
    uint16_t y;
    uint8_t lives;
    Adafruit_ILI9341 *LCD;
    NunchukController *nunchukController;
    BulletList *bulletList;
    bool playerIsMoving;

public:
    Player(uint8_t initialX, uint16_t initialY,uint8_t lives, Adafruit_ILI9341 *lcd, NunchukController *nunchukController, BulletList *bulletList, bool *playerIsMovingValue);
    void drawPlayer();
    void clearPlayer();
    void moveLeft();
    void moveRight();
    void controlPlayer();
    void displayLives();
};

#endif
