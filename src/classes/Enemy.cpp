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
        LCD->fillRect((x - 15) + xOffset, (y - 15) + yOffset, 30, 30, ILI9341_RED);
    }
}

static void Enemy::moveEnemy(Enemy (*enemiesArray)[5], uint8_t timemovement)
{
    {
        // Move enemies horizontally
        for (uint8_t j = 0; j < 4; j++)
        {
            for (uint8_t i = 0; i < 5; i++)
            {
                enemiesArray[j][i].drawEnemy((i * 40) + (4 * timemovement), (j * 50));
            }
        }

        // Check if enemies can't move horizontally anymore
        if (timemovement == 4)
        {
            // Move enemies to the next row
            for (uint8_t j = 3; j > 0; j--)
            {
                for (uint8_t i = 0; i < 5; i++)
                {
                    enemiesArray[j][i] = enemiesArray[j - 1][i];
                }
            }

            // Fill the first row with enemy0
            for (uint8_t i = 0; i < 5; i++)
            {
                // enemies[0][i] = enemy0;
            }

            // Reset timemovement
            timemovement = 0;
        }
    }
}

uint8_t Enemy::getXPosition()
{
    return x;
}

uint16_t Enemy::getYPosition()
{
    return y;
}