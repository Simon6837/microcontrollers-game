#ifndef PLAYER_H
#define PLAYER_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

class Player
{
private:
    int x, y;
    Adafruit_ILI9341 *LCD;

public:
    Player(int initialX, int initialY, Adafruit_ILI9341 *lcd);
    void drawPlayer();
    void clearPlayer();
    void moveLeft();
    void moveRight();
};

#endif
