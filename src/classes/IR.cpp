#include <avr/io.h>
#include "IR.h"
#include <HardwareSerial.h>
#include <avr/delay.h>

const uint8_t BitLength = 1;    // Bitlength is the amount of blocks that represents a bit
const uint8_t LeaderLength = 5; // LeaderLength is the amount of blocks the leader is transmitted (should always be a multiple of 3 - 1 2/3 are high followed by 1/3 low)
const uint8_t MAX_READINGS = 44;
uint8_t irblockcount = 0;
uint8_t irsending = 0;
uint8_t irbitsendcount = 0;
uint8_t readcount = 0;
bool done = false;
uint16_t data = 0;
bool parity = false;
bool pinD2Value = false;
static bool readings[MAX_READINGS];           // Array to store readings
static volatile bool newDataReceived = false; // Flag to indicate when new data is received

enum readingStates
{
    none,
    leader,
    startbit,
    databit,
    paritybit,
    reversing
};
static volatile readingStates readingState = none;

static volatile uint8_t leaderReceiveCheck = 0;
static volatile bool receivedFullBit = false;
static volatile uint8_t previousReceivedBit = 0;
static volatile uint8_t dataCount = 0;
static volatile uint16_t receivedData = 0;

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
    if (done)
    {
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
    if (!done)
    {
        if (irsending == 0)
        {
            SendLeader();
        }
        else if (irsending == 1)
        {
            SendStartbit();
        }
        else if (irsending == 2)
        {
            SendDatabit(data, datalength);
        }
        else if (irsending == 3)
        {
            done = SendParitybit(parity);
            if (done)
            {
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
    bool pinD2Value = !(PIND & (1 << PIND2)); // Invert the value to detect "10"
                                              // Serial.println(newDataReceived);
    if (readingState == none)
    {
        if (pinD2Value == 1)
        {
            readingState = leader;
            leaderReceiveCheck = (pinD2Value << 0);
        }
    }
    else if (readingState == leader)
    {
        Serial.println(leaderReceiveCheck);
        leaderReceiveCheck = (leaderReceiveCheck << 1) | pinD2Value;
        leaderReceiveCheck &= (0b111);
        if (leaderReceiveCheck == (0b100))
        {
            readingState = startbit;
        }
    }
    else if (readingState == startbit)
    {

        if (pinD2Value == 0)
        {
            readingState = databit;
        }
    }
    else if (readingState == databit)
    {
        if (receivedFullBit)
        {
            if (previousReceivedBit == 0 && pinD2Value == 1)
            {
                // Shift the existing bits in receivedData to the left by one position and set the least significant bit (LSB) to 0
                receivedData = (receivedData << 1) | 0b0;
            }
            else if (previousReceivedBit == 1 && pinD2Value == 0)
            {
                // Shift the existing bits in receivedData to the left by one position and set the LSB to 1
                receivedData = (receivedData << 1) | 0b1;
            }
            receivedFullBit = false;
            dataCount++;
        }
        else
        {
            previousReceivedBit = pinD2Value;
            receivedFullBit = true;
        }
        if (dataCount == 16)
        {
            readingState = paritybit;
        }
    }
    else if (readingState == paritybit)
    {
        Serial.println(receivedData);
        // TODO: reset all variables with a function
        readingState = reversing;
        leaderReceiveCheck = 0;
        receivedFullBit = false;
        previousReceivedBit = 0;
        dataCount = 0;
    }
    else if (readingState == reversing)
    {
        uint16_t reversedData = 0;
        uint16_t numBits = sizeof(receivedData) * 8; // Calculate the number of bits in receivedData

        for (uint16_t i = 0; i < numBits; ++i)
        {
            reversedData <<= 1;                    // Left shift the reversedData to make space for the next bit
            reversedData |= (receivedData & 0x01); // OR operation to set the least significant bit of reversedData
            receivedData >>= 1;                    // Right shift receivedData to get the next bit
        }

        receivedData = reversedData; // Assign the reversed data back to receivedData
        Serial.println(receivedData);
        newDataReceived = true;
        readingState = none;
    }
}

void IR::SetNewDataReceived()
{
    if (!newDataReceived)
    {
        newDataReceived = true;
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
