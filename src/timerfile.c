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

uint32_t count3;

int Timer3Innit(){
    
    sei();
    SETMASK((1 << CS30) | (1 << WGM32), TCCR3B); //Set CS12:0 to 1 so that Timer 1's clock is ClkIO, or 1Mhz
    CLEARMASK((1 << WGM30) | (1 << WGM31), TCCR3A); //Set WGM13:0 to 4 for Continuos Measurement Mode CTC
    CLEARMASK((1 << WGM33), TCCR3B); 
    SETMASK((1 << OCIE3A), TIMSK3); //Set Interrupt enable of Timer 1's output compare register
    OCR3AH = 0x01; 
    OCR3AL = 0xF3; //Set the top value of the Output Compare Register A for Timer 0 to be 499, or 0x01F3
    count3 = 0x00000000; //reset Timer0's counter
    return 1;

}

int GetTimer3(){
    uint32_t current_time;
    CLEARMASK((1 << OCIE3A), TIMSK3);
    current_time = count0;
    SETMASK((1 << OCIE3A), TIMSK3);
    return current_time;

}

ISR(TIMER3_COMPA_vect){
    count3++;
}