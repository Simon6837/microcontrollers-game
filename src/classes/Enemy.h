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
public:
    Enemy(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd);
    void drawEnemy();
};


#endif