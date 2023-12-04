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
#define backlight_pin 5
#define test_pin 1
//setup needed objects
Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);
NunchukController nunchukController;
Player player(120, 280, &LCD, &nunchukController);
volatile uint8_t ADCval;

void initTimer2(void)
{
  TCCR2A = 0; //clear timer
  TCCR2A |=(1<<WGM20)|(1<<WGM21); // Fast PWM mode
  TCCR2A |= (1<<COM2A1); // PWM mode, non-inverting
  //TCCR2A |= (1<<COM2B1);
  //TCCR2B |= (1<<CS21);
  TCCR2B |= (1<<CS22); // prescale /256
  //TIMSK2 |= (1<<TOIE2); //interrupt on TOV
}

void initADC(void)
{
  ADMUX |= (1<<REFS0); // reference voltage on AVCC
  ADMUX |= (1<<MUX0);
  ADCSRB &= ~(1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0); // freerunning
  ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //ADC clock prescaler /128
  ADCSRA |= (1<<ADATE); //enable ADC auto trigger 
  ADCSRA |= (1<<ADEN); // enable ADC
  ADCSRA |= (1<<ADIE); // enable interrupt
  ADCSRA |= (1<<ADSC); // first time start ADC conversion
  ADCSRB |= (1<<ACME); // enable multiplexing'
  ADMUX |= (1<<ADLAR); //ADC Left adjust
}

ISR(ADC_vect)
{
  //ADCval = ADC / 4;
  OCR1A = ADCH;
}

ISR(TIMER1_COMPA_vect)
{
  TCCR1A ^= (1<<COM1A1); // toggle from non-inverting to disabled and vice versa
}

/**
 * @brief Sets up the screen and the player than connects to the Nunchuk
 */
void setup()
{
  sei();
  initTimer2();
  initADC();
  Serial.begin(9600);
  LCD.begin();
  LCD.fillScreen(ILI9341_BLACK);
  LCD.setRotation(2);
  player.drawPlayer();
  nunchukController.initialize();
}

void changeBrightness() {
;
}

int main(void)
{
  setup();
  //pinMode(backlight_pin, LOW);
  //PORTD |= (1<<PORTB5);
  //analogWrite(backlight_pin, ADCval);
  DDRB |= (1<<DDB5);
  while (1)
  {
    player.controlPlayer();
    //Serial.println(ADC/4);
    //changeBrightness();
    //Serial.println(ADCval);
    //analogWrite(backlight_pin, ADCval);
    //DDRB |= (1<<DDB5);
  }
  return 0;
}
