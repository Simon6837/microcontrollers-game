#ifndef IR_H
#define IR_H

class IR
{
public:
IR();
void IR_innit();
void StartComm();

private:
void Timer0_innit();
void timerStart();
void timerStop();

};



#endif