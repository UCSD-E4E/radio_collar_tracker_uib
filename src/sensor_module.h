/**
 * @file sensor_module.h
 *
 * @author Eugene Lee
 * 
 * @description 
 * Radio Telemetry Tracker sensor module header.
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
 * 08/23/20  NH  Removed return types enum, removed extern from other module,
 *                 added symbol declarations
 * 08/16/20  EL  Added documentation
 * 08/14/20  EL  Initial commit
 */

#ifndef __SENSOR_MODULE_H__
#define __SENSOR_MODULE_H__
/******************************************************************************
 * Includes
 ******************************************************************************/
#include "sensor_encoder.h"
#include "sensor_module.h"

#include "nmea.h"
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
 * encodes a receives sensor packet and puts the data into a buffer, buf, of length len.
 */
uint32_t encodeSensorPacket(DataSensorPacket_t* data, uint8_t* buf, uint32_t len);

/**
 * Decodes an NMEA GPS message
 */
NMEA_Message_e NMEA_Decode(char c);

/**
 * Parses a sensor packet char by char
 *
 * @param[in]  c     the sensor packet in characters
 *
 * @return     returns PARSE_COMPLETE if successful, PARSE_NOT_COMPLETE otherwise
 */
int sensorParse(char c);

/**
 * The decoded sensor packet data.
 */
extern DataSensorPacket_t sensor_packet;

#endif /* __SENSOR_MODULE_H__ */
