#include <stdio.h>
#include <stdint.h>
#include "LED_module.h"

uint8_t ledcontrol_table[LED_MAPPING_END][MAX_STATUS] =
{
    {LED_5HZ, LED_OFF, LED_1HZ, LED_ON, LED_OFF}, // System LED
    {LED_5HZ, LED_ON, LED_OFF, LED_1HZ, LED_OFF}, // Storage LED
    {LED_5HZ, LED_ON, LED_OFF, LED_1HZ, LED_OFF}, // SDR LED
    {LED_5HZ, LED_ON, LED_OFF, LED_1HZ, LED_OFF}, // GPS LED
    {LED_OFF, LED_ON, LED_OFF, LED_OFF, LED_OFF}  // Combined LED
};

uint8_t led_state[5] = {0, 0, 0, 0, 0};
uint8_t led_buf;

uint8_t LEDInit()
{
    led_buf = 0;
    return 0;
}
int LEDsetState(int index, int value)
{
    if(index > 4)
    {
        return 1;
    }
    led_state[index] = value;
    return 0;
}

int LEDcontrol()
{
    uint32_t sleep_count = 0;
    sleep(0.1);
    sleep_count++; //use sleep here?

    for(int i = 0; i < LED_MAPPING_END; i++)
    {
        uint8_t value = led_state[i];
        if(value == LED_ON)
        {
            led_buf |= (1 << i); // turns LED on
        }
        else if(value == LED_1HZ)
        {
            if(led_buf == 1) // fix (led_buf bit)
            {
                led_buf &= ~(1 << i);
            }
            else if(led_buf == 0) // do same
            {
                led_buf |= (1 << i);
            }
        }
        else if(value == LED_5HZ)
        {
            // blink 5 hertz
        }
        else // LED_OFF
        {
            led_buf &= ~(1 << i); // turns LED off
        }
    }

    return 0;
}

// write the LED uint8_t value into a file.