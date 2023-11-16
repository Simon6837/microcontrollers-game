#include <avr/io.h>
#include <avr/interrupt.h>

void IR_innit(){ //initializes the infraredcommunication
// Timer0_innit();
// Timer2_innit();
}

void Timer0_innit(){ //initializes timer0 for ir communication
    /* timer0 statistics
        
        COM0A[1:0] = 0b01 (toggle OC0A) COM0B[1:0] = 0b00 (OC0B) discontected
        WGM0[2:0] = 0b010 (Mode 2: CTC)
        CS0[2:0] = 0b001 (1/8 prescaler)

        (OCR0A + 1) * N / F_CPU = 1/36000 s
        OCR0A = 1/36000 * (16000000/8) - 1 = 54.5555556 
        OCR0A on 55 gives around 35714 hz
    */

   TCCR0A |= (1 << WGM01);
   TCCR0B |= (1 << CS01);
   OCR0A = 52;
   TCNT0 = 0;
   TIMSK0 |= (1 << OCIE0A);


}
ISR (TIMER0_COMPA_vect){
    PORTD ^= (1 << PORTD6);
}

int main(){
    // Timer0_innit();
    DDRD |= (1 << DDD6);
    DDRD |= (1 << DDD2);
    PORTD |= (1 << PORTD6);
    sei();
    while (1)
    {
    }

    return 0;
    
}


