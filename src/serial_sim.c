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
 * 08/23/20  NH  Moved simulator setup before fifo open, fixed fifo open return
 *                 value checking, added GPS out receiver, added GPS data over
 *                 E4EB output to file.
 * 08/09/20  NH  Implemented full simulation setup
 * 08/06/20  NH  Initial commit
 */

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "serial_sim.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

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
	void* (*simulatorThread) (void*);
	pthread_t thread;
}SerialSim_Desc_t;

typedef struct __attribute__((packed)) heartbeatPayload_
{
	uint8_t version;
	uint8_t systemState;
	uint8_t sdrState;
	uint8_t externalSensorState;
	uint8_t storageState;
	uint8_t switchState;
	uint64_t time;
}heartbeatPayload_t;

typedef struct __attribute__((packed)) heartbeatTransport_
{
	uint8_t sync1;
	uint8_t sync2;
	uint8_t class;
	uint8_t id;
	uint16_t len;
	heartbeatPayload_t payload;
	uint16_t checksum;
}heartbeatTransport_t;

/******************************************************************************
 * Global Data
 ******************************************************************************/

/******************************************************************************
 * Module Static Data
 ******************************************************************************/
static SerialSim_Desc_t serialTable[] = 
{
	{SerialDevice_GPS, 0, 0, "./gps_in", NULL, gpsSimulator, 0},
	{SerialDevice_OBC, 0, 0, "./obc_in", "./obc_out", obcSimulator, 0},
	{SerialDevice_NULL, 0, 0, NULL, NULL, NULL, 0}
};

/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/
static int encodeHeartbeat(uint8_t *pBuf, uint32_t bufLen, uint8_t sysState,
		uint8_t sdrState, uint8_t extState, uint8_t strState, uint8_t swState);
static uint16_t crc16(uint8_t* pData, uint32_t length);
static int createFIFOPipe(char* name);

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
 * @return         Initialized serial device if successful, otherwise NULL
 */
SerialDesc_t* Serial_Init(SerialConfig_t* pConfig)
{
	SerialSim_Desc_t* pDesc = NULL;
	int				  flags = 0;
	int				  tempOutPipeReader;

	pDesc = SerialSim_FindDevice(pConfig->device);
	if(NULL == pDesc)
	{
		printf("Failed to find device\n");
		return NULL;
	}

    if(pDesc->pInPipe)
    {
        if(!createFIFOPipe(pDesc->pInPipe))
        {
            printf("Failed to create input pipe: %s\n", strerror(errno));
            return NULL;
        }

        flags |= O_RDONLY | O_NONBLOCK | O_CREAT;
        pDesc->pIn = open(pDesc->pInPipe, flags, 0777);
        if(pDesc->pIn == -1)
        {
            printf("Failed to open input pipe\n");
            return NULL;
        }
    }

    if(pDesc->pOutPipe)
    {
        if(!createFIFOPipe(pDesc->pOutPipe))
        {
            printf("Failed to create output pipe: %s\n", strerror(errno));
            return NULL;
        }

        tempOutPipeReader = open(pDesc->pOutPipe, O_RDONLY | O_NONBLOCK);
        flags |= O_WRONLY | O_CREAT;
        pDesc->pOut = open(pDesc->pOutPipe, flags, 0777);
        if(pDesc->pOut == -1)
        {
            printf("Failed to open output pipe\n");
            close(tempOutPipeReader);
            return NULL;
        }
    }

    if(pDesc->simulatorThread != NULL)
    {
        pthread_create(&pDesc->thread, NULL, pDesc->simulatorThread, NULL);
    }

	close(tempOutPipeReader);
	return (void*)pDesc;
}

/**
 * Attempts to read data into the provided buffer.  If data is not available,
 * this function shall return 0 immediately.  If there is less than len bytes
 * available, this function shall copy those bytes into pBuf and return the
 * number of bytes copied into pBuf.  If there is more than len bytes available,
 * this function shall copy len bytes into pBuf and return len.
 * @param  pDesc Serial Descriptor
 * @param  pBuf  Pointer to Buffer
 * @param  len   Length of Buffer
 * @return       Number of bytes read, -1 on error
 */
int Serial_Read(SerialDesc_t* pDesc, uint8_t* pBuf, size_t len)
{
	SerialSim_Desc_t *pHandle = (SerialSim_Desc_t*) pDesc;

	if(NULL == pHandle || NULL == pBuf)
	{
		printf("Serial_Read: NULL parameter\n");
		return -1;
	}

	return read(pHandle->pIn, pBuf, len);
}


/**
 * Queues the specified data for transmit.  This function shall copy the
 * data provided into a buffer for transmit.  If more data is specified than can
 * be buffered, this function shall fail.
 * @param  pDesc Serial Descriptor
 * @param  pBuf  Pointer to data
 * @param  len   Length of data
 * @return       Number of bytes written, -1 on error
 */
int Serial_Write(SerialDesc_t* pDesc, uint8_t* pBuf, size_t len)
{
	SerialSim_Desc_t *pHandle = (SerialSim_Desc_t*) pDesc;

	if(NULL == pHandle || NULL == pBuf)
	{
		return -1;
	}

	// We will be using a 256 byte r/w buffer
	if(len > 256)
	{
		return -1;
	}

	return write(pHandle->pOut, pBuf, len);
}

/**
 * GPS Simulator thread
 */
void* gpsSimulator(void* argp)
{

	FILE* gpsData = fopen("gps.txt", "r");
	char* lineptr = NULL;
	size_t bufLen = 0;
	size_t nChars = 0;
	struct timespec itv = {1, 0};
	int gpsPipe = open("gps_in", O_WRONLY);
	int run = 1;

	if(!gpsData)
	{
		printf("GPS data not found\n!");
		return NULL;
	}

	if(gpsPipe == -1)
	{
		printf("Failed to open GPS pipe\n");
		return NULL;
	}

	while(run)
	{
		nChars = getline(&lineptr, &bufLen, gpsData);
		switch(nChars)
		{
		case 1:
		case 2:
			nanosleep(&itv, NULL);
			break;
		case -1:
			run = 0;
			break;
		default:
			// output data to GPS pipe
			write(gpsPipe, lineptr, nChars);
			break;
		}
	}

	free(lineptr);
	return NULL;
}

/**
 * OBC Simulator thread
 */
void* obcSimulator(void* argp)
{
	uint8_t heartbeatBuffer[25];
	uint8_t gpsDataBuffer[1024];
	struct timespec itv = {2, 0};
	int obcOutPipe = open("obc_out", O_RDONLY | O_NONBLOCK);
	int obcInPipe = open("obc_in", O_WRONLY);
	int nChars = 0;
	FILE* gpsFile = fopen("gps.bin", "wb");

	uint8_t sysState = 0, sdrState = 0, sensorState = 0, storageState = 0,
			switchState = 0;
	uint8_t sdrMax = 5, sensorMax = 5, storageMax = 6, systemMax = 7,
			switchMax = 2;

	if(obcInPipe == -1)
	{
		printf("Failed to open OBC input pipe\n");
		return NULL;
	}

	if(obcOutPipe == -1)
	{
		printf("Failed to open OBC output pipe\n");
		return NULL;
	}
	if(!gpsFile)
	{
	    printf("Failed to open GPS output file\n");
	    printf("%s\n", strerror(errno));
	    return NULL;
	}

	while(1)
	{
		nChars = encodeHeartbeat(heartbeatBuffer, 25, sysState++ % systemMax,
				sdrState++ % sdrMax, sensorState++ % sensorMax,
				storageState++ % storageMax, switchState++ % switchMax);
		if(nChars)
		{
		    write(obcInPipe, "\r\n", 2);
			write(obcInPipe, heartbeatBuffer, nChars);
		}

		nChars = read(obcOutPipe, gpsDataBuffer, 1024);
		if(nChars != -1)
		{
            fwrite(gpsDataBuffer, nChars, 1, gpsFile);
            fflush(gpsFile);
		}
		
		nanosleep(&itv, NULL);

	}

	return NULL;
}

/**
 * Encodes a heartbeat packet
 * @param pBuf	Buffer to populate
 * @param bufLen	Length of buffer
 * @param sysState	System State
 * @param sdrState	SDR State
 * @param extState	External Sensors State
 * @param strState	Storage State
 * @param swState	Switch State
 * @return	Number of bytes used
 */
static int encodeHeartbeat(uint8_t *pBuf, uint32_t bufLen, uint8_t sysState,
		uint8_t sdrState, uint8_t extState, uint8_t strState, uint8_t swState)
{
	size_t nBytes = sizeof(heartbeatTransport_t);
	heartbeatTransport_t* pStruct = (heartbeatTransport_t*) pBuf;
	struct timespec spec;
	uint16_t cksum;
	uint8_t* pCksum = (uint8_t*) &cksum;
	uint8_t* pSum;

	if(bufLen < nBytes)
	{
		return -1;
	}

	if(NULL == pBuf)
	{
		return -1;
	}
	pSum = (uint8_t*) &pStruct->checksum;

	clock_gettime(CLOCK_REALTIME, &spec);

	pStruct->sync1 = 0xE4;
	pStruct->sync2 = 0xEB;
	pStruct->class = 0x01;
	pStruct->id = 0x01;
	pStruct->len = sizeof(heartbeatPayload_t);

	pStruct->payload.externalSensorState = extState;
	pStruct->payload.sdrState = sdrState;
	pStruct->payload.storageState = strState;
	pStruct->payload.switchState = swState;
	pStruct->payload.systemState = sysState;

	pStruct->payload.time = spec.tv_sec * 1000 + spec.tv_nsec / 1e6;

	cksum = crc16((uint8_t*) pStruct, sizeof(heartbeatTransport_t) - sizeof(uint16_t));

	pSum[0] = pCksum[1];
	pSum[1] = pCksum[0];

	/*
	 * Check that checksum is valid
	 */
	if(0 != crc16((uint8_t*) pStruct, sizeof(heartbeatTransport_t)))
	{
		printf("CRC failed!\n");
	}

	return nBytes;
}

/*
//                                      16   12   5
// this is the CCITT CRC 16 polynomial X  + X  + X  + 1.
// This works out to be 0x1021, but the way the algorithm works
// lets us use 0x8408 (the reverse of the bit pattern).  The high
// bit is always assumed to be set, thus we only use 16 bits to
// represent the 17 bit value.
*/
static uint16_t crc16(uint8_t* pData, uint32_t length)
{
    uint8_t i;
    uint16_t wCrc = 0xffff;
    while (length--) {
        wCrc ^= *(unsigned char *)pData++ << 8;
        for (i=0; i < 8; i++)
            wCrc = wCrc & 0x8000 ? (wCrc << 1) ^ 0x1021 : wCrc << 1;
    }
    return wCrc & 0xffff;
}

/**
 * Creates a FIFO pipe if it doesn't exist
 * @param name Desired name of FIFO pipe
 * @return     1 if successful, 0 otherwise
 */
static int createFIFOPipe(char* name)
{
    struct stat buf;

    // Check if a file with the desired name already exists
    if(stat(name, &buf))
    {
        // Attempt to create pipe since it doesn't exist
        return !mkfifo(name, 0777);
    }

    // Check if existing file is a FIFO pipe
    return S_ISFIFO(buf.st_mode);
}
