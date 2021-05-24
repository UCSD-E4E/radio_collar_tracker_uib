/*
 * compass.c
 *
 *  Created on: Aug 30, 2020
 *      Author: ntlhui
 */

#include "compass.h"
#include "I2C.h"
#include <math.h>

/**
 * Initializes the Compass module
 * @param  pConfig Pointer to a Compass_Config_t struct with the desired
 *                 configuration parameters
 * @return         1 if successful, 0 otherwise
 */
int Compass_Init(Compass_Config_t* pConfig)
{
    uint8_t address = 0x1E;
    uint8_t register_address = 0x00;
    uint8_t data[3] = {};
    uint8_t data_size = 0x03;
    uint32_t timeout_ms = 0x00; //not yet implemented

    //Set up CRA with TS enabled, 8 averaging, Data output to 30Hz
    register_address = 0x00;
    data[0] = 0xF4;
    data[1] = 0x20; 
    data[2] = pConfig->measMode; //sends the given measurement mode to the Mode Register on the compass

    if(I2C_MasterRegisterTransmit(address, register_address, data, data_size, timeout_ms) == 1){ //with *data_ptr being 0x00, this will tell the compass to address it's 0th   
        return 1;
    }else{
        return 0;
    }
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
    uint8_t address = 0x1E;
    uint8_t register_address = 0x03;
    uint8_t data[6] = {};
    uint8_t data_size = 0x06;
    uint32_t timeout_ms = 0x00; //not yet implemented

    signed int16_t dataX = 0x00;
    signed int16_t dataY = 0x00;
    signed int16_t dataZ = 0x00;

    float headerRad = 0x00;
    
    read_check = I2C_MasterRegisterReceive(address, register_address, data, data_size, timeout_ms);

    dataX = (data[0]) + (data[1] << 8); //String together the MSB and LSB of data for each axis
    dataZ = (data[2]) + (data[3] << 8); 
    dataY = (data[4]) + (data[5] << 8);

    headerRad = atan2((float)dataY, (float)dataX); //find the distance from 
    *pValue = (uint16_t)(headerRad*360/(2*PI));

    return 1;
}
