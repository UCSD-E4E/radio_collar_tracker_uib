/*
 * compass.c
 *
 *  Created on: Aug 30, 2020
 *      Author: ntlhui
 */

#include "compass.h"


/**
 * Initializes the Compass module
 * @param  pConfig Pointer to a Compass_Config_t struct with the desired
 *                 configuration parameters
 * @return         1 if successful, 0 otherwise
 */
int Compass_Init(Compass_Config_t* pConfig)
{
//    TODO
    return 1;
}

/**
 * Reads the current heading from the compass module.  This is the magnetic
 * field vector projected onto the sensor's XY plane, reported in +/- decimal
 * degrees from magnetic North.
 * @param   pValue  Pointer to an int16_t variable to populate.
 * @return  1 if successful, 0 otherwise
 */
int Compass_Read(int16_t *pValue)
{
    // TODO
    return 1;
}
