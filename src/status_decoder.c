/**
 * @file status_decoder.h
 *
 * @author Eugene Lee
 * 
 * @description
 * Radio Telemetry Tracker status decoder module. Takes the status data and
 * stores it into a buffer.
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
 * 08/16/20  EL  Renamed functions
 * 08/14/20  EL  Initial Commit
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "status_decoder.h"
#include "LED_module.h"

/******************************************************************************
 * Defines
 ******************************************************************************/
uint8_t ledcontrol_table[LED_MAPPING_END][MAX_STATUS] =
{
    {LED_5HZ, LED_OFF, LED_1HZ, LED_ON, LED_OFF}, // System LED
    {LED_5HZ, LED_ON, LED_OFF, LED_1HZ, LED_OFF}, // Storage LED
    {LED_5HZ, LED_ON, LED_OFF, LED_1HZ, LED_OFF}, // SDR LED
    {LED_5HZ, LED_ON, LED_OFF, LED_1HZ, LED_OFF}, // GPS LED
    {LED_OFF, LED_ON, LED_OFF, LED_OFF, LED_OFF}  // Combined LED
};
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
 * Decodes a sensor packet received from the On Board Computer
 *
 * @param      data  The packet received
 * @param      buf   The buffer to put the packet data into
 * @param[in]  len   The length of the buffer
 *
 * @return     returns 0 if successful, 1 if otherwise
 */
int decodeStatusPacket(DataStatusPacket_t* data, uint8_t input_byte)
{
    uint16_t crc;
    static int count = 0;
    static uint8_t tmp_buf[64];
    if(count)
    {
        tmp_buf[count] = input_byte;
        count++;
    }
    else if(input_byte == 0xe4)
    {
        tmp_buf[count] = input_byte;
        count = 1;
    }
    if(count == 22)
    {
        if(tmp_buf[1] == 0xeb)
        {
            crc = crc16(tmp_buf, 20);
            uint8_t *crc_ptr = (uint8_t*) &crc;
            if(crc_ptr[1] == tmp_buf[20] && crc_ptr[0] == tmp_buf[21])
            {
                data->sync_char1 = tmp_buf[0];
                data->sync_char2 = tmp_buf[1];
                data->packet_class = tmp_buf[2];
                data->packet_id = tmp_buf[3];
                memcpy((uint8_t*) &data->payload_length, (uint8_t*) &tmp_buf[4], 2);
                // if(data->packet_class == 4 && data->packet_id == 3)
                {
                    data->payload.version = tmp_buf[6];
                    data->payload.system_state = tmp_buf[7];
                    LEDsetState(SYSTEM_STATE_LED, ledcontrol_table[SYSTEM_STATE_LED][data->payload.system_state]);
                    data->payload.sdr_state = tmp_buf[8];
                    LEDsetState(SDR_STATE_LED, ledcontrol_table[SDR_STATE_LED][data->payload.sdr_state]);
                    data->payload.ext_sensor_state = tmp_buf[9];
                    LEDsetState(GPS_STATE_LED, ledcontrol_table[GPS_STATE_LED][data->payload.ext_sensor_state]);
                    data->payload.storage_state = tmp_buf[10];
                    LEDsetState(STORAGE_STATE_LED, ledcontrol_table[STORAGE_STATE_LED][data->payload.storage_state]);
                    data->payload.switch_state = tmp_buf[11];
                    LEDsetState(COMBINED_STATE_LED, ledcontrol_table[COMBINED_STATE_LED][data->payload.switch_state]);
                    memcpy((uint8_t*) &data->payload.time, (uint8_t*) &tmp_buf[12], 8);
                }
                count = 0;
                return 0;
            }
        }
        for(int i = 1; i < 22; i++)
        {
            if(tmp_buf[i] == 0xe4)
            {
                memcpy(tmp_buf, &tmp_buf[i], 22 - i);
                count = 22 - i;
            }
        }
    }
    return 1;
}