#include "Bullet.h"
#define bulletSpeed 5
#define bulletWidth 6
#define bulletHeight 12

Bullet::Bullet(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd)
{
    x = initialX;
    y = initialY;
    LCD = lcd;
}

void Bullet::clearBullet()
{
    LCD->fillRect(x, y, bulletWidth, bulletHeight, ILI9341_BLACK);
}

void Bullet::drawBullet()
{
    LCD->fillRect(x, y, bulletWidth, bulletHeight, ILI9341_YELLOW);
}

uint16_t Bullet::getYPosition()
{
    return y;
}

uint8_t Bullet::getXPosition()
{
    return x;
}

void Bullet::moveUp(uint8_t speed)
{
    clearBullet();
    y -= speed;
    drawBullet();
}

Bullet::~Bullet()
{
    clearBullet();
}