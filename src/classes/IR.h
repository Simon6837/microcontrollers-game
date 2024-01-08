#ifndef IR_H
#define IR_H

class IR
{
public:
    IR();
    void IR_innit();
    bool StartComm(uint8_t data);
    void UpdateBlockcount();
    bool commOrder(uint8_t datalength);

private:
    uint8_t blockcount;
    uint8_t irsending;
    uint8_t data;

    void Timer0_innit();
    void timerStart();
    void timerStop();
    bool calculateParity(uint8_t data);
    void SendLeader();
    void SendStartbit();
    void SendDatabit(uint8_t datatosend, uint8_t datalength);
    bool SendParitybit(bool parityeven);
};

#endif