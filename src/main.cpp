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
#include "classes/NunchukController.h"
#include "classes/Score.h"
// pins for the screen
#define TFT_CS 10 // Chip select line for TFT display
#define TFT_DC 9  // Data/command line for TFTú

// setup devices
Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);
NunchukController nunchukController;
// varibles needed for the game
bool playerIsMoving = false;
// current gameStates: 0 (menu), 1 (solo), 2 (game-over)
volatile uint8_t gameState = 0;
volatile int8_t menuState = 0;
uint8_t shouldDrawEnemy = 4;
uint8_t drawEnemyIndex = 0;
// enemies array, initialized here to prevent stack overflow
Enemy enemies[4][5] = {
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0)},
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0)},
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0)},
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0)}};
// all required objects
Score score(&LCD);
BulletList bulletList(&playerIsMoving, enemies, &score);
Player player(120, 280, 3, &LCD, &nunchukController, &bulletList, &playerIsMoving);
IR ir_comm;
// varibles needed for the timers
uint8_t counteronesec = 0;
uint8_t timemovement = 0;
volatile bool redrawEnemy = true;
volatile uint8_t trespassCheck;
// how many times the enemies move before they go down
const uint8_t maxTimeMovement = 8;
uint8_t currentLevel = 1;

/**
 *  timer1 statistics
 *  COM1x[1:0] 0b00 (pins disconected)
 *  WGM0[3:0] = 0b0101 (mode 5: fast pwm 8-bit)
 *  CS0[2:0] = 0b001 (no prescaler)
 *  OCRA is used for duty cycle (PORTD5 off after COMPA_VECT, on at Overflow)
 */
void initTimer1(void)
{

  TCCR1B |= (1 << WGM12) | (1 << CS12);
  TCNT1 = 0; // reset timer
  OCR1A = 2082;
  TIMSK1 |= (1 << OCIE1A);
}

/**
 *  timer2 statistics
 *  COM2x[1:0] 0b00 (pins disconected)
 *  WGM2[2:0] = 0b010 (mode 2: fast pwm 8-bit)
 *  CS2[2:0] = 0b001 (no prescaler)
 *  OCRA is used for duty cycle (PORTD5 off after COMPA_VECT, on at Overflow)
 */
void initTimer2(void)
{

  TCCR2A |= (1 << WGM20) | (1 << WGM21);
  TCCR2B |= (1 << CS20);
  TCNT2 = 0;                              // reset timer
  TIMSK2 |= (1 << OCIE2A) | (1 << TOIE2); // enable interrupt on compare match A and on overflow
}

void initADC(void)
{
  ADMUX |= (1 << REFS0); // reference voltage on AVCC
  ADMUX &= ~(1 << MUX0);
  ADCSRB &= ~(1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0); // freerunning
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // ADC clock prescaler /128
  ADCSRA |= (1 << ADATE);                                // enable ADC auto trigger
  ADCSRA |= (1 << ADEN);                                 // enable ADC
  ADCSRA |= (1 << ADIE);                                 // enable interrupt
  ADCSRA |= (1 << ADSC);                                 // first time start ADC conversion
  ADCSRB |= (1 << ACME);                                 // enable multiplexing'
  ADMUX |= (1 << ADLAR);                                 // ADC Left adjust
}

void initPotpins()
{
  DDRD |= (1 << DDD5) | (1 << DDD4) | (1 << DDD3);
  PORTD |= (1 << PORTD5);
}

ISR(ADC_vect)
{
  if (ADCH <= 10)
  { // limit the brightness to prevent flickering on the screen
    return;
  }
  OCR2A = ADCH; // set OCR1A for dutycycle
}

ISR(TIMER2_COMPA_vect)
{
  PORTD &= ~(1 << PORTD5);
}

ISR(TIMER2_OVF_vect)
{
  PORTD |= (1 << PORTD5);
}

ISR(TIMER1_COMPA_vect)
{
  bulletList.updateBullets();
  counteronesec++;
  //
  if (counteronesec == 44)
  {
    if (timemovement == (maxTimeMovement - 1))
    {
      trespassCheck = 1;
    }
  }
  if (counteronesec == 45) // TODO: remove magic number (could be made dynamic to increase difficulty)
  {
    Enemy::moveEnemy(enemies, timemovement, maxTimeMovement);
    timemovement++;
    if (timemovement == maxTimeMovement)
    {
      timemovement = 0;
    }
    redrawEnemy = true;
    counteronesec = 0;
  }
}

void showMenu()
{
  gameState = 0;
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
}

void flickerButton()
{
  LCD.drawRect(64, 220, 112, 40, ILI9341_RED);
  LCD.drawRect(64, 220, 112, 40, ILI9341_WHITE);
  LCD.drawRect(64, 220, 112, 40, ILI9341_RED);
  LCD.drawRect(64, 220, 112, 40, ILI9341_WHITE);
  LCD.drawRect(64, 220, 112, 40, ILI9341_RED);
}

void resetEnemies()
{
  // reset all enemies to their default state
  for (uint8_t j = 0; j < 4; j++)
  {
    for (uint8_t i = 0; i < 5; i++)
    {
      enemies[j][i].setType(0);
      enemies[j][i].setYOffset(0);
      enemies[j][i].setXOffset(0);
    }
  }
  // reset the time movement to force the enemies to spawn when the game starts
  timemovement = maxTimeMovement - 1;
}

/**
 * @TODO: Reset enemy position upon re-start
 */
void startGame()
{
  resetEnemies();
  LCD.fillScreen(ILI9341_BLACK);
  LCD.fillScreen(ILI9341_BLACK);
  gameState = 1;
  TIMSK1 |= (1 << OCIE1A);
  player.x = 120;
  player.y = 280;
  player.lives = 1;
  player.displayLives();
  player.drawPlayer();
}

void menuControlsEnable()
{
  if (menuState < 0)
  {
    menuState = 0;
  }
  else if (menuState > 47)
  {
    menuState = 47;
  }
  if (nunchukController.updateMenu() == 0)
  {
    menuState++;
  }
  if (nunchukController.updateMenu() == 1)
  {
    menuState--;
  }
  if (menuState < 16)
  {
    LCD.drawRect(64, 100, 112, 40, ILI9341_YELLOW);
    LCD.drawRect(64, 160, 112, 40, ILI9341_WHITE);
    LCD.drawRect(64, 220, 112, 40, ILI9341_WHITE);
    if (nunchukController.isZButtonPressed() == true)
    {
      startGame();
    }
  }
  if (menuState > 15 && menuState < 32)
  {
    LCD.drawRect(64, 100, 112, 40, ILI9341_WHITE);
    LCD.drawRect(64, 160, 112, 40, ILI9341_GREEN);
    LCD.drawRect(64, 220, 112, 40, ILI9341_WHITE);
  }
  if (menuState > 31)
  {
    LCD.drawRect(64, 100, 112, 40, ILI9341_WHITE);
    LCD.drawRect(64, 160, 112, 40, ILI9341_WHITE);
    LCD.drawRect(64, 220, 112, 40, ILI9341_YELLOW);
    if (nunchukController.isZButtonPressed() == true)
    {
      flickerButton();
    }
  }
}

/**
 *@brief clears screen to show a game over message
 */
void gameOver()
{
  gameState = 2;
  TIMSK1 &= ~(1 << OCIE1A);
  LCD.fillScreen(ILI9341_WHITE);
  LCD.fillScreen(ILI9341_BLACK);
  LCD.drawChar(40, 40, 'G', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
  LCD.drawChar(80, 40, 'A', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
  LCD.drawChar(120, 40, 'M', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
  LCD.drawChar(160, 40, 'E', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
  LCD.drawChar(40, 90, 'O', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
  LCD.drawChar(80, 90, 'V', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
  LCD.drawChar(120, 90, 'E', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
  LCD.drawChar(160, 90, 'R', ILI9341_WHITE, ILI9341_WHITE, 4, 4);
}

/**
  *@brief checks if an enemy is still alive at the lowest row before the next shift takes place
  TODO: relocate gameOver check
*/
void checkEnemyTrespass()
{
  for (uint8_t i = 0; i < 5; i++)
  {
    if (trespassCheck == 1)
    {
      if (enemies[3][i].getType() != 0)
      {
        player.lives--;
        player.displayLives();
        trespassCheck = 0;
      }
      else
      {
      }
    }
  }
  trespassCheck = 0;
  if (player.lives == 0)
  {
    gameOver();
  }
}

void dismissGameOver()
{
  if (nunchukController.isZButtonPressed() == true)
  {
    showMenu();
  }
}

/**
 * @brief Sets up the screen and the player than connects to the Nunchuk
 * Also initializes the timers and the ADC.
 */
void setup()
{
  sei();
  initTimer2();
  initTimer1();
  initADC();
  initPotpins();
  Wire.begin();
  Serial.begin(9600);
  LCD.begin();
  LCD.fillScreen(ILI9341_BLACK);
  LCD.setRotation(2);
  showMenu();
  score.displayScore();
  nunchukController.initialize();
  ir_comm.IR_innit();
  player.displayLives();
}

/**
 * @brief Interrupt service routine for the timer
 * @note Updates the position of the bullets(to prevent delays on the screen)
 * @note Toggles the LED for the IR communication
 */
ISR(TIMER0_COMPA_vect)
{
  PORTD ^= (1 << PORTD6);
}

/**
 * @brief Main loop
 * @note Controls the player
 */
int main(void)
{
  setup();
  while (1)
  {
    if (gameState == 0)
    {
      menuControlsEnable();
    }
    if (gameState == 1)
    {
      player.controlPlayer();
      //! This is not a good solition to fix the player lagging, but it works for now
      if (shouldDrawEnemy)
      {
        enemies[shouldDrawEnemy - 1][drawEnemyIndex].drawEnemy();
        drawEnemyIndex++;
        if (drawEnemyIndex == 5)
        {
          drawEnemyIndex = 0;
          shouldDrawEnemy--;
        }
      }
      if (trespassCheck == 1)
      {
        checkEnemyTrespass();
      }
    }
    if (gameState == 2)
    {
      dismissGameOver();
    }
  }
  return 0;
}
