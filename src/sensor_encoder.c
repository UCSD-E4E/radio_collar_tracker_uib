#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "sensor_encoder.h"

#define POLY 0x1021

uint16_t crc16(uint8_t *buffer, uint32_t len) 
{
    uint32_t i, j;
    uint16_t crc = 0xFFFF;
    for (j = 0; j < len; j++)
    {
        uint8_t next_byte = buffer[j];
        for (i = 0; i < 8; i++)
        {
            if (((crc & 0x8000) >> 8) ^ (next_byte & 0x80))
            {
                crc = (crc << 1) ^ POLY;
            }
            else
            {
                crc = (crc << 1);
            }
            next_byte <<= 1;
        }
    }
    return crc;
}

uint32_t encodeSensorPacket(DataSensorPacket_t* data, uint8_t* buf, uint32_t len)
{
    uint16_t crc;
    if(len < sizeof(DataSensorPacket_t))
    {
        return 0;
    }
    buf[0] = data->sync_char1;
    buf[1] = data->sync_char2;
    buf[2] = data->packet_class;
    buf[3] = data->packet_id;
    memcpy(&buf[4], (uint8_t*) &data->payload_length, 2);
    if(data->packet_class == 4 && data->packet_id == 3)
    {
        buf[6] = data->payload.version;
        memcpy(&buf[7], (uint8_t*) &data->payload.time, 8);
        memcpy(&buf[15], (uint8_t*) &data->payload.latitude, 4);
        memcpy(&buf[19], (uint8_t*) &data->payload.longitude, 4);
        memcpy(&buf[23], (uint8_t*) &data->payload.altitude, 2);
        memcpy(&buf[25], (uint8_t*) &data->payload.heading, 2);
        memcpy(&buf[27], (uint8_t*) &data->payload.voltage, 2);
        buf[29] = data->payload.fixType;
    }
    crc = crc16(buf, 30);
    uint8_t *crc_ptr = (uint8_t*) &crc;
    buf[30] = crc_ptr[1];
    buf[31] = crc_ptr[0];
    return sizeof(DataSensorPacket_t) + 2;
}

void testDataSensor()
{
    DataSensorPacket_t packet;
    uint8_t buffer[256];
    uint32_t length;

    packet.sync_char1 = 0xE4;
    packet.sync_char2 = 0xEB;
    packet.packet_class = 0x04;
    packet.packet_id = 0x03;
    packet.payload_length = sizeof(DataSensorInput_t);
    packet.payload.version = 0x01;
    packet.payload.time = 1594462143333;
    packet.payload.latitude = 332030176;
    packet.payload.longitude = -1165082935;
    packet.payload.altitude = 12203;
    packet.payload.heading = -81;
    packet.payload.voltage = 4897;
    packet.payload.fixType = 1;
    length = encodeSensorPacket(&packet, buffer, sizeof(buffer));
    for(int i = 0; i < length; i ++)
    {
        printf("%02x ", buffer[i]);
        if((i % 2) == 0)
        {
            printf("\b");
        }
    }
    printf("\n");
}