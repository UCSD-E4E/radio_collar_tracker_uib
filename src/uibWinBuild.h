/**
 * @file uibWinBuild.h
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * Radio Telemetry Tracker UI Board FW Windows Build.  This program provides a
 * way to test and simulate the UI Board FW without hardware (SIL).
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
 * 08/23/20  NH  Removed external prototypes
 * 08/13/20  EL  Tied the LED module, encoder, and decoder together
 * 08/09/20  NH  Added boilerplate code
 * 08/07/20  NH  Added serial and hardware initialization
 * 07/02/20  NH  Initial commit
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "status_decoder.h"
#include "sensor_encoder.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/
