#include "Player.h"
#include "Bullet.h"

#define PLAYER_SPEED 2          // the speed at which the player moves
#define BULLET_Y_OFFSET 10      // the offset from the player's y position for creating bullets
#define BULLET_Y_TRESHOLD 200   // the y position at which the player can't create bullets
#define BULLET_AMOUNT_ALLOWED 2 // the amount of bullets the player can have on the screen at once

// can't move past these edges, so the player doesn't go off the screen
// based on the size of the player and the size of the screen
#define LEFT_EDGE 10
#define RIGHT_EDGE 220

// values to draw the player
#define PLAYER_HEIGHT 4
#define PLAYER_WIDTH 5
#define PLAYER_OFFSET 20

uint8_t liveDisplay[4] {0xFF, 0xFE, 0xBE, 0xB6}; //hexvalues for {0, 1, 2, 3} lives

const uint16_t playerPixels[PLAYER_HEIGHT][PLAYER_WIDTH] = {
    {ILI9341_BLACK, ILI9341_BLACK, ILI9341_BLACK, ILI9341_BLACK, ILI9341_BLACK},
    {ILI9341_BLACK, ILI9341_LIGHTGREY, ILI9341_GREEN, ILI9341_LIGHTGREY, ILI9341_BLACK},
    {ILI9341_BLACK, ILI9341_LIGHTGREY, ILI9341_LIGHTGREY, ILI9341_LIGHTGREY, ILI9341_BLACK},
    {ILI9341_BLACK, ILI9341_BLACK, ILI9341_BLACK, ILI9341_BLACK, ILI9341_BLACK},
};
Player::Player(uint8_t initialX, uint16_t initialY,uint8_t initialLives, Adafruit_ILI9341 *lcd, NunchukController *nunchukCtrl, BulletList *newbulletList, bool *playerIsMovingValue)
{
    x = initialX;
    y = initialY;
    lives = initialLives;
    LCD = lcd;
    nunchukController = nunchukCtrl;
    bulletList = newbulletList;
    playerIsMoving = playerIsMovingValue;
}
/**
 * @brief Draws the player on the screen
 * @note This uses the playerPixels array to draw the player
 */
void Player::drawPlayer()
{
    for (int drawY = 0; drawY < PLAYER_HEIGHT; ++drawY)
    {
        for (int drawX = 0; drawX < PLAYER_WIDTH; ++drawX)
        {
            // Draw pixel using the color value in the array
            //TODO: Remove turning off the interupt and just turn off the timer
            cli();
            // TCCR1B ^= (1 << CS12); //stoping all other drawing
            LCD->fillRect((x + 10*drawX) - PLAYER_OFFSET, (y + 10*drawY), 10, 10, playerPixels[drawY][drawX]);
            // TCCR1B ^= (1 << CS12); //restart all other drawing
            sei();
        }
    }
}
/**
 * @brief Clears the player from the screen
 * @note This is used to remove the player from the screen before moving it
 * TODO: This function is not working properly(when increasing the player's speed, the player is not cleared properly and leaves a trail)
 */
void Player::clearPlayer()
{
//     for (int drawY = 0; drawY < PLAYER_HEIGHT; ++drawY)
//     {
//         // remove the extra pixels on the right side of the player

//         for (int drawX = 21; drawX < PLAYER_WIDTH + PLAYER_SPEED; ++drawX)
//         {
//             // Draw pixel using the color value in the array
//             cli();
//             LCD->drawPixel((x + drawX) - PLAYER_OFFSET + 2, (y + drawY), ILI9341_BLACK);
//             sei();
//         }
//         // remove the extra pixels on the left side of the player
//         for (int drawX = 0; drawX < (PLAYER_WIDTH - 21); ++drawX)
//         {
//             // Draw pixel using the color value in the array
//             cli();
//             LCD->drawPixel((x + drawX) - PLAYER_OFFSET - 2, (y + drawY), ILI9341_BLACK);
//             sei();
//         }
//     }
}
/**
 * @brief Moves the player to the left
 * @note The player can't move past the left edge of the screen
 */
void Player::moveLeft()
{
    if (x <= LEFT_EDGE)
        return;
    x -= PLAYER_SPEED;
    drawPlayer();
    clearPlayer();
}
/**
 * @brief Moves the player to the right
 * @note The player can't move past the right edge of the screen
 */
void Player::moveRight()
{
    if (x >= RIGHT_EDGE)
        return;
    x += PLAYER_SPEED;
    drawPlayer();
    clearPlayer();
}
/**
 * @brief Controls the player based on the Nunchuk's joystick and Z button
 * @note The joystick controls the player's movement and the Z button creates bullets
 */
void Player::controlPlayer()
{
    if (nunchukController->updatePlayerControl() == 0)
    {
        moveLeft();
        playerIsMoving = true;
    }
    else if (nunchukController->updatePlayerControl() == 1)
    {
        moveRight();
        playerIsMoving = true;
    }
    else
        playerIsMoving = false;
    if (nunchukController->isZButtonPressed() && bulletList->getBulletCount() < BULLET_AMOUNT_ALLOWED && bulletList->getLastButtonYPosition() < BULLET_Y_TRESHOLD)
        bulletList->addBullet(new Bullet(x, y - BULLET_Y_OFFSET, LCD));
}

/**
 * @brief Uses the 7-segment display to display the amount of lives using stripes (e.g 2 stripes means the player has only 2 lives)
*/
void Player::displayLives()
{
    Wire.beginTransmission(0X21);
    Wire.write(liveDisplay[lives]); //sends hex value to 7-segment display corresponding with the amount of player lives
    Wire.endTransmission();
}