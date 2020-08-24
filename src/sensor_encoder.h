/**
 * @file sensor_encoder.h
 *
 * @author Eugene Lee
 * 
 * @description 
 * Radio Telemetry Tracker sensor encoder header.
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
 * 08/16/20  EL  Added documentation
 * 08/14/20  EL  Initial commit
 */

#ifndef __SENSOR_ENCODER_H__
#define __SENSOR_ENCODER_H__
/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stddef.h>
/******************************************************************************
 * Defines
 ******************************************************************************/
/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * Data for sensor packet payload
 */
typedef struct DataSensorInput_
{
    uint8_t version;
    uint64_t time;
    int32_t latitude;
    int32_t longitude;
    uint16_t altitude;
    int16_t heading;
    uint16_t voltage;
    uint8_t fixType;
} __attribute__((packed)) DataSensorInput_t;

/**
 * Data for the sensor packet
 */
typedef struct DataSensorPacket_
{
    uint8_t sync_char1;
    uint8_t sync_char2;
    uint8_t packet_class;
    uint8_t packet_id;
    uint16_t payload_length;
    DataSensorInput_t payload;
} __attribute__((packed)) DataSensorPacket_t;

/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/

#endif /* __SENSOR_ENCODER_H__ */