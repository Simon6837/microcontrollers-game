#ifndef MENU_FUNCTIONS_H
#define MENU_FUNCTIONS_H

#include <Adafruit_ILI9341.h>
#include "classes/NunchukController.h"
#include "classes/Score.h"
// Add any necessary additional includes

void showMenu(Adafruit_ILI9341 &LCD);
void flickerButton(Adafruit_ILI9341 &LCD, uint8_t flickerCount, uint8_t xPosition, uint16_t yPosition);
void allowGameStart();
void menuControlsEnable(NunchukController &nunchukController, Adafruit_ILI9341 &LCD, Score &score);
void gameOver(Adafruit_ILI9341 &LCD);
void dismissGameOver(NunchukController &nunchukController, Adafruit_ILI9341 &LCD);
// Add any other function declarations as needed

#endif
