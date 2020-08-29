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
 * 08/23/20  NH  Fixed decoder, refactored LED encoding into separate function
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
typedef enum StatusDecoderState_
{
    StatusDecoderState_Sync1,
    StatusDecoderState_Sync2,
    StatusDecoderState_Header,
    StatusDecoderState_Payload,
}StatusDecoderState_e;
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
    static int count = 0;
    static uint8_t tmp_buf[64];
    PhysicalPacketHeader_t* header = (PhysicalPacketHeader_t*) tmp_buf;

    static StatusDecoderState_e decoderState = StatusDecoderState_Sync1;

    switch(decoderState)
    {
    case StatusDecoderState_Sync1:
        if(input_byte == 0xE4)
        {
            count = 0;
            tmp_buf[count] = input_byte;
            count++;
            decoderState++;
        }
        break;
    case StatusDecoderState_Sync2:
        if(input_byte == 0xEB)
        {
            tmp_buf[count++] = input_byte;
            decoderState++;
        }
        else
        {
            decoderState = StatusDecoderState_Sync1;
        }
        break;
    case StatusDecoderState_Header:
        tmp_buf[count++] = input_byte;

        if(count == 7)
        {
            // header read in
            if(header->class == 1 && header->id == 1)
            {
                // heartbeat packet
                decoderState++;
            }
            else
            {
                decoderState = StatusDecoderState_Sync1;
            }
        }
        break;
    case StatusDecoderState_Payload:
        tmp_buf[count++] = input_byte;

        if(count == header->len + 8)
        {
            // packet received
            if(crc16(tmp_buf, header->len + 8) == 0)
            {
                memcpy(data, tmp_buf, sizeof(DataStatusPacket_t));
                decoderState = StatusDecoderState_Sync1;
                return 1;
            }
        }
        break;
    }
    return 0;
}

/**
 * Encodes the LED states based on the heartbeat information provided.
 * @param pHeartbeat    Heartbeat information
 */
void StatusDecoder_encodeLEDs(DataStatusInput_t* pHeartbeat)
{
    LEDsetState(SYSTEM_STATE_LED, ledcontrol_table[SYSTEM_STATE_LED][pHeartbeat->system_state]);
    LEDsetState(SDR_STATE_LED, ledcontrol_table[SDR_STATE_LED][pHeartbeat->sdr_state]);
    LEDsetState(GPS_STATE_LED, ledcontrol_table[GPS_STATE_LED][pHeartbeat->ext_sensor_state]);
    LEDsetState(STORAGE_STATE_LED, ledcontrol_table[STORAGE_STATE_LED][pHeartbeat->storage_state]);
    LEDsetState(COMBINED_STATE_LED, ledcontrol_table[COMBINED_STATE_LED][pHeartbeat->switch_state]);
}
