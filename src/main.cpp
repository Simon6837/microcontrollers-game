// libraries
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <HardwareSerial.h>
// classes
#include "classes/IR.h"
#include "classes/Player.h"
#include "classes/BulletList.h"
#include "classes/Enemy.h"
#include "classes/NunchukController.h" // Include the new header
// pins for the screen
#define TFT_CS 10 // Chip select line for TFT display
#define TFT_DC 9  // Data/command line for TFT
// setup needed objects
Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);
NunchukController nunchukController;
BulletList bulletList;
Player player(120, 280, &LCD, &nunchukController, &bulletList);
Enemy enemy(30, 35, &LCD, 1);
Enemy enemies[4][5]{{enemy, enemy, enemy, enemy, enemy}, {enemy, enemy, enemy, enemy, enemy}, {enemy, enemy, enemy, enemy, enemy}, {enemy, enemy, enemy, enemy, enemy}};
IR ir_comm;

/**
 * @brief Sets up the screen and the player than connects to the Nunchuk
 */
void setup()
{
  sei();
  LCD.begin();
  LCD.fillScreen(ILI9341_BLACK);
  LCD.setRotation(2);
  enemies[2][3].killEnemy();
  for (uint8_t j = 0; j < 4; j++) //voor rijen links en rechts j/2 als rest 1 = links als rest = 0 rechts
  {
    for (uint8_t i = 0; i < 5; i++)
    {
      enemies[j][i].drawEnemy((i * 40), j * 50); //voor rijen links en rechts j/2 als rest 1 = tijdsverplaatsing + als rest = 0 tijdsverplaatsing -, als max reset tijdsverplaatsing
    }
  }
  player.drawPlayer();
  nunchukController.initialize();
  ir_comm.IR_innit();
}

ISR(TIMER0_COMPA_vect)
{
  PORTD ^= (1 << PORTD6);
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
