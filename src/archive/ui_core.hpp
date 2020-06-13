/*
 * @file ui_core.c
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 *
 * @description Radio Telemetry Tracker UI Core
 *
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
 * DATE      Name  Description
 * --------  ----  -----------------------------------------------------------
 * 02/25/20  NH    Added HAL System descriptor
 */
#ifndef __UI_CORE__
#define __UI_CORE__


typedef struct RCT_HAL_System_{
	Stream* RCT_SerialOBC;
	Stream* RCT_SerialGPS;
}RCT_HAL_System_t;

extern RCT_HAL_System_t* pHALSystem;
#endif
