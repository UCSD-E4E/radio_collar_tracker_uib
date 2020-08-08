/**
 * @file serial_sim.c
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
 * 08/06/20  NH  Initial commit
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "serial_sim.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>

#include <sys/types.h>
#include <sys/stat.h>
/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * Serial Simulator Descriptor
 */
typedef struct SerialSim_Desc_
{
	SerialDevice_e device;
	int pIn;
	int pOut;
	char* pInPipe;
	char* pOutPipe;
}SerialSim_Desc_t;

/******************************************************************************
 * Global Data
 ******************************************************************************/

/******************************************************************************
 * Module Static Data
 ******************************************************************************/
static SerialSim_Desc_t serialTable[] = 
{
	{SerialDevice_GPS, 0, 0, "gps_in", "gps_out"},
	{SerialDevice_OBC, 0, 0, "obc_in", "obc_out"},
	{SerialDevice_NULL, 0, 0, NULL, NULL}
};
/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Function Definitions
 ******************************************************************************/
SerialSim_Desc_t* SerialSim_FindDevice(SerialDevice_e device)
{
	SerialSim_Desc_t* pDevice;

	for(pDevice = serialTable; pDevice->device != SerialDevice_NULL; pDevice++)
	{
		if(device == pDevice->device)
		{
			return pDevice;
		}
	}
	return NULL;
}
/**
 * Initializes the Serial descriptor
 * @param  pConfig Serial Configuration
 * @return         1 if successful, otherwise 0
 */
SerialDesc_t* Serial_Init(SerialConfig_t* pConfig)
{
	SerialSim_Desc_t* pDesc = NULL;

	pDesc = SerialSim_FindDevice(pConfig->device);
	if(NULL == pDesc)
	{
		printf("Failed to find device\n");
		return NULL;
	}

	if(mkfifo(pDesc->pInPipe, 0777))
	{
		printf("Failed to create input pipe\n");
		return NULL;
	}

	if(mkfifo(pDesc->pOutPipe, 0777))
	{
		printf("Failed to create output pipe\n");
		return NULL;
	}

	pDesc->pIn = open(pDesc->pInPipe, O_RDONLY | O_NONBLOCK | O_CREAT, 0777);
	if(!pDesc->pIn)
	{
		printf("Failed to open input pipe\n");
		return NULL;
	}

	pDesc->pOut = open(pDesc->pOutPipe, O_RDONLY | O_NONBLOCK | O_CREAT, 0777);
	if(!pDesc->pOut)
	{
		printf("Failed to open output pipe\n");
		return NULL;
	}
	return (void*)pDesc;
}

/**
 * Checks if the specified serial port has data available
 * @param  pDesc Serial Descriptor
 * @return       1 if data available, otherwise 0
 */
int Serial_HasData(SerialDesc_t* pDesc)
{
	if(NULL == pDesc)
	{
		return 0;
	}

	return 0;

}