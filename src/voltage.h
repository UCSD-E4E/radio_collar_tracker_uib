/**
 * @file voltage.h
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * Radio Telemetry Tracker Voltage Base Module Header
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
 * 07/02/20  NH  Initial commit
 */
#ifndef __VOLTAGE_H__
#define __VOLTAGE_H__

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/
/**
 * Initializes the Voltage module
 * @return  1 if successful, 0 otherwise
 */
int Voltage_Init(void);

/**
 * Reads the current voltage from the compass module.  This is the voltage of 
 * the 5V rail in mV
 * @param pValue Pointer to the variable to populate
 * @return  1 if successful, 0 otherwise
 */
int Voltage_Read(uint16_t* pValue);
#endif /* __VOLTAGE_H__ */