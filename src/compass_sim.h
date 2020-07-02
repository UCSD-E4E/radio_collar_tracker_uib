/**
 * @file compass.h
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * Radio Telemetry Tracker Compass Simulation Module Header
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
 * 07/01/20  NH  Initial commit
 */
#ifndef __COMPASS_SIM_H__
#define __COMPASS_SIM_H__

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "compass.h"

/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * Compass Simulator Configuration
 */
typedef struct Compass_Sim_Config
{
    char* path; //!< Path to datafile
}Compass_Sim_Config_t;

/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/
/**
 * Initializes the simulator aspects of the Compass module
 * @param  pConfig Pointer to the configuration struct
 * @return         1 if successful, otherwise 0
 */
int Compass_Sim_Init(Compass_Sim_Config_t* pConfig);

/**
 * Deinitializes the Compass module
 */
void Compass_Sim_Deinit(void);
#endif /* __COMPASS_SIM_H_ */
