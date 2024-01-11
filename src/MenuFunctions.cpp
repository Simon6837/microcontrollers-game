#include "MenuFunctions.h"

enum gameStates
{
    MENU,
    SOLO,
    GAMEOVER
};
extern volatile gameStates gameState;
extern volatile int8_t menuState;
extern bool rotationState;
extern bool allowGameToStart;
volatile uint16_t finalScore;
char finalScoreDigit1;
char finalScoreDigit2;
char finalScoreDigit3;
char finalScoreDigit4;


/**
 *@brief shows the menu screen with the options to play single player, versus, or coop
 * and a button to flip the screen
 */
void showMenu(Adafruit_ILI9341 &LCD)
{
    gameState = MENU;
    LCD.fillScreen(ILI9341_BLACK);
    // stop timer 1
    TIMSK1 &= ~(1 << OCIE1A);
    // title
    LCD.drawChar(40, 40, 'S', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(52, 40, 'P', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(64, 40, 'A', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(76, 40, 'C', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(88, 40, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(120, 40, 'D', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(132, 40, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(144, 40, 'F', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(156, 40, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(168, 40, 'N', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(180, 40, 'D', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(192, 40, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(204, 40, 'R', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    // Single player option
    LCD.drawRect(64, 100, 112, 40, ILI9341_WHITE);
    LCD.drawChar(76, 115, 'S', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(88, 115, 'I', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(100, 115, 'N', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(112, 115, 'G', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(124, 115, 'L', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(136, 115, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    // Versus option
    LCD.drawRect(64, 160, 112, 40, ILI9341_WHITE);
    LCD.drawChar(76, 175, 'V', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(88, 175, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(100, 175, 'R', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(112, 175, 'S', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(124, 175, 'U', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(136, 175, 'S', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    // Coop option
    LCD.drawRect(64, 220, 112, 40, ILI9341_WHITE);
    LCD.drawChar(76, 235, 'C', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(88, 235, 'O', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(100, 235, '-', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(112, 235, 'O', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(124, 235, 'P', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    // Flip screen option
    LCD.drawRect(190, 290, 50, 30, ILI9341_WHITE);
    LCD.drawChar(195, 295, 'F', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(205, 295, 'L', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(215, 295, 'I', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(225, 295, 'P', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
}

/**
 *@brief makes the button flicker when selected
 */
void flickerButton(Adafruit_ILI9341 &LCD)
{
    LCD.drawRect(64, 220, 112, 40, ILI9341_RED);
    LCD.drawRect(64, 220, 112, 40, ILI9341_WHITE);
    LCD.drawRect(64, 220, 112, 40, ILI9341_RED);
    LCD.drawRect(64, 220, 112, 40, ILI9341_WHITE);
    LCD.drawRect(64, 220, 112, 40, ILI9341_RED);
}

/**
 *@brief allow the game to start from the main loop
 */
void allowGameStart()
{
    allowGameToStart = true;
}

/**
 *@brief controls the menu cursor so the player can select a game mode
 */
void menuControlsEnable(NunchukController &nunchukController, Adafruit_ILI9341 &LCD)
{
    if (menuState < 0)
    {
        menuState = 0;
    }
    else if (menuState > 63)
    {
        menuState = 63;
    }
    // prevents cursor from moving when holding Z
    if (nunchukController.isZButtonPressed() == false)
    {
        if (nunchukController.updateMenu() == 0)
        {
            menuState++;
        }
        if (nunchukController.updateMenu() == 1)
        {
            menuState--;
        }
    }
    if (menuState < 16)
    {
        LCD.drawRect(64, 100, 112, 40, ILI9341_YELLOW);
        LCD.drawRect(64, 160, 112, 40, ILI9341_WHITE);
        LCD.drawRect(64, 220, 112, 40, ILI9341_WHITE);
        LCD.drawRect(190, 290, 50, 30, ILI9341_WHITE);
        if (nunchukController.isZButtonPressed() == true)
        {
            allowGameStart();
        }
    }
    if (menuState > 15 && menuState < 32)
    {
        LCD.drawRect(64, 100, 112, 40, ILI9341_WHITE);
        LCD.drawRect(64, 160, 112, 40, ILI9341_GREEN);
        LCD.drawRect(64, 220, 112, 40, ILI9341_WHITE);
        LCD.drawRect(190, 290, 50, 30, ILI9341_WHITE);
    }
    if (menuState > 31 && menuState < 48)
    {
        LCD.drawRect(64, 100, 112, 40, ILI9341_WHITE);
        LCD.drawRect(64, 160, 112, 40, ILI9341_WHITE);
        LCD.drawRect(64, 220, 112, 40, ILI9341_YELLOW);
        LCD.drawRect(190, 290, 50, 30, ILI9341_WHITE);
        if (nunchukController.isZButtonPressed() == true)
        {
            flickerButton(LCD);
        }
    }
    if (menuState > 47)
    {
        LCD.drawRect(64, 100, 112, 40, ILI9341_WHITE);
        LCD.drawRect(64, 160, 112, 40, ILI9341_WHITE);
        LCD.drawRect(64, 220, 112, 40, ILI9341_WHITE);
        LCD.drawRect(190, 290, 50, 30, ILI9341_YELLOW);
        if (nunchukController.isZButtonPressed() == true)
        {
            {
                if (rotationState == false)
                {
                    rotationState = true;
                    LCD.setRotation(0);
                    LCD.fillScreen(ILI9341_BLACK);
                    showMenu(LCD);
                }
                else
                {
                    rotationState = false;
                    LCD.setRotation(2);
                    LCD.fillScreen(ILI9341_BLACK);
                    showMenu(LCD);
                }
            }
        }
    }
}

/**
 *@brief clears screen to show a game over message
 */
void gameOver(Adafruit_ILI9341 &LCD, uint16_t finalScore)
{
    finalScoreDigit1 = char(finalScore / 1000 % 10);
    finalScoreDigit2 = char(finalScore / 100 % 10);
    finalScoreDigit3 = char(finalScore / 10 % 10);
    finalScoreDigit4 = char(finalScore % 10);
    gameState = GAMEOVER;
    TIMSK1 &= ~(1 << OCIE1A);
    LCD.fillScreen(ILI9341_WHITE);
    LCD.fillScreen(ILI9341_BLACK);
    LCD.drawChar(50, 40, 'G', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
    LCD.drawChar(90, 40, 'A', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
    LCD.drawChar(130, 40, 'M', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
    LCD.drawChar(170, 40, 'E', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
    LCD.drawChar(50, 90, 'O', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
    LCD.drawChar(90, 90, 'V', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
    LCD.drawChar(130, 90, 'E', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
    LCD.drawChar(170, 90, 'R', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
    LCD.drawChar(40, 180, 'Y', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(52, 180, 'O', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(64, 180, 'U', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(76, 180, 'R', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(93, 180, 'S', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(105, 180, 'C', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(117, 180, 'O', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(129, 180, 'R', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(141, 180, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    if (finalScoreDigit1 !=0) {
        LCD.drawChar(159, 180, finalScoreDigit1+48, ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    }
    LCD.drawChar(171, 180, finalScoreDigit2+48, ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(183, 180, finalScoreDigit3+48, ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(195, 180, finalScoreDigit4+48, ILI9341_WHITE, ILI9341_WHITE, 2, 2);
}

/**
 * @brief checks if the Z button is pressed to dismiss the game over screen
 */
void dismissGameOver(NunchukController &nunchukController, Adafruit_ILI9341 &LCD)
{
    if (nunchukController.isZButtonPressed() == true)
    {
        showMenu(LCD);
    }
}
