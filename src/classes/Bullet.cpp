#include "Bullet.h"
#define bulletSpeed 5

Bullet::Bullet(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd)
{
    x = initialX;
    y = initialY;
    LCD = lcd;
}

void Bullet::clearBullet()
{
    LCD->fillRect(x, y, 6, 12, ILI9341_BLACK);
}

void Bullet::drawBullet()
{
    LCD->fillRect(x, y, 6, 12, ILI9341_YELLOW);
}

uint16_t Bullet::getYPosition()
{
    return y;
}

void Bullet::moveUp()
{
    clearBullet();
    y -= bulletSpeed;
    drawBullet();
}

Bullet::~Bullet()
{
    clearBullet();
}