#ifndef ENEMY_H
#define ENEMY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
extern const uint8_t maxEnemyRows;
extern const uint8_t maxEnemyColumns;
class Enemy
{
private:
    uint8_t x;
    uint16_t y;
    uint8_t xOffset;
    uint16_t yOffset;
    Adafruit_ILI9341 *LCD;
    uint8_t type;
public:
    Enemy(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd, uint8_t type);
    void drawEnemy();
    void moveRowFix();
    uint8_t getXPosition();
    uint16_t getYPosition();
    static void moveEnemy(Enemy (*enemiesArray)[5], uint8_t timemovement, uint8_t maxTimeMovement);
    uint8_t getType();
    void setType(uint8_t newType);
    void setXOffset(uint8_t newXOffset);
    void setYOffset(uint16_t newYOffset);
    static uint8_t getRandomType();
};


#endif