/**
 * @file uibWinBuild.h
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
 * 07/02/20  NH  Initial commit
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "compass_sim.h"
#include "nmea.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * Global Data
 ******************************************************************************/

/******************************************************************************
 * Module Static Data
 ******************************************************************************/

/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Function Definitions
 ******************************************************************************/
/**
 * Program entry point
 * @param  argc Arg Count
 * @param  argv Arg Vector
 * @return      0 on success, error code otherwise
 */
int main(int argc, char const *argv[])
{
	const char* nmeaInputFile = "nmeaTestData.txt";
	const char* compassSimFile = "compassSim.bin";
	const char* voltageSimFile = "voltageSim.bin";

	
	return 0;
}
