#include "timerfile.h"

#include <avr/io.h>
#include <stdint.h>
#include "cutils.h"
#include "serial.h"
#include <stddef.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

uint32_t count0;

int Timer0Innit(){
    
    sei();
    SETMASK((1 << CS10) | (1 << WGM12), TCCR1B); //Set CS12:0 to 1 so that Timer 1's clock is ClkIO, or 1Mhz
    CLEARMASK((1 << WGM10) | (1 << WGM11), TCCR1A); //Set WGM13:0 to 4 for Continuos Measurement Mode CTC
    CLEARMASK((1 << WGM13), TCCR1B); 
    SETMASK((1 << OCIE1A), TIMSK1); //Set Interrupt enable of Timer 1's output compare register
    OCR1AH = 0x01; 
    OCR1AH = 0xF3; //Set the top value of the Output Compare Register A for Timer 0 to be 499, or 0x01F3
    count0 = 0x00000000; //reset Timer0's counter
    return 1;

}

int GetTimer0(){
    uint32_t current_time;
    CLEARMASK((1 << OCIE1A), TIMSK1);
    current_time = count0;
    SETMASK((1 << OCIE1A), TIMSK1);
    return current_time;

}

ISR(TIMER1_COMPA_vect){
    count0++;
}