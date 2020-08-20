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
 * 08/16/20  EL  Added documentation
 * 08/14/20  EL  Initial commit
 */

#ifndef __LED_MODULE_H__
#define __LED_MODULE_H__
/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
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
    SYSTEM_STATE_LED = 0,
    STORAGE_STATE_LED,
    SDR_STATE_LED,
    GPS_STATE_LED,
    COMBINED_STATE_LED,
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

#endif /* __LED_MODULE_H__ */