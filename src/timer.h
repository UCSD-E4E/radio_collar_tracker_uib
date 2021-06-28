/*
 * I2C.h
 *
 *  Created on: June 3, 2021
 *      Author: DavidSalzmann
 */

#include <stdint.h>

uint32_t count0;
bool Timer0IntEn; 

int Timer0Innit();
int GetTimer0();
void Timer0Increment();
