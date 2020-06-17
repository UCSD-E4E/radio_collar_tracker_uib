/**
 * @file nmea.h
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * Radio Telemetry Tracker NMEA Module Header
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
 * 06/12/20  NH  Initial commit
 */
#ifndef __NMEA_H__
#define __NMEA_H__
/******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
/******************************************************************************
 * Defines
 ******************************************************************************/
/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * NMEA Message Bitmasks
 */
typedef enum NMEA_Message_
{
    NMEA_MESSAGE_GGA = 0x00000001,  //!< GGA Message Bitmask
    NMEA_MESSAGE_GLL = 0x00000002,  //!< GLL Message Bitmask
    NMEA_MESSAGE_GSV = 0x00000004,  //!< GSV Message Bitmask
    NMEA_MESSAGE_RMC = 0x00000008,  //!< RMC Message Bitmask
    NMEA_MESSAGE_ZDA = 0x00000010,  //!< ZDA Message Bitmask
    NMEA_MESSAGE_VTG = 0x00000020,  //!< VTG Message Bitmask
    NMEA_MESSAGE_NONE = 0x00000000, /*!< Returned when message is not complete 
                                         or not enabled */
    NMEA_MESSAGE_ERROR = 0x80000000,/*!< Returned when message is enabled and
                                         corrupted or invalid */
}NMEA_Message_e;

/**
 * NMEA Configuration Structure
 */
typedef struct NMEA_Config_
{
    /**
     * Bitmask of NMEA_Message_e fields denoting the messages that should be
     * accepted by this parser
     */
    uint32_t messageMask;
}NMEA_Config_t;

/**
 * NMEA GGA Structure.  Integer fields not provided should be set to -1 (0xFF).
 * Floating point fields not provided should be set to NaN.  Character fields
 * not provided should be set to '\0'.
 *
 * Example: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
 */
typedef struct NMEA_GGA_
{
    char talkerID;          //!< Talker ID.  For the above example, 'P'
    uint8_t fixHour;        //!< Fix Hour.  For the above example, 12
    uint8_t fixMinute;      //!< Fix Minute.  For the above example, 35
    uint8_t fixSecond;      //!< Fix Second.  For the above example, 19
    float latitude;         //!< Latitude.  For the above example, 48.127222
    float longitude;        //!< Longitude.  For the above example, 11.516667
    uint8_t fixQuality;     //!< Fix Quality.  For the above example, 1
    uint8_t nSatellites;    /*!< Number of satellites in fix.  For the above 
                                 example, 8 */
    float hdop;             /*!< Horizontal Dilution of precision.  For the 
                                 above example, 0.9 */
    float altitude;         /*!< Receiver altitude.  For the above example, 
                                 545.4 meters */
    float elevation;        /*!< Geoid elevation.  For the above example, 46.9 
                                 meters */
    uint8_t dGpsStale;      /*!< Seconds since last DGPS Update.  For the above
                                 example, 0xFF */
    uint8_t dGpsID;         //!< DGPS ID.  For the above example, 0xFF
}NMEA_GGA_t;

/**
 * NMEA GLL Structure.  Integer fields not provided should be set to -1 (0xFF).
 * Floating point fields not provided should be set to NaN.  Character fields
 * not provided should be set to '\0'.
 *
 * Example: $GPGLL,4916.45,N,12311.12,W,225444,A,*1D
 */
typedef struct NMEA_GLL_
{
    char talkerID;          //!< Talker ID.  For the above example, 'P'
    float latitude;         //!< Latitude.  For the above example, 49.279444
    float longitude;        //!< Longitude.  For the above example, 123.186667
    uint8_t fixHour;        //!< Fix Hour.  For the above example, 22
    uint8_t fixMinute;      //!< Fix Minute.  For the above example, 54
    uint8_t fixSecond;      //!< Fix Second.  For the above example, 44
    char fixType;           //!< Fix Mode.  For the above example, A
}NMEA_GLL_t;

typedef union NMEA_Data_{
    NMEA_GGA_t GGA;
    NMEA_GLL_t GLL;
}NMEA_Data_u;

extern NMEA_Data_u NMEA_Data;

typedef struct NMEA_TYPE_
{
    char name[4];
    NMEA_Message_e type;
} NMEA_TYPE_t;
/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/
/**
 * Initialization of variable state
 */
void NMEA_Init(NMEA_Config_t* pConfig);

/**
 * Parses one character from an NMEA stream.
 * @param  c Character to parse
 * @return   NMEA_Message_e denoting message detected
 */
NMEA_Message_e NMEA_Decode(char c);
NMEA_Message_e NMEA_Decode2(char *string);


#endif /* __NMEA_H__ */