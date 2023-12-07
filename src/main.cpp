// libraries
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <HardwareSerial.h>
#include <avr/io.h>
// classes
#include "classes/Player.h"
#include "classes/NunchukController.h" // Include the new header
// pins for the screen
#define TFT_CS 10
#define TFT_DC 9
//setup needed objects
Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);
NunchukController nunchukController;
Player player(120, 280, &LCD, &nunchukController);

void initTimer1(void)
{
 /* timer1 statistics
        
        COM1x[1:0] 0b00 (pins disconected)
        WGM0[3:0] = 0b0101 (mode 5: fast pwm 8-bit)
        CS0[2:0] = 0b001 (no prescaler)

      OCRA is used for duty cycle (PORTD5 off after COMPA_VECT, on at Overflow)
    */
   TCCR1A |= (1 << WGM10);
   TCCR1B |= (1 << WGM12) | (1 << CS10);
   TCNT1 = 0; //reset timer
   TIMSK1 |= (1 << OCIE1A) | (1 << TOIE1); //eneable interupt on compare match A and on overFlow

}

void initADC(void)
{
  ADMUX |= (1<<REFS0); // reference voltage on AVCC
  ADMUX &= ~(1<<MUX0);
  ADCSRB &= ~(1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0); // freerunning
  ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //ADC clock prescaler /128
  ADCSRA |= (1<<ADATE); //enable ADC auto trigger 
  ADCSRA |= (1<<ADEN); // enable ADC
  ADCSRA |= (1<<ADIE); // enable interrupt
  ADCSRA |= (1<<ADSC); // first time start ADC conversion
  ADCSRB |= (1<<ACME); // enable multiplexing'
  ADMUX |= (1<<ADLAR); //ADC Left adjust
}

void initPotpins()
{
  DDRD |= (1<<DDD5) | (1 << DDD4) | (1 << DDD3);
  PORTD |= (1<<PORTD5);
}

ISR(ADC_vect)
{
  if (ADCH <= 10){ // limit the brightness to prevent flickering on the screen
    return;
  }
  OCR1AL = ADCH;  //set OCR1A for dutycycle
}

ISR(TIMER1_COMPA_vect)
{
  PORTD &= ~(1 << PORTD5);
}

ISR(TIMER1_OVF_vect){
  PORTD |= (1 << PORTD5);
}

/**
 * @brief Sets up the screen and the player than connects to the Nunchuk
 * Also initializes the timers and the ADC.
 */
void setup()
{
  sei();
  initTimer1();
  initADC();
  initPotpins();
  Serial.begin(9600);
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
  }
  return 0;
}
