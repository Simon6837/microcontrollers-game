// libaries
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Nunchuk.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <HardwareSerial.h>
// classes
#include "classes/Player.h"

// pins for the screen
#define TFT_CS 10
#define TFT_DC 9
// Define constants for the nunchuk
#define NUNCHUK_ADDRESS 0x52
#define BAUDRATE 9600
#define CHUNKSIZE 32
#define BUFFERLEN 256

Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);

Player player(200, 280, &LCD); // Instantiate player object

// Declare all functions
void ControlPlayer(void);
void checkNunchuckConnection(void);
void printNunchuckState(void);

void setup()
{
  // enable global interrupts
  sei();
  // use Serial for printing nunchuk data
  Serial.begin(BAUDRATE);
  // join I2C bus as master
  Wire.begin();
  // connect with the nunchuk
  Serial.print("Connecting to nunchuk at address 0x");
  Serial.println(NUNCHUK_ADDRESS, HEX);
  if (!Nunchuk.begin(NUNCHUK_ADDRESS))
  {
    Serial.println("No nunchuk found");
    Serial.flush();
  }

  LCD.begin();
  LCD.fillScreen(ILI9341_BLACK);
  LCD.setRotation(2);
  // check if the screen is working with some simple text TODO: remove this after testing
  LCD.println("Hello World!");
}

int main(void)
{
  setup();
  // super loop
  while (1)
  {
    ControlPlayer();
  }
  return (0);
}

/**
 * @brief Updates the nunchuck values to the current state of the nunchuck
 */
void ControlPlayer(void)
{
  checkNunchuckConnection();
  printNunchuckState();
  if (Nunchuk.state.joy_x_axis < 115)
  {
    player.moveLeft();
  }
  else if (Nunchuk.state.joy_x_axis > 140)
  {
    player.moveRight();
  }
}

void checkNunchuckConnection(){
if (!Nunchuk.getState(NUNCHUK_ADDRESS))
  {
    Serial.println("******** No nunchuk found");
    Serial.flush();
    while (!Nunchuk.getState(NUNCHUK_ADDRESS))
    {
      // wait for nunchuk to be connected
    }
  }
}

void printNunchuckState(void)
{
  Serial.print("Joy X: ");
  Serial.print(Nunchuk.state.joy_x_axis, DEC);
  Serial.print("\tJoy Y: ");
  Serial.print(Nunchuk.state.joy_y_axis, DEC);
  Serial.print("\t\tButton C: ");
  Serial.print(Nunchuk.state.c_button, DEC);
  Serial.print("\t\t\tButton Z: ");
  Serial.println(Nunchuk.state.z_button, DEC);
}
