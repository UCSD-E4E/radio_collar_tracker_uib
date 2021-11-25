/**
 * @file compass.h
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * Radio Telemetry Tracker Compass Base Module Header
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * DATE      WHO DESCRIPTION
 * ----------------------------------------------------------------------------
 * 07/02/20  NH  Fixed organization, fixed value
 * 07/01/20  NH  Initial commit
 */
#ifndef __COMPASS_H__
#define __COMPASS_H__

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
/******************************************************************************
 * Defines
 ******************************************************************************/
#define ABSDATAMAX 0x0800
#define PI 3.14159265
/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * Compass Measurement Mode.
 */
typedef enum Compass_Mode_
{
    Compass_Mode_Continuous = 0,    //!< Continuous measurement mode
    Compass_Mode_Single = 1,        //!< Single measurement mode
    Compass_Mode_Idle = 2          //!< Idle
}Compass_Mode_e;
/**
 * Compass Configuration Struct
 */
typedef struct Compass_Config_
{
    Compass_Mode_e measMode;    //!< Desired Measurement mode
}Compass_Config_t;

/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/
/**
 * Initializes the Compass module
 * @param  pConfig Pointer to a Compass_Config_t struct with the desired 
 *                 configuration parameters
 * @return         1 if successful, 0 otherwise
 */
int Compass_Init(Compass_Config_t* pConfig);

/**
 * Reads the current heading from the compass module.  This is the magnetic
 * field vector projected onto the sensor's XY plane, reported in +/- decimal 
 * degrees from magnetic North.
 * @param	pValue	Pointer to an int16_t variable to populate.
 * @return  1 if successful, 0 otherwise
 */
int Compass_Read(int16_t *pValue);
#endif /* __COMPASS_H_ */