#ifndef IR_H
#define IR_H

class IR
{
public:
    IR();
    void IR_innit();
    void StartComm();
    void SendLeader();
    void UpdateBlockcount();
    void SendStartbit();
    void SendDatabit(uint8_t datatosend, uint8_t datalength);
    void SendParitybit(bool parityeven);
    void resetblockcount();

private:
    uint8_t blockcount;
    uint8_t irsending;

    void Timer0_innit();
    void timerStart();
    void timerStop();
};

#endif