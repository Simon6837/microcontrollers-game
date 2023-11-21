#include "Player.h"

Player::Player(int initialX, int initialY, Adafruit_ILI9341 *lcd)
{
    x = initialX;
    y = initialY;
    LCD = lcd;
}

void Player::drawPlayer()
{
    LCD->fillCircle(x, y, 20, ILI9341_WHITE);
}

void Player::clearPlayer()
{
    LCD->fillCircle(x, y, 20, ILI9341_BLACK);
}

void Player::moveLeft()
{
    clearPlayer();
    x -= 10;
    drawPlayer();
}

void Player::moveRight()
{
    clearPlayer();
    x += 10;
    drawPlayer();
}
