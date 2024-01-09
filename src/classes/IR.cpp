#include <avr/io.h>
#include "IR.h"
#include <HardwareSerial.h>
#include <avr/delay.h>

enum IRState {
    WAITING_FOR_LEADER_START,
    RECEIVING_LEADER,
    WAITING_FOR_START,
    RECEIVING_DATA
};

volatile IRState irReceiverState = WAITING_FOR_LEADER_START;
uint16_t receivedDataBuffer = 0;
uint8_t receivedBitCount = 0;
const uint8_t BitLength = 1;    // Bitlength is the amount of blocks that represents a bit
const uint8_t LeaderLength = 5; // LeaderLength is the amount of blocks the leader is transmitted (should always be a multiple of 3 - 1 2/3 are high followed by 1/3 low)
uint8_t irblockcount = 0;
uint8_t irsending = 0;
uint8_t irbitsendcount = 0;
uint8_t readcount = 0;
bool done = false;
uint16_t data = 0;
bool parity = false;
bool pinD2Value = false;

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
    // TIMSK0 &= ~(1 << OCIE0A);
    TCCR0A &= ~(1 << COM0A0);
}

bool IR::StartComm(uint16_t data)
{
    timerStart();
    if (done){
        irblockcount = 0;
        IR::data = data;
        parity = calculateParity(IR::data);
        done = false;
        return true;
    }
    return false;
}

bool IR::commOrder(uint8_t datalength)
{
    _delay_ms(1);
    if (!done){
    if (irsending == 0) { 
        SendLeader();
    }
    else if (irsending == 1) {
        SendStartbit();
    }
    else if (irsending == 2) {
        SendDatabit(data, datalength);
    }
    else if (irsending == 3){
        done = SendParitybit(parity);
        if (done) {
            data = 0;
            timerStop();
            return true;
        }
    }
    }
    return false;
}

void IR::UpdateBlockcount()
{
    irblockcount++;
}

void IR::UpdateReadcount()
{
    readcount++;
    bool pinD2Value = !(PIND & (1 << PIND2));  // Invert the value to detect "10"

    switch (irReceiverState)
    {
    case WAITING_FOR_LEADER_START:
        if (!pinD2Value)  // Check for "10" pattern indicating the leader pulse start
        {
            irReceiverState = RECEIVING_LEADER;
            receivedBitCount = 0;  // Reset the bit count when transitioning to RECEIVING_LEADER
        }
        break;

    case RECEIVING_LEADER:
        // Implement logic to handle the leader pulse if needed
        // For example, you may want to wait for a certain number of transitions before proceeding to the next state.
        // ...

        irReceiverState = WAITING_FOR_START;
        break;

    case WAITING_FOR_START:
        if (pinD2Value)  // Check for "10" pattern indicating the start bit
        {
            irReceiverState = RECEIVING_DATA;
            receivedDataBuffer = 0;
            receivedBitCount = 0;
        }
        else
        {
            irReceiverState = WAITING_FOR_LEADER_START;  // Go back to waiting for the leader
        }
        break;

    case RECEIVING_DATA:
        if (receivedBitCount % 2 == 0)  // Process every second read (even counts)
        {
            receivedDataBuffer |= (pinD2Value << (receivedBitCount / 2));
        }

        receivedBitCount++;

        // Check if you've received the desired number of bits (adjust as needed)
        if (receivedBitCount >= 16)  // Assuming 8 bits of data (2 reads per bit)
        {
            // Process the received data (e.g., save it to a variable)
            if (receivedDataBuffer == 0xF0FF)
            {
                Serial.println("Received: 0xF0FF");
            }
            irReceiverState = WAITING_FOR_LEADER_START;  // Go back to waiting for the leader
        }
        break;
    }
}





bool IR::calculateParity(uint16_t data)
{
  int count = 0;
  for (int i = 0; i < 16; i++)
  {
    if ((data & (1 << i)) != 0)
    {
      count++;
    }
  }
  return (count % 2 == 0); // Even parity
}

void IR::SendLeader()
{
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
}

void IR::SendStartbit()
{

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

void IR::SendDatabit(uint16_t datatosend, uint8_t datalength)
{
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
    }
}

bool IR::SendParitybit(bool parityeven)
{
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
    }
    else
    {
        irblockcount = 0;
        irsending = 0;
        return true;
    }
    return false;
}
