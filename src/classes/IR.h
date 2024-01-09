#ifndef IR_H
#define IR_H

class IR
{
public:
    IR();
    void IR_innit();
    bool StartComm(uint16_t data);
    void UpdateBlockcount();
    bool commOrder(uint8_t datalength);
    void UpdateReadcount();
    void SetNewDataReceived();
    void timerfullStop();
    
private:
    uint8_t blockcount;
    uint8_t irsending;
    uint16_t data;

    void Timer0_innit();
    void timerStart();
    void timerStop();
    bool calculateParity(uint16_t data);
    void SendLeader();
    void SendStartbit();
    void SendDatabit(uint16_t datatosend, uint8_t datalength);
    bool SendParitybit(bool parityeven);
};

#endif