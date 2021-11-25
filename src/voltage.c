/*
 * voltage.c
 *
 *  Created on: Aug 30, 2020
 *      Author: ntlhui
 */

#include <stdint.h>


/**
 * Initializes the Voltage module
 * @return  1 if successful, 0 otherwise
 */
int Voltage_Init(void)
{
    return 1;
}

/**
 * Reads the current voltage from the compass module.  This is the voltage of
 * the 5V rail in mV
 * @param pValue Pointer to the variable to populate
 * @return  1 if successful, 0 otherwise
 */
int Voltage_Read(uint16_t* pValue)
{
    return 1;
}
