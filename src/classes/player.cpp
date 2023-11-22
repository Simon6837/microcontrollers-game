#include "Player.h"
#define playerSpeed 2
// can't move past these edges, so the player doesn't go off the screen
// based on the size of the player and the size of the screen
#define LEFT_EDGE 20
#define RIGHT_EDGE 220

Player::Player(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd, NunchukController *nunchukCtrl)
{
    x = initialX;
    y = initialY;
    LCD = lcd;
    nunchukController = nunchukCtrl;
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
    if (x <= LEFT_EDGE)
        return;
    clearPlayer();
    x -= playerSpeed;
    drawPlayer();
}

void Player::moveRight()
{
    if (x >= RIGHT_EDGE)
        return;
    clearPlayer();
    x += playerSpeed;
    drawPlayer();
}

void Player::controlPlayer()
{
    if (nunchukController->updatePlayerControl() == 0)
    {
        moveLeft();
    }
    else if (nunchukController->updatePlayerControl() == 1)
    {
        moveRight();
    }
}
