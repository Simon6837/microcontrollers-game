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

const long fivesecondcount = 190000; // one second is 38000 (timer0 runs at 38khz)
// setup needed objects
Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);
NunchukController nunchukController;
BulletList bulletList;
Player player(120, 280, &LCD, &nunchukController, &bulletList);
IR ir_comm;
Enemy allenemies(0, 0, &LCD, 0);
Enemy enemy0(30, 35, &LCD, 0);
Enemy enemy1(30, 35, &LCD, 1);
Enemy enemies[4][5]{{enemy1, enemy1, enemy1, enemy1, enemy1},
                    {enemy0, enemy0, enemy0, enemy0, enemy0},
                    {enemy0, enemy0, enemy0, enemy0, enemy0},
                    {enemy0, enemy0, enemy0, enemy0, enemy0}};

long counteronesec = 0;
uint8_t timemovement = 0;
/**
 * @brief Sets up the screen and the player than connects to the Nunchuk
 */
void setup()
{
  sei();
  LCD.begin();
  LCD.fillScreen(ILI9341_BLACK);
  LCD.setRotation(2);
  for (uint8_t j = 0; j < 4; j++) // voor rijen links en rechts j/2 als rest 1 = links als rest = 0 rechts
  {
    for (uint8_t i = 0; i < 5; i++)
    {
      enemies[j][i].drawEnemy((i * 40), (j * 50) + (1 * timemovement)); // voor rijen links en rechts j/2 als rest 1 = tijdsverplaatsing + als rest = 0 tijdsverplaatsing -, als max reset tijdsverplaatsing
    }
  }
  player.drawPlayer();
  nunchukController.initialize();
  ir_comm.IR_innit();
}

ISR(TIMER0_COMPA_vect)
{
  PORTD ^= (1 << PORTD6);
  counteronesec++;

  if (counteronesec == fivesecondcount)
  {
    allenemies.moveEnemy(enemies, timemovement, enemy0);
    timemovement++;
    if (timemovement == 5)
    {
      timemovement = 0;
    }
    counteronesec = 0;
  }
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
