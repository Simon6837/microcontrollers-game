// libraries
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <HardwareSerial.h>
// classes
#include "classes/Player.h"
#include "classes/BulletList.h"
#include "classes/NunchukController.h" // Include the new header
// pins for the screen
#define TFT_CS 10
#define TFT_DC 9
// setup needed objects
Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);
NunchukController nunchukController;
BulletList bulletList;
Player player(120, 280, &LCD, &nunchukController, &bulletList);

/**
 * @brief Sets up the screen and the player than connects to the Nunchuk
 */
void setup()
{
  sei();
  LCD.begin();
  LCD.fillScreen(ILI9341_BLACK);
  LCD.setRotation(2);
  player.drawPlayer();
  nunchukController.initialize();
}

int main(void)
{
  setup();
  while (1)
  {
    player.controlPlayer();
    bulletList.updateBullets();
  }
  return 0;
}
