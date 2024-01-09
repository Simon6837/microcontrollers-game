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
#include "MenuFunctions.h"
// pins for the screen
#define TFT_CS 10 // Chip select line for TFT display
#define TFT_DC 9  // Data/command line for TFTú
// setup devices
Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);
const int brightnessBottomLimit = 10;
NunchukController nunchukController;
// varibles needed for the drawFunctions
bool playerIsMoving = false;
const uint8_t playerInitialXPosition = 120;
const uint16_t playerInitialYPosition = 280;
uint8_t shouldDrawEnemy = 4;
uint8_t drawEnemyIndex = 0;
// menu related varibles
enum gameStates
{
  MENU,
  SOLO,
  GAMEOVER
};
volatile gameStates gameState = MENU;
volatile int8_t menuState = 0;
bool rotationState = false;
bool allowGameToStart = false;
// enemies array, initialized here to prevent stack overflow
const uint8_t maxEnemyRows = 4;
const uint8_t maxEnemyColumns = 5;
Enemy enemies[maxEnemyRows][maxEnemyColumns] = {
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0)},
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0)},
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0)},
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0)}};
// all required objects
Score score(&LCD);
BulletList bulletList(&playerIsMoving, enemies, &score);
Player player(120, 280, 3, &LCD, &nunchukController, &bulletList, &playerIsMoving);
IR ir_comm;
bool sharestate = false;
// varibles needed for the timers
uint8_t counteronesec = 0;
uint8_t timemovement = 0;
volatile bool redrawEnemy = true;
volatile bool trespassCheck;
//  how many times the enemies move before they go down
const uint8_t defaultMaxTimeMovement = 8;
const uint8_t defaultCurrentLevel = 1;
const uint8_t trespassCheckCounterThreshhold = 43;
const uint8_t enemyMoveCounterThreshhold = 45;
// varibles related to level management
uint8_t maxTimeMovement = 8;
uint8_t currentLevel = 1;
uint8_t downMovementCount = 0;
uint8_t downMovementCountTreashhold = 5;
// variables used for ir
const uint8_t T = 50;                // T is the amount of pulses we want per block
const uint8_t Block = 2 * T;         // Block is the amount of times we need an interupt
const uint8_t LeaderLength = 5;      // LeaderLength is the amount of blocks the leader is transmitted (should always be a multiple of 3 - 1 2/3 are high followed by 1/3 low)
const uint8_t BitLength = 1;         // Bitlength is the amount of blocks that represents a bit
const uint8_t ParityLength = 3;      // Paritylength is the amount of blocks that represent the parity bit
const uint8_t pvpdatalength = 16;    // pvpbitlegth is the amount of bits that need to be send during a communication in mode pvp
const uint8_t bit1 = 0b01;           // bitmask for bit 0
const uint8_t bit2 = 0b10;           // bitmask for bit 1
volatile uint8_t t = 0;              // t is the amount of interrupts that have passed
uint8_t blockcount = 0;              // blockcount is the amount of blocks that have been send
uint8_t sending = 0;                 // what is being send (0 = leader, 1 = startbit, 2 = databits, 3 = paritybit)
volatile uint8_t bitsendcount = 0;   // which bit of the data is being send
volatile uint16_t senddata = 0xFFFF; // data is the data that needs to be send over
uint8_t datatosend = 0b00000000;     // data that is being send
volatile bool parityeven = false;    // used for paritybit
volatile bool sendingdata = true;
uint8_t u = 0;
bool countset = true;
uint8_t readinterrupt = 0;
volatile bool int0FallingEdge = false;
volatile uint8_t maxSoloLives = 1;

bool toggled = true;
bool gamerunning = false;

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

/**
 * @brief Initializes the pins for the potentiometer that controls the brightness of the screen
 */
void initPotpins()
{
  DDRD |= (1 << DDD5) | (1 << DDD4) | (1 << DDD3);
  PORTD |= (1 << PORTD5);
}

ISR(ADC_vect)
{
  if (ADCH <= brightnessBottomLimit)
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
  sei();
  counteronesec++;
  // ir related
  if (counteronesec == 5)
  {
    toggled = true;
  }
  if (sharestate)
  {
    if (counteronesec == 30)
    {
      senddata = score.getHighscore();
      counteronesec = 0;
    }
    // Serial.println(senddata);
    if (senddata)
    {
      if (ir_comm.StartComm(senddata))
      {
        senddata = 0;
      }
    }
  }
  // gameRelated
  if (gamerunning)
  {
    bulletList.updateBullets();
    if (counteronesec == trespassCheckCounterThreshhold)
    {
      if (timemovement == (maxTimeMovement - 1))
      {
        trespassCheck = true;
      }
    }
    if (counteronesec == enemyMoveCounterThreshhold) // TODO: remove magic number (could be made dynamic to increase difficulty)
    {

      Enemy::moveEnemy(enemies, timemovement, maxTimeMovement);
      timemovement++;
      if (timemovement == maxTimeMovement)
      {
        if (maxTimeMovement != 1)
        {
          if (downMovementCount == downMovementCountTreashhold)
          {
            maxTimeMovement--;
            downMovementCount = 0;
            currentLevel++;
          }
        }
        downMovementCount++;
        timemovement = 0;
      }
      redrawEnemy = true;
      counteronesec = 0;
    }
  }
}

void resetEnemies()
{
  // reset all enemies to their default state
  for (uint8_t j = 0; j < maxEnemyRows; j++)
  {
    for (uint8_t i = 0; i < maxEnemyColumns; i++)
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
  maxTimeMovement = defaultMaxTimeMovement;
  currentLevel = defaultCurrentLevel;
  downMovementCount = 0;
  LCD.fillScreen(ILI9341_BLACK);
  LCD.fillScreen(ILI9341_BLACK);
  gameState = SOLO;
  // TIMSK1 |= (1 << OCIE1A);
  player.x = playerInitialXPosition;
  player.y = playerInitialYPosition;
  // TODO: make the lives dynamic based on gamemode
  player.lives = maxSoloLives;
  player.displayLives();
  player.drawPlayer();
  score.resetScore();
  allowGameToStart = false;
  gamerunning = true;
}

/**
  *@brief checks if an enemy is still alive at the lowest row before the next shift takes place
  TODO: relocate gameOver check
*/
void checkEnemyTrespass()
{
  for (uint8_t i = 0; i < maxEnemyColumns; i++)
  {
    if (trespassCheck == true)
    {
      if (enemies[maxEnemyRows - 1][i].getType() != 0)
      {
        player.lives--;
        player.displayLives();
        trespassCheck = false;
      }
    }
  }
  trespassCheck = false;
  if (player.lives == 0)
  {
    gameOver(LCD);
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
  // need to be done when sending score
  EICRA |= (1 << ISC01); // Set falling edge trigger
  EIMSK |= (1 << INT0);  // Enable INT0 interrupt
  //! This seems unnessesary
  // for (uint8_t j = 0; j < 4; j++) // voor rijen links en rechts j/2 als rest 1 = links als rest = 0 rechts
  // {
  //   for (uint8_t i = 0; i < 5; i++)
  //   {
  //     enemies[j][i].drawEnemy((i * 40), (j * 50) + (1 * timemovement)); // voor rijen links en rechts j/2 als rest 1 = tijdsverplaatsing + als rest = 0 tijdsverplaatsing -, als max reset tijdsverplaatsing
  //   }
  // }
  // player.drawPlayer();
  nunchukController.initialize();
  ir_comm.IR_innit();
  player.displayLives();
  showMenu(LCD);
}

ISR(TIMER0_COMPA_vect)
{
  if (!sharestate)
  {
    ir_comm.timerfullStop();
    countset = true;
  }
  t++;
  u++;
  readinterrupt++;
  if (u == 10)
  {
    ir_comm.commOrder(pvpdatalength);
    u = 0;
  }
  if (t == Block)
  {
    t = 0;
    ir_comm.UpdateBlockcount();
  }
  if (readinterrupt == Block)
  {
    readinterrupt = 0;
    ir_comm.UpdateReadcount();
  }
  if (int0FallingEdge && readinterrupt == T && countset)
  {
    readinterrupt = 0;
    countset = false;
  }
}

ISR(INT0_vect)
{
  // Handle INT0 falling edge detection
  int0FallingEdge = true;
  ir_comm.SetNewDataReceived();
  // Serial.println("ISR executed");
}

/**
 * @brief This function will draw one enemie each loop, this is done to prevent the player from lagging when all enemies are drawn at once
 * @note //! This is not a good solition to fix the player lagging, but it works for now
 */
void drawEnemies()
{
  if (shouldDrawEnemy)
  {
    enemies[shouldDrawEnemy - 1][drawEnemyIndex].drawEnemy();
    drawEnemyIndex++;
    if (drawEnemyIndex == maxEnemyColumns)
    {
      drawEnemyIndex = 0;
      shouldDrawEnemy--;
    }
  }
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
    // TODO: turn this into a switch case and move the SOLO code to a function
    if (allowGameToStart == true)
    {
      startGame();
    }
    if (gameState == MENU)
    {
      menuControlsEnable(nunchukController, LCD, score);
    }
    if (gameState == SOLO)
    {
      player.controlPlayer();
      drawEnemies();
      if (trespassCheck == true)
      {
        checkEnemyTrespass();
      }
    }
    if (gameState == GAMEOVER)
    {
      dismissGameOver(nunchukController, LCD);
    }
  }
  return 0;
}
