#include <stdio.h>
#include <stdint.h>
#define MAX_STATUS 5

typedef enum LED_MAPPING_
{
    SYSTEM_STATE_LED = 0,
    STORAGE_STATE_LED,
    SDR_STATE_LED,
    GPS_STATE_LED,
    COMBINED_STATE_LED,
    LED_MAPPING_END
} LED_MAPPING_e;

typedef enum LED_STATE_
{
    LED_OFF = 0,
    LED_ON,
    LED_1HZ,
    LED_5HZ
} LED_STATE_e;