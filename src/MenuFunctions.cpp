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
const uint8_t buttonWidth = 112;
const uint8_t buttonHeight = 40;
const uint8_t flipButtonWidth = 50;
const uint8_t flipButtonHeight = 30;
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
    LCD.drawChar(40, 20, 'S', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(52, 20, 'P', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(64, 20, 'A', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(76, 20, 'C', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(88, 20, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(120, 20, 'D', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(132, 20, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(144, 20, 'F', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(156, 20, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(168, 20, 'N', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(180, 20, 'D', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(192, 20, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(204, 20, 'R', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    // Single player option
    LCD.drawRect(64, 60, buttonWidth, buttonHeight, ILI9341_WHITE);
    LCD.drawChar(76, 75, 'S', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(88, 75, 'I', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(100, 75, 'N', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(112, 75, 'G', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(124, 75, 'L', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(136, 75, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    // Versus option
    LCD.drawRect(64, 120, buttonWidth, buttonHeight, ILI9341_WHITE);
    LCD.drawChar(76, 135, 'V', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(88, 135, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(100, 135, 'R', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(112, 135, 'S', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(124, 135, 'U', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(136, 135, 'S', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    // Coop option
    LCD.drawRect(64, 180, buttonWidth, buttonHeight, ILI9341_WHITE);
    LCD.drawChar(76, 195, 'C', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(88, 195, 'O', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(100, 195, '-', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(112, 195, 'O', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(124, 195, 'P', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    // share score option
    LCD.drawRect(64, 240, buttonWidth, buttonHeight, ILI9341_WHITE);
    LCD.drawChar(76, 255, 'S', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(88, 255, 'H', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(100, 255, 'A', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(112, 255, 'R', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(124, 255, 'E', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    // Flip screen option
    LCD.drawRect(190, 290, flipButtonWidth, flipButtonHeight, ILI9341_WHITE);
    LCD.drawChar(195, 295, 'F', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(205, 295, 'L', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(215, 295, 'I', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
    LCD.drawChar(225, 295, 'P', ILI9341_WHITE, ILI9341_WHITE, 2, 2);
}

/**
 *@brief makes the button flicker when selected
 */
void flickerButton(Adafruit_ILI9341 &LCD, uint8_t flickerCount, uint8_t xPosition, uint16_t yPosition)
{
    for (size_t i = 0; i < flickerCount; i++)
    {
        LCD.drawRect(xPosition, yPosition, buttonWidth, buttonHeight, ILI9341_RED);
        LCD.drawRect(xPosition, yPosition, buttonWidth, buttonHeight, ILI9341_WHITE);
    }
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
void menuControlsEnable(NunchukController &nunchukController, Adafruit_ILI9341 &LCD, Score &score)
{
    if (menuState < 0)
    {
        menuState = 79;
    }
    else if (menuState > 79)
    {
        menuState = 0;
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
    // Single player option
    if (menuState < 16)
    {
        LCD.drawRect(64, 60, buttonWidth, buttonHeight, ILI9341_YELLOW);
        LCD.drawRect(64, 120, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 180, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 240, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(190, 290, flipButtonWidth, flipButtonHeight, ILI9341_WHITE);
        if (nunchukController.isZButtonPressed() == true)
        {
            allowGameStart();
        }
    }
    // Verses option
    else if (menuState > 15 && menuState < 32)
    {
        LCD.drawRect(64, 60, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 120, buttonWidth, buttonHeight, ILI9341_GREEN);
        LCD.drawRect(64, 180, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 240, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(190, 290, flipButtonWidth, flipButtonHeight, ILI9341_WHITE);
    }
    // coop option
    else if (menuState > 31 && menuState < 48)
    {
        LCD.drawRect(64, 60, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 120, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 180, buttonWidth, buttonHeight, ILI9341_YELLOW);
        LCD.drawRect(64, 240, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(190, 290, flipButtonWidth, flipButtonHeight, ILI9341_WHITE);
        if (nunchukController.isZButtonPressed() == true)
        {
            flickerButton(LCD, 5, 64, 180);
        }
    }
    // share score option
    else if (menuState > 47 && menuState < 63)
    {
        LCD.drawRect(64, 60, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 120, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 180, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 240, buttonWidth, buttonHeight, ILI9341_GREEN);
        LCD.drawRect(190, 290, flipButtonWidth, flipButtonHeight, ILI9341_WHITE);
        if (nunchukController.isCButtonPressed() == true)
        {
            // reset high score
            score.resetHighScore();
            flickerButton(LCD, 10, 64, 240);
        }
    }
    else if (menuState > 63)
    {
        LCD.drawRect(64, 60, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 120, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 180, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(64, 240, buttonWidth, buttonHeight, ILI9341_WHITE);
        LCD.drawRect(190, 290, flipButtonWidth, flipButtonHeight, ILI9341_YELLOW);
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
void gameOver(Adafruit_ILI9341 &LCD)
{
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
