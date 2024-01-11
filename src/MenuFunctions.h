#ifndef MENU_FUNCTIONS_H
#define MENU_FUNCTIONS_H

#include <Adafruit_ILI9341.h>
#include "classes/NunchukController.h"
// Add any necessary additional includes

void showMenu(Adafruit_ILI9341 &LCD);
void flickerButton(Adafruit_ILI9341 &LCD);
void allowGameStart();
void menuControlsEnable(NunchukController &nunchukController, Adafruit_ILI9341 &LCD);
void gameOver(Adafruit_ILI9341 &LCD, uint16_t finalScore);
void dismissGameOver(NunchukController &nunchukController, Adafruit_ILI9341 &LCD);
// Add any other function declarations as needed

#endif
