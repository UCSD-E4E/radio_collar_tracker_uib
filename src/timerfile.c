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
    SETMASK((1 << CS30) | (1 << WGM32), TCCR3B); //Set CS32:0 to 1 so that Timer 3's clock is ClkIO, or 1Mhz
    CLEARMASK((1 << WGM30) | (1 << WGM31) | (1 << COM3A1) | (1 << COM3A0), TCCR3A); //Set WGM13:0 to 4 for Continuos Measurement Mode CTC
    CLEARMASK((1 << WGM33) | (1 << CS31) | (1 << CS32) | (1 << 0x05), TCCR3B); 
    SETMASK((1 << OCIE3A), TIMSK3); //Set Interrupt enable of Timer 3's output compare register
    //OCR3AH = 0x01; 
    //OCR3AL = 0xF3; //Set the top value of the Output Compare Register A for Timer 3 to be 499, or 0x01F3
    OCR3A = 0x01F3;
    count3 = 0x00000000; //reset Timer3's counter
    return 1;

}

int GetTimer3(){
    uint32_t current_time;
    CLEARMASK((1 << OCIE3A), TIMSK3);
    current_time = count3;
    SETMASK((1 << OCIE3A), TIMSK3);
    return current_time;

}

int GetCounter3(){
    uint16_t counter;
    counter = TCNT3H + (TCNT3L << 8);
    return counter;
}

ISR(TIMER3_COMPA_vect){
    count3++;
}