#include "Enemy.h"

#define Enemy_speed 10
// #define BULLET_AMOUNT_ALLOWED 1

Enemy::Enemy(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd){
    x = initialX;
    y = initialY;
    LCD = lcd;
}

void Enemy::drawEnemy(){
    LCD->fillRect(x-20, y-20, 40 , 40, ILI9341_WHITE);
}

 