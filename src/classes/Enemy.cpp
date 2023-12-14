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

void Enemy::drawEnemy()
{
    if (type == 0)
    {
        LCD->fillRect((x - 15) + xOffset, (y - 15) + yOffset, 30, 30, ILI9341_BLACK);
    }
    else if (type == 1)
    {
        LCD->fillRect((x - 15) + xOffset, (y - 15) + yOffset, 30, 30, ILI9341_RED);
        LCD->fillRect((x - 19) + xOffset, (y - 15) + yOffset, 4, 30, ILI9341_BLACK);
    }
}
/**
 * @brief Clears the screen where the enemies are, this is used when the enemies move to the next row
 * We do this because the enemies leave some pixels behind when they move a row down
 * TODO: find a better solition for this, the current method causes a little lagspike
 */
void Enemy::moveRowFix()
{
    LCD->fillRect(0, 0, 230, 230, ILI9341_BLACK);
}

static void Enemy::moveEnemy(Enemy (*enemiesArray)[5], uint8_t timemovement, uint8_t maxTimeMovement)
{
    {
        // Check if enemies can't move horizontally anymore
        if (timemovement == (maxTimeMovement - 1))
        {
            // Move enemies to the next row
            enemiesArray[0][0].moveRowFix();
            for (uint8_t j = 3; j > 0; j--)
            {
                for (uint8_t i = 0; i < 5; i++)
                {
                    //copy row above
                    enemiesArray[j][i].setType(enemiesArray[j - 1][i].getType());
                }
            }
              for (uint8_t i = 0; i < 5; i++)
                {
                    //set type of new row
                    //TODO: make this random
                    enemiesArray[0][i].setType(1);
                }
            // Reset timemovement
            timemovement = 0;
        }
        // Move enemies horizontally
        for (uint8_t j = 0; j < 4; j++)
        {
            for (uint8_t i = 0; i < 5; i++)
            {
                enemiesArray[j][i].setXOffset((i * 40) + (4 * timemovement));
                enemiesArray[j][i].setYOffset((j * 50));
                enemiesArray[j][i].drawEnemy();
            }
        }
    }
}

// setters
void Enemy::setType(uint8_t type)
{
    Enemy::type = type;
}

void Enemy::setXOffset(uint8_t newXOffset)
{
    xOffset = newXOffset;
}

void Enemy::setYOffset(uint16_t newYOffset)
{
    yOffset = newYOffset;
}

// getters

uint8_t Enemy::getXPosition()
{
    return (x - 15) + xOffset;
}

uint16_t Enemy::getYPosition()
{
    return (y - 15) + yOffset;
}

uint8_t Enemy::getType()
{
    return type;
}