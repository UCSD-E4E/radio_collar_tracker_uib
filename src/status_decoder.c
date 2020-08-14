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
extern uint16_t crc16(uint8_t *buffer, uint32_t len);
extern uint8_t ledcontrol_table[LED_MAPPING_END][MAX_STATUS];
extern uint8_t LEDsetState(int index, int value);
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
int decodeSensorPacket(DataStatusPacket_t* data, uint8_t* buf, uint32_t len)
{
    uint16_t crc;
    crc = crc16(buf, 20);
    uint8_t *crc_ptr = (uint8_t*) &crc;
    if(crc_ptr[1] == buf[20] && crc_ptr[0] == buf[21])
    {
        if(len < sizeof(DataStatusPacket_t))
        {
            return 1;
        }
        data->sync_char1 = buf[0];
        data->sync_char2 = buf[1];
        data->packet_class = buf[2];
        data->packet_id = buf[3];
        memcpy((uint8_t*) &data->payload_length, (uint8_t*) &buf[4], 2);
        // if(data->packet_class == 4 && data->packet_id == 3)
        {
            data->payload.version = buf[6];
            data->payload.system_state = buf[7];
            LEDsetState(SYSTEM_STATE_LED, ledcontrol_table[SYSTEM_STATE_LED][data->payload.system_state]);
            data->payload.sdr_state = buf[8];
            LEDsetState(SDR_STATE_LED, ledcontrol_table[SDR_STATE_LED][data->payload.sdr_state]);
            data->payload.ext_sensor_state = buf[9];
            LEDsetState(GPS_STATE_LED, ledcontrol_table[GPS_STATE_LED][data->payload.ext_sensor_state]);
            data->payload.storage_state = buf[10];
            LEDsetState(STORAGE_STATE_LED, ledcontrol_table[STORAGE_STATE_LED][data->payload.storage_state]);
            data->payload.switch_state = buf[11];
            LEDsetState(COMBINED_STATE_LED, ledcontrol_table[COMBINED_STATE_LED][data->payload.switch_state]);
            memcpy((uint8_t*) &data->payload.time, (uint8_t*) &buf[12], 8);
            // printf("time = %lx\n", data->payload.time);
        }
        return 0;
    }
    return 1;
}