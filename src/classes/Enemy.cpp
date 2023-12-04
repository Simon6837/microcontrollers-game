#include "Enemy.h"

#define Enemy_speed 10
// #define BULLET_AMOUNT_ALLOWED 1

Enemy::Enemy(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd, uint8_t type)
{
    x = initialX;
    y = initialY;
    LCD = lcd;
    Enemy::type = type;
}

void Enemy::drawEnemy(uint8_t xOffset, uint16_t yOffset)
{
    if (type == 0)
    {
    }
    else if (type == 1)
    {
        LCD->fillRect((x - 15) + xOffset, (y - 15) + yOffset, 30, 30, ILI9341_WHITE);
    }
}

void Enemy::killEnemy(){
    Enemy::type = 0;
}
