#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <classes/player.h>

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define TFT_CS 10
#define TFT_DC 9
volatile int c;
volatile int x;
volatile int y;
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void draw_screen() {
  tft.fillScreen(ILI9341_BLACK);
}

void draw_player() {
  tft.fillCircle(x, y, 20, ILI9341_WHITE);
}

void player_start() {
  x = 120;
  y = 160;
  //Player player_ship(120, 160);
}

//Something that manipulates x and y based on nunchuck movement
void player_move() {
  y++;
}

void setup(void)
{
  Serial.begin(9600);
  tft.begin();
  player_start();

  tft.fillScreen(ILI9341_BLACK);
  // origin = left,top landscape (USB left upper)
  tft.setRotation(2);
//tft.println("Hello World!");
}

void loop()
{
  draw_screen();
  draw_player();
  player_move();
}



