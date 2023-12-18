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

// setup devices
Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);
NunchukController nunchukController;
// varibles needed for the game
bool playerIsMoving = false;
// enemies array
Enemy enemies[4][5] = {
    {Enemy(30, 35, &LCD, 1), Enemy(30, 35, &LCD, 1), Enemy(30, 35, &LCD, 1), Enemy(30, 35, &LCD, 1), Enemy(30, 35, &LCD, 1)},
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 8), Enemy(30, 35, &LCD, 0)},
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 8), Enemy(30, 35, &LCD, 0)},
    {Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 0), Enemy(30, 35, &LCD, 8), Enemy(30, 35, &LCD, 0)}};
BulletList bulletList(&playerIsMoving, enemies);
Player player(120, 280, 3, &LCD, &nunchukController, &bulletList, &playerIsMoving);
IR ir_comm;
// varibles needed for the timers
uint8_t counteronesec = 0;
uint8_t timemovement = 0;
// how many times the enemies move before they go down
const uint8_t maxTimeMovement = 8;
// variables used for ir
const uint8_t T = 50;           // T is the amount of pulses we want per block
const uint8_t Block = 2 * T;    // Block is the amount of times we need an interupt
const uint8_t LeaderLength = 6; // LeaderLength is the amount of blocks the leader is transmitted
const uint8_t BitLength = 2;    // Bitlength is the amount of blocks that represents a bit
const uint8_t ParityLength = 4; // Paritylength is the amount of blocks that represent the parity bit
const uint8_t pvpbitlength = 2; // pvpbitlegth is the amount of bits that need to be send during a communication in mode pvp
const uint8_t bit1 = 0b00000001;      // bitmask for bit 0
const uint8_t bit2 = 0b00000010;      // bitmask for bit 1
uint8_t t = 0;                  // t is the amount of interrupts that have passed
uint8_t blockcount = 0;         // blockcount is the amount of blocks that have been send
uint8_t sending = 0;            // what is being send (0 = leader, 1 = startbit, 2 = databits, 3 = paritybit)
uint8_t bitsendcount = 0;       // which bit of the data is being send
uint8_t data = 0b00000011;      // data is the data that needs to be send over
bool parityeven = false;        // used for paritybit

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
  sei();
  if (data)
  {
    ir_comm.StartComm();
  }
  bulletList.updateBullets();
  counteronesec++;
  if (counteronesec == 45) // TODO: remove magic number (could be made dynamic to increase difficulty)
  {
    Enemy::moveEnemy(enemies, timemovement, maxTimeMovement);
    timemovement++;
    if (timemovement == maxTimeMovement)
    {
      timemovement = 0;
    }
    counteronesec = 0;
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
  //! This seems unnessesary
  // for (uint8_t j = 0; j < 4; j++) // voor rijen links en rechts j/2 als rest 1 = links als rest = 0 rechts
  // {
  //   for (uint8_t i = 0; i < 5; i++)
  //   {
  //     enemies[j][i].drawEnemy((i * 40), (j * 50) + (1 * timemovement)); // voor rijen links en rechts j/2 als rest 1 = tijdsverplaatsing + als rest = 0 tijdsverplaatsing -, als max reset tijdsverplaatsing
  //   }
  // }
  player.drawPlayer();
  nunchukController.initialize();
  ir_comm.IR_innit();
  player.displayLives();
}

ISR(TIMER0_COMPA_vect)
{
  t++;
  if (t == Block)
  {
    t = 0;
    blockcount++;
  }
  if (sending == 0 && blockcount < (2 * (LeaderLength / 3)))
  {
    TCCR0A |= (1 << COM0A0);
  }
  else if (sending == 0 && blockcount >= (2 * (LeaderLength / 3)) && blockcount < LeaderLength)
  {
    TCCR0A &= ~(1 << COM0A0);
  }
  else if (sending == 0 && blockcount > LeaderLength)
  {
    blockcount = 0;
    sending++;
  }
  else if (sending == 1 && blockcount == 0)
  {
    TCCR0A |= (1 << COM0A0);
  }
  else if (sending == 1 && blockcount == 1)
  {
    TCCR0A &= ~(1 << COM0A0);
  }
  else if (sending == 1 && blockcount > BitLength)
  {
    sending++;
    blockcount = 0;
  }
  else if (sending == 2 && blockcount == 0)
  {
    if (data & (1 << bitsendcount))
    {
      TCCR0A |= (1 << COM0A0);
    }
    else
    {
      TCCR0A &= ~(1 << COM0A0);
    }
  }
  else if (sending == 2 && blockcount == 1)
  {
    if (data & (1 << bitsendcount))
    {
      TCCR0A &= ~(1 << COM0A0);
    }
    else
    {
      TCCR0A |= (1 << COM0A0);
    }
  }
    else if (sending == 2 && blockcount > BitLength && bitsendcount < pvpbitlength)
  {
    bitsendcount++;
    blockcount = 0;
  } else if (bitsendcount >= pvpbitlength){
    sending++;
    blockcount = 0;
  }
  else if (sending == 3 && blockcount < 2){
    if (parityeven){
      TCCR0A |= (1 << COM0A0);
    } else {
      TCCR0A &= ~(1 << COM0A0);
    }
  }
    else if (sending == 3 && blockcount < 4){
    if (parityeven){
      TCCR0A &= ~(1 << COM0A0);
    } else {
      TCCR0A |= (1 << COM0A0);
    }
  } else if (sending == 3 && blockcount >= ParityLength){
    sending = 0;
    blockcount = 0;
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
    player.controlPlayer();
if (data & bit1 && data & bit2) {
    parityeven = true;  // Even parity
} else {
    parityeven = false; // Odd parity
}

  }
  return 0;
}
