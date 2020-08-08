/**
 * @file serial_sim.h
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * Radio Telemetry Tracker UI Board FW Windows Build.  This program provides a
 * way to test and simulate the UI Board FW without hardware (SIL).
 *
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
 * 08/07/20  NH  Initial commit
 */
#ifndef __SERIAL_SIM_H__
#define __SERIAL_SIM_H__

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "serial.h"
/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * Serial Configuration handle
 */
typedef struct SerialSim_Config_
{
	char* filename;
}SerialSim_Config_t;
/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/

#endif /* __SERIAL_SIM_H__ */