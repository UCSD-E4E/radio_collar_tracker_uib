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
 * 08/16/20  EL  Fixed documentation
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
 * State of decoder.
 */
typedef enum state_
{
    START,
    DATA_ID,
    TALKER_ID,
    MESSAGE_TYPE,
    FIELDS,
    CHECKSUM
} state_e;

/**
 * list of fields for GGA
 */
typedef enum GGA_field_type_
{
    FIELD_TALKER_ID = 0,
    FIELD_GGA_TIME,
    FIELD_GGA_LAT,
    FIELD_GGA_LAT_DIR,
    FIELD_GGA_LONGITUDE,
    FIELD_GGA_LONGITUDE_DIR,
    FIELD_GGA_QUALITY,
    FIELD_GGA_SATELLITES,
    FIELD_GGA_HDOP,
    FIELD_GGA_ALTITUDE,
    FIELD_GGA_ALTITUDE_UNIT,
    FIELD_GGA_ELEVATION,
    FIELD_GGA_ELEVATION_UNIT,
    FIELD_GGA_DGPSSTALE,
    FIELD_GGA_DGPSID,
    FIELD_GGA_END
} GGA_field_type_e;

/**
 * list of fields for GLL
 */
typedef enum GLL_field_type_
{
    FIELD_GLL_LAT = 1,
    FIELD_GLL_LAT_DIR,
    FIELD_GLL_LONGITUDE,
    FIELD_GLL_LONGITUDE_DIR,
    FIELD_GLL_TIME,
    FIELD_GLL_FIXTYPE,
    FIELD_GLL_END
} GLL_field_type_e;

/**
 * list of fields for ZDA
 */
typedef enum ZDA_field_type_
{
    FIELD_ZDA_TIME = 1,
    FIELD_ZDA_DAY,
    FIELD_ZDA_MONTH,
    FIELD_ZDA_YEAR,
    FIELD_ZDA_ZHOURS,
    FIELD_ZDA_ZMINUTES,
    FIELD_ZDA_END
} ZDA_field_type_e;
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
    uint8_t fixTime[3];     /*!< Fix Time.  For the above example, 12 hours,
                                 35 minutes, 19 seconds */
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
    uint8_t fixTime[3];     /*!< Fix Time.  For the above example, 22 hours,
                                 54 minutes, 44 seconds */
    char fixType;           //!< Fix Mode.  For the above example, A
}NMEA_GLL_t;

/**
 * NMEA ZDA Structure.  Integer fields not provided should be set to -1 (0xFF).
 * Floating point fields not provided should be set to NaN.  Character fields
 * not provided should be set to '\0'.
 *
 * Example: $GPZDA,201530.00,04,07,2002,00,00*60
 */
typedef struct NMEA_ZDA_
{
    char talkerID;          //!< Talker ID. For the above example, 'P'
    uint8_t fixTime[3];     /*!< Fix Time.  For the above example, 20 hours,
                                 15 minutes, 30 seconds */
    uint8_t day;            //!< Day. For the above example, 4
    uint8_t month;          //!< Month. For the above example, 7
    int year;           //!< Year. For the above example, 2002
    uint8_t zoneHours;      //!< Local Zone Hours. For the above example, 0xFF
    uint8_t zoneMinutes;    //!< Local Zone Minutes. For the above example, 0xFF
}NMEA_ZDA_t;
/**
 * Describes the table structure for the tables in nmea.c
 */
typedef struct NMEA_Function_Ptr_
{
    void (*field_func_ptr)(char *, void *);
    void *arg0;
    int next_field;
} NMEA_Function_Ptr_t;

/**
 * Puts the different types of GPS messages into a union
 */
typedef union NMEA_Data_{
    NMEA_GGA_t GGA;
    NMEA_GLL_t GLL;
    NMEA_ZDA_t ZDA;
}NMEA_Data_u;

/**
 * Creates the union
 */
extern NMEA_Data_u NMEA_Data;

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


#endif /* __NMEA_H__ */