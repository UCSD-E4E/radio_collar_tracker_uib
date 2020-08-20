/**
 * @file LED_module.h
 * 
 * @author Eugene Lee
 * 
 * @description
 * Radio Telemetry Tracker LED module. 
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
 * 08/16/20  EL  Added documentation
 * 08/14/20  EL  Initial Commit
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "LED_module.h"

/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * Global Data
 ******************************************************************************/

/******************************************************************************
 * Module Static Data
 ******************************************************************************/
static uint8_t led_state[5] = {0, 0, 0, 0, 0};
static uint8_t led_buf;
/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Function Definitions
 ******************************************************************************/
/**
 * LED initializer
 * 
 * All the LEDs should be turned off when initialized.
 * 
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
 * The LED values can range from 0 to 4. For most LEDs,
 * 0 is blink 5Hz, 1 is on, 2 is off, and 3 is blink 1Hz. The LED values are categorized
 * through the enum LED_STATE_e.
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
 * LEDcontrol sets the LED buffer to 1 of 4 different states based on the value received from LEDsetState.
 * The LED values are categorized through the enum LED_STATE_e.
 *
 * @return     returns 0
 */
int LEDcontrol()
{
    static int count[LED_MAPPING_END] = {0, 0, 0, 0, 0};
    for(int i = 0; i < LED_MAPPING_END; i++)
    {
        uint8_t value = led_state[i];
        if(value == LED_ON)
        {
            led_buf |= (1 << i); // turns LED on
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
        }
        else if(value == LED_5HZ)
        {
            if(count[i] % 2) // if the LED at i is 1
            {
                led_buf &= ~(1 << i); // turn that LED off
                count[i] = 0;
            }
            else // if the LED at i is 0
            {
                led_buf |= (1 << i); // turn that LED on
                count[i] = 1;
            }
        }
        else // LED_OFF
        {
            led_buf &= ~(1 << i); // turns LED off
            count[i] = 0;
        }
    }

    return 0;
}