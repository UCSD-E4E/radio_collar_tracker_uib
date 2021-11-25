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
 * 08/23/20  NH  Fixed LED State types, reorganized includes
 * 08/16/20  EL  Added documentation
 * 08/14/20  EL  Initial Commit
 */

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "product.h"
#include "LED_module.h"

#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>

#include "cutils.h"
#ifdef E4E_RTT_UIBv2
#include <avr/io.h>
#endif

/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/
typedef struct LED_Map_
{
    LED_MAPPING_e LED;
    volatile uint8_t *port;
    volatile uint8_t *dir;
    uint8_t bit;
} LED_Map_t;
/******************************************************************************
 * Global Data
 ******************************************************************************/

/******************************************************************************
 * Module Static Data
 ******************************************************************************/
static LED_STATE_e led_state[5] =
{0, 0, 0, 0, 0};
static int LED_counter[LED_MAPPING_END] =
{0, 0, 0, 0, 0};

#ifdef E4E_RTT_SIM
static uint8_t led_buf;
#endif

#ifdef E4E_RTT_UIBv2
static const LED_Map_t LED_table[] =
{
{LED_SYSTEM_STATE, &PORTD, &DDRD, PORTD4},
{LED_STORAGE_STATE, &PORTD, &DDRD, PORTD6},
{LED_SDR_STATE, &PORTD, &DDRD, PORTD7},
{LED_GPS_STATE, &PORTB, &DDRB, PORTB4},
{LED_COMBINED_STATE, &PORTB, &DDRB, PORTB5},
{LED_MAPPING_END, NULL, NULL, 0}};
#endif
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
int LED_init()
{
#ifdef E4E_RTT_SIM
    led_buf = 0;
#elif defined(E4E_RTT_UIBv2)
    const LED_Map_t *pEntry = LED_table;
    while(pEntry->port)
    {
        SETBIT(pEntry->bit, *pEntry->dir);
        SETBIT(pEntry->bit, *pEntry->port);
        led_state[pEntry->LED] = LED_OFF;
        _delay_ms(200);
        pEntry++;
    }
#endif
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
int LED_setState(LED_MAPPING_e index, LED_STATE_e value)
{
    if(index > LED_MAPPING_END)
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
int LED_control()
{
    int i;
    for(i = 0; i < LED_MAPPING_END; i++)
    {
        switch(led_state[i])
        {
        case LED_ON:
#ifdef E4E_RTT_SIM
            led_buf |= (1 << i); // turns LED on
#elif defined(E4E_RTT_UIBv2)
            SETBIT(LED_table[i].bit, *LED_table[i].port);
#endif
            LED_counter[i] = 0;
            break;
        case LED_1HZ:
            if(LED_counter[i] <= 5) // if the LED at i is 1
            {
#ifdef E4E_RTT_SIM
                led_buf &= ~(1 << i); // turn the LED off
#elif defined(E4E_RTT_UIBv2)
                CLEARBIT(LED_table[i].bit, *LED_table[i].port);
#endif
            }
            else if(LED_counter[i] > 5 && LED_counter[i] <= 10)
            {
#ifdef E4E_RTT_SIM
                led_buf |= (1 << i); // turn the LED on
#elif defined(E4E_RTT_UIBv2)
                SETBIT(LED_table[i].bit, *LED_table[i].port);
#endif
                LED_counter[i] = 0;
            }
            LED_counter[i]++;
            break;
        case LED_5HZ:
            if(LED_counter[i] % 2) // if the LED at i is 1
            {
#ifdef E4E_RTT_SIM
                led_buf &= ~(1 << i); // turn that LED off
#elif defined(E4E_RTT_UIBv2)
                CLEARBIT(LED_table[i].bit, *LED_table[i].port);
#endif
                LED_counter[i] = 0;
            }
            else // if the LED at i is 0
            {
#ifdef E4E_RTT_SIM
                led_buf |= (1 << i); // turn that LED on
#elif defined(E4E_RTT_UIBv2)
                SETBIT(LED_table[i].bit, *LED_table[i].port);
#endif

                LED_counter[i] = 1;
            }
            break;
        default:
        case LED_OFF:
#ifdef E4E_RTT_SIM
            led_buf &= ~(1 << i); // turns LED off
#elif defined(E4E_RTT_UIBv2)
            CLEARBIT(LED_table[i].bit, *LED_table[i].port);
#endif
            LED_counter[i] = 0;
            break;

        }

    }
    return 0;
}
