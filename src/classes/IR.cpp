#include <avr/io.h>
#include "IR.h"

IR::IR(){

}

void IR::IR_innit(){ //initializes the infraredcommunication
    DDRD |= (1 << DDD6);
    PORTD |= (1 << PORTD6);
    Timer0_innit();
}

void IR::Timer0_innit(){ //initializes timer0 for ir communication
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


void IR::Timer0_switchOnOff() {
    TCCR0B ^= (1 << CS00);
    TCNT0 = 0;
}

void Timer2_innit(){
    /* timer2 statistics
        
        COM0A[1:0] = 0b00 (OC0A discontected) COM0B[1:0] = 0b00 (OC0B discontected)
        WGM0[2:0] = 0b010 (Mode 2: CTC)
        CS0[2:0] = 0b001 (no prescaler)

        (OCR0A + 1) * N / F_CPU = 1/36000 s
        OCR0A = 1/38000 * (16000000) - 1 = 421
        421/2 gives us 210.5 which can't fit in OCR0A, rounding up to 211 gives us a result close to 38kHz which will still work for communication.
        OCR0A on 211 gives around 37732 hz
    */
}


