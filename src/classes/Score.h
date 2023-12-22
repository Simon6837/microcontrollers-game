#ifndef SCORE_H
#define SCORE_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

class Score
{
private:
    uint16_t score;
    Adafruit_ILI9341 *LCD;
    uint16_t getHighscore();
public:
    Score(Adafruit_ILI9341 *lcd);
    void increaseScore();
    void setHighscore();
    void displayScore();
    void resetScore();
};

#endif // SCORE_H
