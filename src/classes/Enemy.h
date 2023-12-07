#ifndef ENEMY_H
#define ENEMY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

class Enemy
{
private:
    uint8_t x;
    uint16_t y;
    Adafruit_ILI9341 *LCD;
    uint8_t type;
public:
    Enemy(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd, uint8_t type);
    void drawEnemy(uint8_t xOffset, uint16_t yOffset);
};


#endif