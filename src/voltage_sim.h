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
#ifndef __VOLTAGE_SIM_H__
#define __VOLTAGE_SIM_H__

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "voltage.h"
/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/
typedef struct Voltage_Sim_Config_
{
	const char* path;
} Voltage_Sim_Config_t;
/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/
/**
 * Initializes the Voltage module
 * @param pConfig Voltage Simulation configuration
 * @return  1 if successful, 0 otherwise
 */
int Voltage_Sim_Init(Voltage_Sim_Config_t* pConfig);

/**
 * Deinitializes the Voltage module
 */
void Voltage_Sim_Deinit(void);
#endif /* __VOLTAGE_SIM_H__ */