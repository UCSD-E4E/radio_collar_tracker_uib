#ifndef __SENSOR_ENCODER_H__
#define __SENSOR_ENCODER_H__

#include <stdio.h>
#include <string.h>
#include <stddef.h>

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

typedef struct DataSensorPacket_
{
    uint8_t sync_char1;
    uint8_t sync_char2;
    uint8_t packet_class;
    uint8_t packet_id;
    uint16_t payload_length;
    DataSensorInput_t payload;
} __attribute__((packed)) DataSensorPacket_t;

#endif /* __SENSOR_ENCODER_H__ */