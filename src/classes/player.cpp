#include "Player.h"
#include "Bullet.h"
#define PLAYER_SPEED 2
#define BULLET_Y_OFFSET 30
// can't move past these edges, so the player doesn't go off the screen
// based on the size of the player and the size of the screen
#define LEFT_EDGE 20
#define RIGHT_EDGE 220

Player::Player(uint8_t initialX, uint16_t initialY, Adafruit_ILI9341 *lcd, NunchukController *nunchukCtrl, BulletList *newbulletList)
{
    x = initialX;
    y = initialY;
    LCD = lcd;
    nunchukController = nunchukCtrl;
    bulletList = newbulletList;
}

void Player::drawPlayer()
{
    // TODO: this function creates lag, fix it
    LCD->fillCircle(x, y, 20, ILI9341_WHITE);
}

void Player::clearPlayer()
{
    // TODO: this function creates lag, fix it
    LCD->fillCircle(x, y, 20, ILI9341_BLACK);
}

void Player::moveLeft()
{
    if (x <= LEFT_EDGE)
        return;
    clearPlayer();
    x -= PLAYER_SPEED;
    drawPlayer();
}

void Player::moveRight()
{
    if (x >= RIGHT_EDGE)
        return;
    clearPlayer();
    x += PLAYER_SPEED;
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
    if (nunchukController->isZButtonPressed() && bulletList->getBulletCount() < 3)
    {
        bulletList->addBullet(new Bullet(x, y - BULLET_Y_OFFSET, LCD));
    }
}
