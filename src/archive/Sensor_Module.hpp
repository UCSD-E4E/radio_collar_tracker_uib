/*
 * @file Sensor_Module.cpp
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
 * 02/25/20  NH    Added measure vCC
 */
#ifndef __SENSOR_MODULE__
#define __SENSOR_MODULE__

#include "nmea.hpp"
#include "Status_Packet.hpp"
#include "HMC5983.hpp"

/**
 * Sensor Interface Module.  This class is responsible for initializing each
 * sensor, aggregating the data, and having it ready to be forwarded to the OBC.
 */
class Sensor_Module{
	private:
		const static char* RUN_TRUE;
		const static char* RUN_FALSE;
		unsigned long previous_fix;
		HMC5983 compass;
		uint32_t utc_offset_ms;
		uint32_t offset_timestamp_ms;
		bool compass_ready;

	protected:
		/**
		 * GPS States.  Fix should represent at least a 3D fix.
		 */
		enum GPSFix{
			GPS_FIX_NONE = 0,	/// Less than 3D fix.
			GPS_FIX_FIX = 1		/// At least 3D fix.
		};
		/**
		 * Struct to store sensor data
		 */
		typedef struct SensorPacket{
			/// Latitude in decimal degrees WRT WGS84
			float lat;
			/// Longitude in decimal degrees WRT WGS84
			float lon;
			/// Heading in decimal degrees WRT magnetic North
			uint16_t hdg;
			/// GPS timestamp
			char time[10];
			/// GPS Date
			char date[7];
			/// Run switch state
			bool run;
			/// GPS Fix state
			GPSFix fix;
			/// Number of GPS satellites used in fix.
			uint8_t sat;
			/// 5V Rail voltage
			uint16_t rail;
		} SensorPacket;

		/**
		 * Pointer to the GPS System state variable.
		 */
		GPSState* state_var;

		/**
		 * NMEA parser object
		 */
		NMEA gps{1};

		/**
		 * Sensor Packet instance.
		 */
		SensorPacket packet;


	public:
		/**
		 * Constructs a new Sensor Module instance.  This should only be called
		 * once in the lifetime of the program, and initializes all of the
		 * underlying sensor variables.
		 *
		 * @param	state_var	Pointer to a GPSState status variable.
		 */
		Sensor_Module(GPSState* state_var);

		/**
		 * Default deconstructor.
		 */
		~Sensor_Module();

		/**
		 * Initializes the sensor hardware.
		 */
		void start();

		/**
		 * Decodes a character of the GPS serial stream
		 * @param  c next character of the GPS serial stream
		 * @return   1 if a full GPS fix has been received, 0 otherwise.
		 */
		int decode(char c);

		/**
		 * Gets the currently available sensor data packet, formatted as a JSON
		 * dictionary.
		 * @param  buf char buffer in which to store the data packet.
		 * @param  len Length of the data packet.
		 * @return     The number of characters that would have been written if
		 *             len had been sufficiently large, not counting the
		 *             terminating null character.
		 */
		int getPacket(char* buf, size_t len);

		/**
		 * Measures the VCC pin.
		 * @return	VCC in mV
		 */
		uint16_t measureVCC();
};

#endif
