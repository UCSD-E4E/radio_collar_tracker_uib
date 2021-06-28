/*
 * I2C.c
 *
 *  Created on: June 3, 2021
 *      Author: DavidSalzmann
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "timer.h"

#include <avr/io.h>
#include <stdint.h>
#include "cutils.h"
#include "serial.h"
#include <stddef.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

uint32_t count0;

int Timer0Innit(){
    
    sei();
    SETMASK((1 << CS01) | (1 << WGM02), TCCR0B); //Set CS0:2 to 2 so that Timer 0's clock is ClkIO/8, or 125kHz
    SETMASK((1 << WGM00) | (1 << WGM01), TCCR0A); 
    SETMASK((1 << OCIE0A), TIMSK0); //Set Interrupt enable of Timer 0's output compare register
    OCR0A = 0x7C; //Set the top value of the Output Compare Register A for Timer 0 to be 124

    count0 = 0; //reset Timer0's counter
    return 1;

}

int GetTimer0(){
    CLEARMASK((1 << OCIE0A), TIMSK0);
    return count0;
    SETMASK((1 << OCIE0A), TIMSK0);
}

ISR(TIMER0_COMPA_vect){
    count0++;
}
