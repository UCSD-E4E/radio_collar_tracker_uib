#ifndef __STATUS_DECODER_H__
#define __STATUS_DECODER_H__

#include <stdio.h>
#include <string.h>
#include <stddef.h>

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