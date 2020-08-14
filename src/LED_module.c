/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
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
uint8_t led_state[5] = {0, 0, 0, 0, 0};
uint8_t led_buf;
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
 * LED initializer
 * @return      1 on success
 */
int LEDInit()
{
    led_buf = 0;
    return 1;
}

/**
 * Sets the target LED at index to the value "value"
 *
 * @param[in]  index  The target LED to be set
 * @param[in]  value  The value to set the LED to
 *
 * @return     0 if successful, otherwise 1
 */
int LEDsetState(int index, int value)
{
    if(index > 4)
    {
        return 1;
    }
    led_state[index] = value;

    return 0;
}

/**
 * LEDcontrol sets the LED buffer to 1 of 4 different states based on the value received from LEDsetState
 *
 * @return     returns 0
 */
int LEDcontrol()
{
    static int count[LED_MAPPING_END] = {0, 0, 0, 0, 0};
    static int led_flip[LED_MAPPING_END] = {0, 0, 0, 0, 0};
    for(int i = 0; i < LED_MAPPING_END; i++)
    {
        uint8_t value = led_state[i];
        if(value == LED_ON)
        {
            led_buf |= (1 << i); // turns LED on
            led_flip[i] = 0;
            count[i] = 0;
        }
        else if(value == LED_1HZ)
        {
            if(count[i] <= 5) // if the LED at i is 1
            {
                led_buf &= ~(1 << i); // turn the LED off
            }
            else if(count[i] > 5 && count[i] <= 10)
            {
                led_buf |= (1 << i); // turn the LED on
                count[i] = 0;
            }
            count[i]++;
            led_flip[i] = 0;
        }
        else if(value == LED_5HZ)
        {
            if(led_flip[i] % 2) // if the LED at i is 1
            {
                led_buf &= ~(1 << i); // turn that LED off
            }
            else // if the LED at i is 0
            {
                led_buf |= (1 << i); // turn that LED on
            }
            led_flip[i]++;
            count[i] = 0;
        }
        else // LED_OFF
        {
            led_buf &= ~(1 << i); // turns LED off
            led_flip[i] = 0;
            count[i] = 0;
        }
    }

    return 0;
}