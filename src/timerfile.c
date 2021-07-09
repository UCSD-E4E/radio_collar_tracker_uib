/*
 * timerfile.c
 *
 *  Created on: May 28th, 2021
 *      Author: DavidSalzmann
 */

/******************************************************************************
 * Includes
 ******************************************************************************/

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

/******************************************************************************
 * File Macro Definitions
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Module Static Data
 ******************************************************************************/

/******************************************************************************
 * Module Global Data
 ******************************************************************************/
uint32_t count0;
/******************************************************************************
 * Function Definitions
 ******************************************************************************/
/**
 * Timer 0 Initialization Function
 * Set up Timer 0 to begin counting milliseconds
 * @return  1 if successful
 */
int Timer0Innit(){
    
    sei(); //Enable Global Interrupt
    SETMASK((1 << CS01) | (1 << WGM02) | (1 << CS00), TCCR0B); //Set CS0:2 to 2 so that Timer 0's clock is ClkIO/8, or 125kHz
    SETMASK((1 << WGM00) | (1 << WGM01), TCCR0A); 
    SETMASK((1 << OCIE0A), TIMSK0); //Set Interrupt enable of Timer 0's output compare register
    OCR0A = 0xF9; //Set the top value of the Output Compare Register A for Timer 0 to be 249

    count0 = 0; //reset Timer0's counter
    count0 = 0x00000000; //reset Timer0's counter
    return 1;

}

/**
 * Get Timer 0 Function
 * An atomic getter function to receive the current time in milliseconds since timer 0 was initiallized
 * @return  1 if successful, otherwise 0
 */
uint32_t GetTimer0(){

    uint32_t current_time;
    CLEARMASK((1 << OCIE0A), TIMSK0);//Disable the Compare Interrupt
    current_time = count0;
    SETMASK((1 << OCIE0A), TIMSK0);//Enable the Compare Interrupt
    return current_time;
}

ISR(TIMER0_COMPA_vect){
    count0++;
}