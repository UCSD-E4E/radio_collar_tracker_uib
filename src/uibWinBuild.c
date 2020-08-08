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
 * 08/07/20  NH  Added serial and hardware initialization
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
#include "serial_sim.h"
#include "voltage_sim.h"
#include <pthread.h>
#include <time.h>

/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/
typedef struct globalParams_
{
	SerialDesc_t* pGPS;
	SerialDesc_t* pOBC;
}globalParams_t;
/******************************************************************************
 * Global Data
 ******************************************************************************/
globalParams_t globals;
/******************************************************************************
 * Module Static Data
 ******************************************************************************/
pthread_t timer;
/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/
static int init();
static int initSerial();
static int initVoltage();
static int initCompass();
static int initTimer();

static void* timerThread(void*);

void app()
{
	while(1);
}
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
	if(!init())
	{
		printf("Init failed\n");
		return -1;
	}

	app();

	return 0;
}

/**
 * Initializes all of the hardware
 * @return 1 if successful, otherwise 0
 */
static int init()
{
	if(!initSerial())
	{
		return 0;
	}

	if(!initCompass())
	{
		return 0;
	}

	if(!initVoltage())
	{
		return 0;
	}

	if(!initTimer())
	{
		return 0;
	}

	return 1;
}

/**
 * Initializes the Serial ports
 * @return 1 if successful, otherwise 0
 */
static int initSerial()
{
	SerialConfig_t serialConfig;


	serialConfig.device = SerialDevice_GPS;
	globals.pGPS = Serial_Init(&serialConfig);
	if(!globals.pGPS)
	{
		return 0;
	}

	serialConfig.device = SerialDevice_OBC;
	globals.pOBC = Serial_Init(&serialConfig);
	if(!globals.pOBC)
	{
		return 0;
	}

	return 1;
}

/**
 * Initializes the voltage module
 * @return 1 if successful, otherwise 0
 */
static int initVoltage()
{
	Voltage_Sim_Config_t config;
	config.path = "voltageSim";

	if(!Voltage_Sim_Init(&config))
	{
		return 0;
	}
	return 1;
}

/**
 * Initializes the compass module
 * @return 1 if successful, otherwise 0
 */
static int initCompass()
{
	Compass_Sim_Config_t config;
	config.path = "compassSim";

	if(!Compass_Sim_Init(&config))
	{
		return 0;
	}
	return 1;
}

/**
 * Initializes the timer module
 * @return 1 if successful, otherwise 0
 */
static int initTimer()
{
	if(pthread_create(&timer, NULL, &timerThread, NULL))
	{
		return 0;
	}
	return 1;
}

/**
 * Timer interrupt simulator
 */
static void* timerThread(void* argp)
{
	struct timespec itv = {0, 200000000};
	while(1)
	{
		nanosleep(&itv, NULL);
		// timerIrq();
	}
	return NULL;
}