#include <avr/io.h>
#include "IR.h"
#include <HardwareSerial.h>

const uint8_t BitLength = 1;    // Bitlength is the amount of blocks that represents a bit
const uint8_t LeaderLength = 5; // LeaderLength is the amount of blocks the leader is transmitted (should always be a multiple of 3 - 1 2/3 are high followed by 1/3 low)
uint8_t irblockcount = 0;
uint8_t irsending = 0;
uint8_t irbitsendcount = 0;

IR::IR()
{
}

void IR::IR_innit()
{ // initializes the infraredcommunication
    DDRD |= (1 << DDD6);
    PORTD |= (1 << PORTD6);
    Timer0_innit();
    timerStop();
}

void IR::Timer0_innit()
{ // initializes timer0 for ir communication
    /* timer0 statistics

        COM0A[1:0] = 0b01 (OC0A discontected) COM0B[1:0] = 0b00 (OC0B discontected)
        WGM0[2:0] = 0b010 (Mode 2: CTC)
        CS0[2:0] = 0b001 (no prescaler)

        (OCR0A + 1) * N / F_CPU = 1/36000 s
        OCR0A = 1/38000 * (16000000) - 1 = 421
        421/2 gives us 210.5 which can't fit in OCR0A, rounding up to 211 gives us a result close to 38kHz which will still work for communication.
        OCR0A on 211 gives around 37732 hz
    */

    TCCR0A |= (1 << WGM01) | (1 << COM0A0);
    TCCR0B |= (1 << CS00);
    OCR0A = 211;
    TCNT0 = 0;
    TIMSK0 |= (1 << OCIE0A);
}

void IR::timerStart()
{
    TCCR0A |= (1 << COM0A0);
    TIMSK0 |= (1 << OCIE0A);
    TCNT0 = 0;
}

void IR::timerStop()
{
    TIMSK0 &= ~(1 << OCIE0A);
    TCCR0A &= ~(1 << COM0A0);
}

void IR::StartComm()
{
    bool done = false;
    timerStart();
    if (done)
    {
        timerStop();
    }
}

void IR::resetblockcount(){
    irblockcount = 0;
}

void IR::UpdateBlockcount()
{
    irblockcount++;
}

void IR::SendLeader(){
if (irsending == 0){

    if (irblockcount < (2 * ((LeaderLength + 1) / 3)))
    {
        TCCR0A |= (1 << COM0A0);
    }
    else if (irblockcount >= (2 * ((LeaderLength + 1) / 3)) && irblockcount <= LeaderLength)
    {
        TCCR0A &= ~(1 << COM0A0);
    }
    else
    {
        irblockcount = 0;
        irsending++;
    }
}}

void IR::SendStartbit()
{
    if (irsending == 1){
    if (irblockcount == 0)
    {
        TCCR0A |= (1 << COM0A0);
    }
    else if (irblockcount == 1)
    {
        TCCR0A &= ~(1 << COM0A0);
    }
    else
    {
        irblockcount = 0;
        irsending++;
    }
    }
}

void IR::SendDatabit(uint8_t datatosend, uint8_t datalength)
{
    if (irsending == 2){
    if (irblockcount == 0)
    {

        if ((datatosend & (1 << irbitsendcount)) == 0)
        {
            TCCR0A &= ~(1 << COM0A0);
        }
        else
        {
            TCCR0A |= (1 << COM0A0);
        }
    }
    else if (irblockcount == 1)
    {
        if ((datatosend & (1 << irbitsendcount)) == 0)
        {
            TCCR0A |= (1 << COM0A0);
        }
        else
        {
            TCCR0A &= ~(1 << COM0A0);
        }
    }
    else
    {
        irblockcount = 0;
        irbitsendcount++;
        if (irbitsendcount >= datalength)
        {
            irbitsendcount = 0;
            irsending++;
        }
    }}
}

void IR::SendParitybit(bool parityeven){
    if (irsending == 3){
      if (irblockcount < 2)
  {
    if (parityeven)
    {
      TCCR0A |= (1 << COM0A0);
    }
    else
    {
      TCCR0A &= ~(1 << COM0A0);
    }
  }
  else if (irblockcount < 4)
  {
    if (parityeven)
    {
      TCCR0A &= ~(1 << COM0A0);
    }
    else
    {
      TCCR0A |= (1 << COM0A0);
    }
  } else {
    irblockcount = 0;
    irsending = 0;
  }}
}
