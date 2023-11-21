// libaries
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
// classes
#include "classes/Player.h"

// pins for the screen
#define TFT_CS 10
#define TFT_DC 9

Adafruit_ILI9341 LCD = Adafruit_ILI9341(TFT_CS, TFT_DC);

Player player(200, 280, &LCD); // Instantiate player object

void setup()
{
  LCD.begin();
  LCD.fillScreen(ILI9341_BLACK);
  LCD.setRotation(2);
  // check if the screen is working with some simple text TODO: remove this after testing
  LCD.println("Hello World!");
}

void loop()
{
  player.moveLeft();
  delay(1000);
}