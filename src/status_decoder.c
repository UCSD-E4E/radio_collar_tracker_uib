#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "status_decoder.h"
#include "LED_module.h"

extern uint16_t crc16(uint8_t *buffer, uint32_t len);
extern uint8_t ledcontrol_table[LED_MAPPING_END][MAX_STATUS];
extern uint8_t LEDsetState(int index, int value);

int decodeSensorPacket(DataStatusPacket_t* data, uint8_t* buf, uint32_t len)
{
    uint16_t crc;
    crc = crc16(buf, 20);
    uint8_t *crc_ptr = (uint8_t*) &crc;
    if(crc_ptr[1] == buf[20] && crc_ptr[0] == buf[21])
    {
        if(len < sizeof(DataStatusPacket_t))
        {
            return 0;
        }
        data->sync_char1 = buf[0];
        data->sync_char2 = buf[1];
        data->packet_class = buf[2];
        data->packet_id = buf[3];
        memcpy((uint8_t*) &data->payload_length, (uint8_t*) &buf[4], 2);
        printf("length = %x\n", data->payload_length);
        if(data->packet_class == 4 && data->packet_id == 3)
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
            printf("time = %I64x\n", data->payload.time);
        }
        return 0;
    }
    return 1;
}

void testDataSensorDecoder()
{
    DataStatusPacket_t packet;
    uint8_t buffer[256];
    buffer[0] = 0xE4;
    buffer[1] = 0xEB;
    buffer[2] = 0x04;
    buffer[3] = 0x03;
    buffer[4] = 0x18;
    buffer[5] = 0x00;
    buffer[6] = 0x01;
    buffer[7] = 0x04;
    buffer[8] = 0x02;
    buffer[9] = 0x03;
    buffer[10] = 0x01;
    buffer[11] = 0x00;
    buffer[12] = 0x90;
    buffer[13] = 0x74;
    buffer[14] = 0x24;
    buffer[15] = 0x81;
    buffer[16] = 0x3c;
    buffer[17] = 0x71;
    buffer[18] = 0x08;
    buffer[19] = 0x16;
    buffer[20] = 0xba;
    buffer[21] = 0x0b;
    decodeSensorPacket(&packet, buffer, sizeof(buffer));
}