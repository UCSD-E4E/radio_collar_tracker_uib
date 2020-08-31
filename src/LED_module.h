/**
 * @file LED_module.h
 *
 * @author Eugene Lee
 * 
 * @description 
 * Radio Telemetry Tracker LED module header.
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
 * 08/23/20  NH  Fixed prototypes
 * 08/16/20  EL  Added documentation
 * 08/14/20  EL  Initial commit
 */

#ifndef __LED_MODULE_H__
#define __LED_MODULE_H__
/******************************************************************************
 * Includes
 ******************************************************************************/

/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * LED types
 */
typedef enum LED_MAPPING_
{
    LED_SYSTEM_STATE = 0,
    LED_STORAGE_STATE,
    LED_SDR_STATE,
    LED_GPS_STATE,
    LED_COMBINED_STATE,
    LED_MAPPING_END
} LED_MAPPING_e;

/**
 * LED states
 */
typedef enum LED_STATE_
{
    LED_OFF = 0,
    LED_ON,
    LED_1HZ,
    LED_5HZ
} LED_STATE_e;

/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/
int LED_init();
int LED_setState(LED_MAPPING_e index, LED_STATE_e value);
int LED_control();



#endif /* __LED_MODULE_H__ */
