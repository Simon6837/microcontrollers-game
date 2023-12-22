#include "Score.h"
#include <EEPROM.h>

Score::Score(Adafruit_ILI9341 *lcd)
{
    LCD = lcd;
    if (EEPROM.read(0) == 255 && EEPROM.read(1) == 255)
    {
        EEPROM.write(0, 0);
        EEPROM.write(1, 0);
    }
}
/**
 * @brief Increases the score by 1 and updates the highscore if needed
 */
void Score::increaseScore()
{
    score += 1;
    //? mabye only update highscore on player death?
    setHighscore();
    LCD->fillRect(0, 0, 320, 20, ILI9341_BLACK);
    displayScore();
}
/**
 * @brief Sets the highscore if the current score is higher than the highscore
 * highscore will be saved in EEPROM
 */
void Score::setHighscore()
{
    if (score > getHighscore())
    {
        byte lowByte = score & 0xFF;         // Get the low byte of the score
        byte highByte = (score >> 8) & 0xFF; // Get the high byte of the score
        EEPROM.write(0, lowByte);            // Write the low byte to address 0
        EEPROM.write(1, highByte);           // Write the high byte to address 1
    }
}

uint16_t Score::getHighscore()
{
    byte lowByte = EEPROM.read(0);  // Read the low byte from address 0
    byte highByte = EEPROM.read(1); // Read the high byte from address 1
    return (highByte << 8) | lowByte;
}

/**
 * @brief Displays the score and the highscore on the top off the screen
 */
void Score::displayScore()
{
    LCD->setCursor(0, 0);
    LCD->setTextColor(ILI9341_WHITE);
    LCD->setTextSize(1);
    LCD->print("Score: ");
    LCD->print(score);
    LCD->setCursor(150, 0);
    LCD->print("Highscore: ");
    LCD->print(getHighscore());
}
