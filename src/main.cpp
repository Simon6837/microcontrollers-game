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
#include "classes/NunchukController.h" // Include the new header
// pins for the screen
#define TFT_CS 10 // Chip select line for TFT display
#define TFT_DC 9  // Data/command line for TFT

const uint16_t ToUpdate = 1267;
// setup needed objects
Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);
NunchukController nunchukController;
BulletList bulletList;
Player player(120, 280, &LCD, &nunchukController, &bulletList);
IR ir_comm;

volatile uint16_t counterUpdate = 0;
/**
 * @brief Interrupt routine for timer 0
 *  Set Timer2 prescaler to 1024
 * set compare interrupt
 * For 30 interrupts per second (16000000 / (1024 * 30)) - 1
 */
void initTimer2()
{
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
  TIMSK2 |= (1 << OCIE2A);
  OCR2A = 156;
}

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
  ir_comm.IR_innit();
}

ISR(TIMER0_COMPA_vect)
{
  PORTD ^= (1 << PORTD6);
  counterUpdate++;
  if (counterUpdate == ToUpdate)
  {
    bulletList.updateBullets();
    counterUpdate = 0;
  }
}

ISR(TIMER2_COMPA_vect)
{
}

int main(void)
{
  setup();
  while (1)
  {
    player.controlPlayer();
  }
  return 0;
}
