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
 * 08/23/20  NH  Removed externs, added NMEA and sim setup, fixed output and
 *                 action sequences, fixed compass init
 * 08/13/20  EL  Tied the LED module, encoder, and decoder together
 * 08/09/20  NH  Added boilerplate code
 * 08/07/20  NH  Added serial and hardware initialization
 * 07/02/20  NH  Initial commit
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "uibWinBuild.h"
#include "compass_sim.h"
#include "nmea.h"
#include "serial_sim.h"
#include "voltage_sim.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "sensor_module.h"
#include "status_decoder.h"
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
} globalParams_t;
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
static int initNMEA();
static void sim_setup();

static void* timerThread(void*);

void app()
{
	uint8_t rxBuf[64];
	int nchars;
    DataStatusPacket_t status_data;

    while(1)
	{
		nchars = Serial_Read(globals.pGPS, rxBuf, 63);
		if(nchars > 0)
		{
			for(int i = 0; i < nchars; i++)
			{
				if(sensorParse(rxBuf[i]) == 1)
				{
				    Serial_Write(globals.pOBC, (uint8_t *) &sensor_packet, sizeof(sensor_packet));
				}
			}
		}

		nchars = Serial_Read(globals.pOBC, rxBuf, 63);
		if(nchars > 0)
		{
			for(int i = 0; i < nchars; i++)
			{
				if(decodeStatusPacket(&status_data, rxBuf[i]))
				{
				    StatusDecoder_encodeLEDs(&status_data.payload);
				}
			}
		}
	}
}

void timer1_IRQ()
{
	LED_control();
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
    sim_setup();
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
	    printf("Serial init failed\n");
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

	if(!LED_init())
	{
		return 0;
	}

	if(!initNMEA())
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
	    printf("GPS init failed\n");
		return 0;
	}

	serialConfig.device = SerialDevice_OBC;
	globals.pOBC = Serial_Init(&serialConfig);
	if(!globals.pOBC)
	{
	    printf("OBC init failed\n");
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
	Compass_Sim_Config_t simConfig;
	Compass_Config_t cConfig;
	simConfig.path = "compassSim";
	cConfig.measMode = Compass_Mode_Continuous;

	if(!Compass_Sim_Init(&simConfig))
	{
		return 0;
	}

	if(!Compass_Init(&cConfig))
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
	struct timespec itv = {0, 100000000};
	while(1)
	{
		nanosleep(&itv, NULL);
		timer1_IRQ();
	}
	return NULL;
}

/**
 * Initializes the NMEA module
 * @return
 */
static int initNMEA()
{
    NMEA_Config_t config;

    config.messageMask = NMEA_MESSAGE_GGA | NMEA_MESSAGE_GLL | NMEA_MESSAGE_ZDA;
    NMEA_Init(&config);

    return 1;
}

/**
 * Initializes the environment for the simulation
 */
static void sim_setup()
{
    if(access("gps_in", F_OK) != 1)
    {
        remove("gps_in");
    }
    if(access("gps_out", F_OK) != 1)
    {
        remove("gps_out");
    }
    if(access("obc_in", F_OK) != 1)
    {
        remove("obc_in");
    }
    if(access("obc_out", F_OK) != 1)
    {
        remove("obc_out");
    }
}
