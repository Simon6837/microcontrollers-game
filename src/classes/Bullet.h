#ifndef BULLET_H
#define BULLET_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

class Bullet
{
private:
    uint8_t x;
    uint16_t y;
    Adafruit_ILI9341 *LCD;

public:
    Bullet(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd);
    void drawBullet();
    void clearBullet();
    uint16_t getYPosition();
    uint8_t getXPosition();
    // moves the bullet up
    void moveUp(uint8_t speed);
    virtual ~Bullet();
};

#endif
