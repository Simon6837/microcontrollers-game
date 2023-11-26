#include "Player.h"
#include "Bullet.h"

#define PLAYER_SPEED 2          // the speed at which the player moves
#define BULLET_Y_OFFSET 35      // the offset from the player's y position for creating bullets
#define BULLET_Y_TRESHOLD 200   // the y position at which the player can't create bullets
#define BULLET_AMOUNT_ALLOWED 2 // the amount of bullets the player can have on the screen at once

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
    // TODO: this function creates lag, fix it (creating the player per pixel should be faster)
    LCD->fillCircle(x, y, 20, ILI9341_WHITE);
}

void Player::clearPlayer()
{
    // TODO: this function creates lag, fix it (deleting the player per pixel should be faster)
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
/**
 * @brief Controls the player based on the Nunchuk's joystick and Z button
 * @note The joystick controls the player's movement and the Z button creates bullets
 */
void Player::controlPlayer()
{
    if (nunchukController->updatePlayerControl() == 0)
        moveLeft();
    else if (nunchukController->updatePlayerControl() == 1)
        moveRight();
    if (nunchukController->isZButtonPressed() && bulletList->getBulletCount() < BULLET_AMOUNT_ALLOWED && bulletList->getLastButtonYPosition() < BULLET_Y_TRESHOLD)
        bulletList->addBullet(new Bullet(x, y - BULLET_Y_OFFSET, LCD));
}
