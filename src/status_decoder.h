/**
 * @file status_decoder.h
 *
 * @author Eugene Lee
 * 
 * @description 
 * Radio Telemetry Tracker status decoder header.
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

#ifndef __STATUS_DECODER_H__
#define __STATUS_DECODER_H__

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
 * Data for status packet payload
 */
typedef struct DataStatusInput_
{
    uint8_t version;
    uint8_t system_state;
    uint8_t sdr_state;
    uint8_t ext_sensor_state;
    uint8_t storage_state;
    uint8_t switch_state;
    uint64_t time;
} __attribute__((packed)) DataStatusInput_t;

/**
 * Data for the status packet
 */
typedef struct DataStatusPacket_
{
    uint8_t sync_char1;
    uint8_t sync_char2;
    uint8_t packet_class;
    uint8_t packet_id;
    uint16_t payload_length;
    DataStatusInput_t payload;
} __attribute__((packed)) DataStatusPacket_t;

#endif /* __STATUS_DECODER_H__ */