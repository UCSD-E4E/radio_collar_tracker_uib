/**
 * @file serial.h
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
 * 08/09/20  NH  Removed kbhit
 * 08/07/20  NH  Initial commit
 */
#ifndef __SERIAL_H__
#define __SERIAL_H__

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
/**
 * Serial Descriptor handle
 */
typedef void* SerialDesc_t;

/**
 * Serial Device enumerator
 */
typedef enum SerialDevice_
{
	SerialDevice_GPS,	//!< GPS Serial Device
	SerialDevice_OBC,	//!< OBC Serial Device
	SerialDevice_NULL
}SerialDevice_e;

/**
 * Serial Configuration handle
 */
typedef struct SerialConfig_
{
	SerialDevice_e device;
}SerialConfig_t;
/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/
/**
 * Initializes the Serial descriptor
 * @param  pConfig Serial Configuration
 * @return         1 if successful, otherwise 0
 */
SerialDesc_t* Serial_Init(SerialConfig_t* pConfig);

/**
 * Attempts to read data into the provided buffer.  If data is not available,
 * this function shall return 0 immediately.  If there is less than len bytes
 * available, this function shall copy those bytes into pBuf and return the
 * number of bytes copied into pBuf.  If there is more than len bytes available,
 * this function shall copy len bytes into pBuf and return len.
 * @param  pDesc Serial Descriptor
 * @param  pBuf  Pointer to Buffer
 * @param  len   Length of Buffer
 * @return       Number of bytes read
 */
int Serial_Read(SerialDesc_t* pDesc, uint8_t* pBuf, uint32_t len);

/**
 * Queues the specified data for transmit.  This function shall copy the 
 * data provided into a buffer for transmit.  If more data is specified than can
 * be buffered, this function shall fail.
 * @param  pDesc Serial Descriptor
 * @param  pBuf  Pointer to data
 * @param  len   Length of data
 * @return       1 if successful, otherwise 0
 */
int Serial_Write(SerialDesc_t* pDesc, uint8_t* pBuf, uint32_t len);
#endif /* __SERIAL_H__ */
