/**
 * @file sensor_module.h
 *
 * @author Eugene Lee
 * 
 * @description
 * Radio Telemetry Tracker sensor packet parser. Takes in a packet char by char and parses it.
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
 * 08/16/20  EL  Fixed voltage/compass
 * 08/14/20  EL  Initial Commit
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "compass_sim.h"
#include "nmea.h"
#include "voltage_sim.h"
#include "sensor_encoder.h"
#include "sensor_module.h"
#include <time.h>

/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * Global Data
 ******************************************************************************/
DataSensorPacket_t sensor_packet;
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
 * Reads the compass value in the file "dataStore.bin"
 *
 * @return     returns the compass value
 */
int16_t readCompass(void)
{
    int16_t compassValue;

    Compass_Read(&compassValue);
    return compassValue;
}

/**
 * Reads the voltage value in the file "voltageSim.bin"
 *
 * @return     returns the voltage value
 */
int16_t readVoltage(void)
{
    uint16_t voltageValue;

    Voltage_Read(&voltageValue);
    return voltageValue;
}

/**
 * Changes the time from year, month, day, hours, minutes, seconds, to time since Unix Epoch
 *
 * @param      year   The year
 * @param      month  The month
 * @param      day    The day
 * @param      hms    The hours, minutes, and seconds
 *
 * @return     returns the time since Unix Epoch
 */
uint64_t changeTime(int *year, uint8_t *month, uint8_t *day, uint8_t hms[3])
{
    struct tm t;
    time_t t_of_day;

    t.tm_year = *year - 1900;      // Year - 1900
    t.tm_mon = *month - 1;           // Month, where 0 = january
    t.tm_mday = *day;            // Day of the month
    t.tm_hour = hms[0];
    t.tm_min = hms[1];
    t.tm_sec = hms[2];
    t.tm_isdst = -1;            // Is DST on? 1 = yes, 0 = no, -1 = unknown
    t_of_day = mktime(&t);

    return t_of_day;
}

/**
 * Sets the constant portion of the DataSensorPacket struct
 *
 * @param      packet  The packet
 */
void setPacket(DataSensorPacket_t *packet)
{
    packet->sync_char1 = 0xE4;
    packet->sync_char2 = 0xEB;
    packet->packet_class = 0x04;
    packet->packet_id = 0x03;
    packet->payload.version = 0x01;
    packet->payload_length = sizeof(DataSensorInput_t);
}

/**
 * Sets the sensor_packet to contain the right encoded data from the GPS port
 *
 * @param[in]  c     The data from the GPS port, char by char
 *
 * @return     returns PARSE_COMPLETE if successful, PARSE_NOT_COMPLETE if not
 */
int sensorParse(char c)
{
    static int ymd_ready = 0;       // year month day ready
    static int location_rdy = 0;    // location ready
    static int alt_rdy = 0;         // altitude ready
    static int type_set = 0;
    static int tmp_year;
    static uint8_t tmp_month;
    static uint8_t tmp_day;
    static int ZDA_ymd_set = 0;

    switch(NMEA_Decode(c))
    {
        case NMEA_MESSAGE_GGA:
            {
                setPacket(&sensor_packet);

                sensor_packet.payload.fixType = NMEA_Data.GGA.fixQuality;
                type_set = 1;

                if(NMEA_Data.GGA.fixQuality != 0)
                {
                    sensor_packet.payload.latitude = (int32_t) (NMEA_Data.GGA.latitude * 1e7);
                    sensor_packet.payload.longitude = (int32_t) (NMEA_Data.GGA.longitude * 1e7);
                    sensor_packet.payload.altitude = (uint16_t) (NMEA_Data.GGA.altitude * 10);
                    location_rdy = 1;
                    alt_rdy = 1;

                    if(ZDA_ymd_set == 1)
                    {
                        sensor_packet.payload.time = changeTime(&tmp_year, &tmp_month, &tmp_day, NMEA_Data.GGA.fixTime);
                        ymd_ready = 1;
                    }
                }
            }
            break;
        case NMEA_MESSAGE_GLL:
            {
                setPacket(&sensor_packet);

                if(NMEA_Data.GLL.fixType != 'V')
                {
                    sensor_packet.payload.latitude = (int32_t) (NMEA_Data.GLL.latitude * 1e7);
                    sensor_packet.payload.longitude = (int32_t) (NMEA_Data.GLL.longitude * 1e7);
                    location_rdy = 1;

                    if(ZDA_ymd_set == 1)
                    {
                        sensor_packet.payload.time = changeTime(&tmp_year, &tmp_month, &tmp_day, NMEA_Data.GLL.fixTime);
                        ymd_ready = 1;
                    }
                }
                else
                {
                    sensor_packet.payload.fixType = 0;
                    type_set = 1;
                }
            }
            break;
        case NMEA_MESSAGE_ZDA:
            {
                setPacket(&sensor_packet);

                if(NMEA_Data.ZDA.fixTime[0] != 0xFF && NMEA_Data.ZDA.fixTime[1] != 0xFF && NMEA_Data.ZDA.fixTime[2] != 0xFF) //ZDA has no fixType like input
                {
                    sensor_packet.payload.time = changeTime(&NMEA_Data.ZDA.year, &NMEA_Data.ZDA.month, &NMEA_Data.ZDA.day, NMEA_Data.ZDA.fixTime);
                    tmp_year = NMEA_Data.ZDA.year;
                    tmp_month = NMEA_Data.ZDA.month;
                    tmp_day = NMEA_Data.ZDA.day;
                    ZDA_ymd_set = 1;
                    ymd_ready = 1;
                }
                else
                {
                    sensor_packet.payload.fixType = 0;
                    type_set = 1;
                }
            }
            break;
        default:
            break;
    }
    sensor_packet.payload.heading = readCompass(); //the number in dataStore.bin (pValue);
    sensor_packet.payload.voltage = readVoltage(); //the number in voltageSim.bin (pValue);
    if(ymd_ready == 1 && location_rdy == 1 && alt_rdy == 1 && type_set == 1)
    {
        uint8_t buffer[256];
        encodeSensorPacket(&sensor_packet, buffer, sizeof(buffer));
        ymd_ready = 0;
        alt_rdy = 0;
        location_rdy = 0;
        type_set = 0;
        return PARSE_COMPLETE;
    }
    return PARSE_NOT_COMPLETE;
}