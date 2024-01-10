#ifndef SCORE_H
#define SCORE_H

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

class Score
{
private:
    uint16_t score;
    Adafruit_ILI9341 *LCD;
public:
    Score(Adafruit_ILI9341 *lcd);
    uint16_t getHighscore();
    void increaseScore();
    void setHighscore();
    void setHighscoreFromShare(uint16_t newScore);
    void displayScore();
    void resetScore();
    void resetHighScore();
};

#endif // SCORE_H
