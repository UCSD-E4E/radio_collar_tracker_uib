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
 * 02/25/20  NH    Fixed Sensor Module constructor and reformatted, added
 * 					measure vCC
 */
#include "Sensor_Module.hpp"
#include "HMC5983.hpp"
#include <Wire.h>

#define RUN_SWITCH_PIN 10

const char *Sensor_Module::RUN_TRUE = "true";
const char *Sensor_Module::RUN_FALSE = "false";

// #define DEBUG

Sensor_Module::Sensor_Module(GPSState *state_var) :
		state_var(state_var), gps(ALL), offset_timestamp_ms(0), utc_offset_ms(
				0), previous_fix(0) {
	*state_var = GPS_INIT;
	compass_ready = false;
	packet.lat = 181;
	packet.lon = 181;
	packet.hdg = 361;
	for (uint8_t i = 0; i < 10; i++) {
		packet.time[i] = 0;
	}
	for (uint8_t i = 0; i < 7; i++) {
		packet.date[i] = 0;
	}
	packet.run = false;
	packet.fix = GPS_FIX_NONE;
	packet.sat = 0;
	packet.rail = 0;
}

Sensor_Module::~Sensor_Module() {
}

int Sensor_Module::decode(const char c) {
	if (gps.decode(c)) {
#ifdef DEBUG
		Serial.println(gps.sentence());
		Serial.println(gps.term(0));
		#endif
		if (gps.term(0)[2] == 'R' && gps.term(0)[3] == 'M'
				&& gps.term(0)[4] == 'C') {

			// have RMC message
			if (gps.gprmc_status() == 'A') {
				packet.lat = gps.gprmc_latitude();
				packet.lon = gps.gprmc_longitude();
				for (int i = 0; i < 10; i++) {
					if (gps.term(1)[i] != 0) {
						packet.time[i] = gps.term(1)[i];
					} else {
						break;
					}
				}
				for (int i = 0; i < 7; i++) {
					if (gps.term(9)[i] != 0) {
						packet.date[i] = gps.term(9)[i];
					} else {
						break;
					}
				}
				if (compass_ready) {
					packet.hdg = compass.read();
				}
				packet.run = digitalRead(RUN_SWITCH_PIN);
				previous_fix = millis();
				if (packet.fix == GPS_FIX_FIX) {
					return 1;
				}
			}
			return 0;
		}
		if (gps.term(0)[2] == 'G' && gps.term(0)[3] == 'G'
				&& gps.term(0)[4] == 'A') {
			// have GGA message
			switch (gps.term(6)[0]) {
			case '0':	// Invalid, no position available
				packet.fix = GPS_FIX_NONE;
				*state_var = GPS_INIT;
				break;
			default:	// All other types of fixes
				packet.fix = GPS_FIX_FIX;
				*state_var = GPS_READY;
				break;

			}
			packet.lat = gps.term_decimal(2);
			packet.lon = gps.term_decimal(4);
			if (compass_ready) {
				packet.hdg = compass.read();
			}
			packet.sat = gps.term_decimal(7);
			return 0;
		}
		if (gps.term(0)[2] == 'Z' && gps.term(0)[3] == 'D'
				&& gps.term(0)[4] == 'A') {
			// have ZDA message
			utc_offset_ms = gps.term_decimal(1) * 1e3;
			offset_timestamp_ms = millis();
			return 0;
		}
	}
	if (millis() - previous_fix > 5000) {
		*state_var = GPS_INIT;
	}
	return 0;
}

void Sensor_Module::start() {
	// Check if compass device is present first
	Wire.begin();
	Wire.beginTransmission(0x1E);
	if (Wire.endTransmission() == 0) {
		compass.begin(NULL);
		compass.setMeasurementMode(HMC5983_CONTINOUS);
		compass_ready = true;
	} else {
		*state_var = GPS_FAIL;
		compass_ready = false;
		packet.hdg = 361;
	}
	packet.lat = 181;
	packet.lon = 181;
	packet.hdg = 361;
	for (uint8_t i = 0; i < 10; i++) {
		packet.time[i] = 0;
	}
	for (uint8_t i = 0; i < 7; i++) {
		packet.date[i] = 0;
	}
	packet.run = false;
	packet.fix = GPS_FIX_NONE;
	packet.sat = 0;
	packet.rail = 0;
}

int Sensor_Module::getPacket(char *buf, size_t len) {
	return snprintf(buf, len,
			"{\"lat\": %ld, \"lon\": %ld, \"hdg\": %d, \"tme\": \"%s\", "
					"\"run\": \"%s\", \"fix\": %d, \"sat\": %d, \"dat\": \"%s\"}",
			(long) (packet.lat * 1e7), (long) (packet.lon * 1e7), packet.hdg,
			packet.time, ((packet.run) ? RUN_TRUE : RUN_FALSE),
			(int) packet.fix, packet.sat, packet.date);
}

uint16_t Sensor_Module::measureVCC(){
	// Read 1.1V reference against AVcc
	// set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
	ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	ADMUX = _BV(MUX3) | _BV(MUX2);
#else
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

	delay(2); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA, ADSC))
		; // measuring

	uint8_t low = ADCL; // must read ADCL first - it then locks ADCH
	uint8_t high = ADCH; // unlocks both

	long result = (high << 8) | low;

	result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
	return result; // Vcc in millivolts
}
