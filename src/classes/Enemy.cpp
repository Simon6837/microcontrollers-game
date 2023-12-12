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

void Enemy::moveEnemy(Enemy enemies[4][5], uint8_t timemovement, Enemy enemy0)
{
    {
        // Move enemies horizontally
        for (uint8_t j = 0; j < 4; j++)
        {
            for (uint8_t i = 0; i < 5; i++)
            {
                enemies[j][i].drawEnemy((i * 40) + (4 * timemovement), (j * 50));
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
                    enemies[j][i] = enemies[j - 1][i];
                }
            }

            // Fill the first row with enemy0
            for (uint8_t i = 0; i < 5; i++)
            {
                enemies[0][i] = enemy0;
            }

            // Reset timemovement
            timemovement = 0;
        }
    }
}