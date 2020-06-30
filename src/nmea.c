/**
 * @file nmea.h
 *
 * @author Nathan Hui, nthui@eng.ucsd.edu
 * 
 * @description 
 * Radio Telemetry Tracker NMEA Module
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
 *  DATE      WHO DESCRIPTION
 *  ----------------------------------------------------------------------------
 *  06/12/20  NH  Initial commit
 */
 /* DATE      WHO DESCRIPTION
 *  ----------------------------------------------------------------------------
 *  06/29/20  EL  Added Documentation
 */
/******************************************************************************
 * Includes
 ******************************************************************************/
#include "nmea.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/******************************************************************************
 * Defines
 ******************************************************************************/
#define STRLENGTH 256
#define BASE 16
/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * Global Data
 ******************************************************************************/
NMEA_Data_u NMEA_Data;
/******************************************************************************
 * Module Static Data
 ******************************************************************************/
char dataID;
char talkerID;
static const NMEA_Function_Ptr_t GGA_table[];
static const NMEA_Function_Ptr_t GLL_table[];
/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/
static void setID(char token[], void *ID);
static void fixTime(char token[], void *time);
static void findLat(char token[], void *lat);
static void latDir(char token[], void *lat);
static void findLong(char token[], void *longitude);
static void longDir(char token[], void *longitude);
static void fixQuality(char token[], void *quality);
static void nSatellites(char token[], void *satellite);
static void hdop(char token[], void *hdop);
static void altitude(char token[], void *alt);
static void elevation(char token[], void *elevation);
static void dGpsStale(char token[], void *stale);
static void dGpsID(char token[], void *ID);
static void fixType(char token[], void *type);
/******************************************************************************
 * Function Definitions
 ******************************************************************************/
/**
 * Initialization of variable state
 */

/**
 * GGA_table[] is a table in the format {function, function input, next field}
 * including all the functions necessary to parse a GGA string.
 */
static const NMEA_Function_Ptr_t GGA_table[] =
{
    {setID, &NMEA_Data.GGA.talkerID, FIELD_GGA_TIME},
    {fixTime, &NMEA_Data.GGA.fixTime, FIELD_GGA_LAT},
    {findLat, &NMEA_Data.GGA.latitude, FIELD_GGA_LAT_DIR},
    {latDir, &NMEA_Data.GGA.latitude, FIELD_GGA_LONGITUDE},
    {findLong, &NMEA_Data.GGA.longitude, FIELD_GGA_LONGITUDE_DIR},
    {longDir, &NMEA_Data.GGA.longitude, FIELD_GGA_QUALITY},
    {fixQuality, &NMEA_Data.GGA.fixQuality, FIELD_GGA_SATELLITES},
    {nSatellites, &NMEA_Data.GGA.nSatellites, FIELD_GGA_HDOP},
    {hdop, &NMEA_Data.GGA.hdop, FIELD_GGA_ALTITUDE},
    {altitude, &NMEA_Data.GGA.altitude, FIELD_GGA_ALTITUDE_UNIT},
    {NULL, 0, FIELD_GGA_ELEVATION},
    {elevation, &NMEA_Data.GGA.elevation, FIELD_GGA_ELEVATION_UNIT},
    {NULL, 0, FIELD_GGA_DGPSSTALE},
    {dGpsStale, &NMEA_Data.GGA.dGpsStale, FIELD_GGA_DGPSID},
    {dGpsID, &NMEA_Data.GGA.dGpsID, FIELD_GGA_END}
};

/**
 * GLL_table[] is a table in the format {function, function input, next field}
 * including all the functions necessary to parse a GLL string.
 */
static const NMEA_Function_Ptr_t GLL_table[] =
{
    {setID, &NMEA_Data.GLL.talkerID, FIELD_GLL_LAT},
    {findLat, &NMEA_Data.GLL.latitude, FIELD_GLL_LAT_DIR},
    {latDir, &NMEA_Data.GLL.latitude, FIELD_GLL_LONGITUDE},
    {findLong, &NMEA_Data.GLL.longitude, FIELD_GLL_LONGITUDE_DIR},
    {longDir, &NMEA_Data.GLL.longitude, FIELD_GLL_TIME},
    {fixTime, &NMEA_Data.GLL.fixTime, FIELD_GLL_FIXTYPE},
    {fixType, &NMEA_Data.GLL.fixType, FIELD_GLL_END}
};

void NMEA_Init(NMEA_Config_t* pConfig)
{

}

/**
 * Sets the ID.
 * 
 * setID takes the talkerID from the TALKER_ID switch state in NMEA_Decode
 * @param      token  Does nothing (only included for identical inputs of functions)
 * @param      ID     Character to set as talkerID
 */
static void setID(char token[], void *ID)
{
    *(char *)ID = talkerID;
}

/**
 * Sets the Time
 * 
 * fixTime takes the token with the three time values: hour, minute, second
 * and sets an array to hour, minute, and second respectively.
 * @param      token  The token containing the time values that will be parsed
 * @param      time   The uint8 array pointer
 */
static void fixTime(char token[], void *time)
{
    uint8_t *time_arr = time;

    if(token[0] == '\0')
    {
        time_arr[0] = 0xFF;
        time_arr[1] = 0xFF;
        time_arr[2] = 0xFF;
    }
    else
    {
        time_arr[2] = atoi(&token[4]);
        token[4] = '\0';
        time_arr[1] = atoi(&token[2]);
        token[2] = '\0';
        time_arr[0] = atoi(&token[0]);
    }
}

/**
 * Calculates Latitude and parses it
 * 
 * findLat takes the token with the latitude in degrees, minute form and
 * calculates the latitude solely in degrees then sets the value to the
 * Latitude variable for the message type.
 * @param      token  The token with the latitude which will be parsed
 * @param      lat    The latitude variable to be set
 */
static void findLat(char token[], void *lat)
{
    int temp_int;
    float temp_frac;
    if(token[0] == '\0')
    {
        *(float *)lat = 0;
    }
    else
    {
        temp_frac = atof(&token[2]);
        token[2] = '\0';
        temp_int = atoi(&token[0]);
        *(float *)lat = temp_int + temp_frac/60;
    }
}

/**
 * Puts a sign on the latitude
 *
 * latDir changes the latitude to be negative iff the token received is S
 * @param      token  The token with the direction of latitude (N or S)
 * @param      lat    The latitude variable to be edited
 */
static void latDir(char token[], void *lat)
{
    if(token[0] == 'S')
    {
        *(float *)lat = -1 * (*(float *)lat);
    }
}

/**
 * Calculates Longitude and parses it
 * 
 * findLong takes the token with the longitude in degrees, minute form and
 * calculates the longitude solely in degrees then sets the value to the
 * Longitude variable for the message type.
 * @param      token        The token with the longitude which will be parsed
 * @param      longitude    The longitude variable to be set
 */
static void findLong(char token[], void *longitude)
{
    int temp_int;
    float temp_frac;
    if(token[0] == '\0')
    {
        *(float *)longitude = 0;
    }
    else
    {
        temp_frac = atof(&token[3]);
        token[3] = '\0';
        temp_int = atoi(&token[0]);
        *(float *)longitude = temp_int + temp_frac/60;
    }
}

/**
 * Puts a sign on the longitude
 *
 * longDir changes the longitude to be negative iff the token received is S
 * @param      token        The token with the direction of longitude (N or S)
 * @param      longitude    The longitude variable to be edited
 */
static void longDir(char token[], void *longitude)
{
    if(token[0] == 'W')
    {
        *(float *)longitude = -1 * (*(float *)longitude);
    }
}

/**
 * Parses the quality
 *
 * fixQuality takes the quality from the token and sets it as a uint8 value in the respective message type
 * @param      token    The token which contains the quality and will be parsed
 * @param      quality  The variable to be set
 */
static void fixQuality(char token[], void *quality)
{
    if(token[0] == '\0')
    {
        *(uint8_t *)quality = 0;
    }
    else
    {
        *(uint8_t *)quality = atoi(token);
    }
}

/**
 * Parses the nSatellites
 *
 * nSatellites takes the number of satellites given by the token and sets the token value
 * into a uint8 value through the use of atoi.
 * @param      token      The token which contains the number of satellites
 * @param      satellite  The variable to be set
 */
static void nSatellites(char token[], void *satellite)
{
    if(token[0] == '\0')
    {
        *(uint8_t *)satellite = 0;
    }
    else
    {
        *(uint8_t *)satellite = atoi(token);
    }
}

/**
 * Parses the Horizontal Dilution of precision
 *
 * hdop takes the token containing the Horizontal Dilution of precision and
 * sets the variable hdop from the respective message type to the token value
 * @param      token  The token containing the Horizontal Dilution of precision
 * @param      hdop   The variable to be set
 */
static void hdop(char token[], void *hdop)
{
    if(token[0] == '\0')
    {
        *(float *)hdop = 0;
    }
    else
    {
        *(float *)hdop = atof(token);
    }
}

/**
 * Parses the altitude
 *
 * altitude takes the number in token and converts it to a float then sets
 * the altitude variable of the corresponding message type.
 * @param      token  The token with the altitude value
 * @param      alt    The variable to be set
 */
static void altitude(char token[], void *alt)
{
    if(token[0] == '\0')
    {
        *(float *)alt = 0;
    }
    else
    {
        *(float *)alt = atof(token);
    }
}

/**
 * Parses the elevation
 *
 * elevation takes the number in token and converts it to a float then sets
 * the elevation variable of the corresponding message type.
 * @param      token        The token with the elevation value
 * @param      elevation    The variable to be set
 */
static void elevation(char token[], void *elevation)
{
    if(token[0] == '\0')
    {
        *(float *)elevation = 0;
    }
    else
    {
        *(float *)elevation = atof(token);
    }
}

/**
 * Parses the dGpsStale value
 *
 * dGpsStale takes the token and sets it to the stale value
 * @param      token  The token containing dGpsStale
 * @param      stale  The variable to be set
 */
static void dGpsStale(char token[], void *stale)
{
    if(token[0] == '\0')
    {
        *(uint8_t *)stale = 0xFF;
    }
    else
    {
        *(uint8_t *)stale = atoi(token);
    }
}

/**
 * Parses the dGpsID value
 *
 * dGpsID takes the token and sets it to the ID value
 * @param      token  The token containing dGpsID
 * @param      ID     The variable to be set
 */
static void dGpsID(char token[], void *ID)
{
    if(token[0] == '\0')
    {
        *(uint8_t *)ID = 0xFF;
    }
    else
    {
        *(uint8_t *)ID = atoi(token);
    }
}

/**
 * Parses the type value
 *
 * fixType takes the character from token and sets the
 * char to type.
 * @param      token  The token containing type
 * @param      type   The variable to be set
 */
static void fixType(char token[], void *type)
{
    if(token[0] == '\0')
    {
        *(char *)type = 0xFF;
    }
    else
    {
        *(char *)type = token[0];
    }
}

/**
 * Parses one character from an NMEA stream.
 * 
 * NMEA_Decode takes in an NMEA stream one character at a time. When
 * NMEA_Decode has received the entirety of a NMEA sentence, it will return the
 * appropriate NMEA_Message_e to let the calling function know that it has
 * successfully decoded a message. The decoded data is available in NMEA_Data
 * in the respective type.
 * @param  c Character to parse
 * @return   NMEA_Message_e denoting message detected
 */
NMEA_Message_e NMEA_Decode(char c)
{
    static state_e decodeState = START;
    static NMEA_Message_e message = NMEA_MESSAGE_NONE;
    static char checksum = 0;
    long int checksum_hex;
    static int counter = 0;
    static int char_count = 0;
    static char sum[3] = {0, 0, 0};
    static char *ptr1;
    static char *ptr2;
    static char next_expected_char = 0;
    static int i = 0;
    static char str[STRLENGTH];
    static int next_field = FIELD_TALKER_ID;

    switch(decodeState)
    {
        case START:
            if(c == '$')
            {
                decodeState = DATA_ID;
            }
            break;
        case DATA_ID:
            dataID = c;
            decodeState = TALKER_ID;
            checksum ^= c;
            break;
        case TALKER_ID:
            talkerID = c;
            decodeState = MESSAGE_TYPE;
            counter = 0;
            checksum ^= c;
            break;
        case MESSAGE_TYPE:
            switch(counter)
            {
                case 0:
                    ptr1 = strchr("GRZV", c);
                    counter = 1;
                    break;
                case 1:
                    switch(*ptr1)
                    {
                        case 'G':
                            counter = 2;
                            ptr2 = strchr("GLS", c);
                            break;
                        case 'R':
                            counter = 2;
                            if(c == 'M')
                            {
                                next_expected_char = 'C';
                            }
                            break;
                        case 'Z':
                            counter = 2;
                            if(c == 'D')
                            {
                                next_expected_char = 'A';
                            }
                            break;
                        case 'V':
                            counter = 2;
                            if(c == 'T')
                            {
                                next_expected_char = 'G';
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case 2:
                    if(next_expected_char == 0)
                    {
                        if(*ptr2 == 'G' && c == 'A')
                        {
                            message = NMEA_MESSAGE_GGA;
                        }
                        else if(*ptr2 == 'L' && c == 'L')
                        {
                            message = NMEA_MESSAGE_GLL;
                        }
                        else if(*ptr2 == 'S' && c == 'V')
                        {
                            message = NMEA_MESSAGE_GSV;
                        }
                    }
                    else if(next_expected_char == c)
                    {
                        if(c == 'C')
                        {
                            message = NMEA_MESSAGE_RMC;
                        }
                        else if(c == 'A')
                        {
                            message = NMEA_MESSAGE_ZDA;
                        }
                        else if(c == 'G')
                        {
                            message = NMEA_MESSAGE_VTG;
                        }
                    }
                    decodeState = FIELDS;
                    break;
            }
            checksum ^= c;
            break;
        case FIELDS:
            if(c != ',' && c != '*' && i < STRLENGTH)
            {
                str[i++] = c;
            }
            else
            {
                str[i] = '\0';
                i = 0;

                switch(message)
                {
                    case NMEA_MESSAGE_GGA:
                        if(GGA_table[next_field].field_func_ptr != NULL)
                        {
                            (GGA_table[next_field].field_func_ptr)(str, GGA_table[next_field].arg0);                       
                        }
                        next_field = GGA_table[next_field].next_field;
                        if(next_field == FIELD_GGA_END)
                        {
                            decodeState = CHECKSUM;
                            next_field = FIELD_TALKER_ID;
                        }
                        break;
                    case NMEA_MESSAGE_GLL:                   
                        if(GLL_table[next_field].field_func_ptr != NULL)
                        {
                            (GLL_table[next_field].field_func_ptr)(str, GLL_table[next_field].arg0);                   
                        }
                        next_field = GLL_table[next_field].next_field;
                        if(next_field == FIELD_GLL_END)
                        {
                            decodeState = CHECKSUM;
                            next_field = FIELD_TALKER_ID;
                        }
                        break;
                    default:
                        break;
                }
            }
            if(c != '*')
            {
                checksum ^= c;
            }
            break;
        case CHECKSUM:
            if(c != '*')
            {
                sum[char_count++] = c;
            }
            if(char_count == 2)
            {
                sum[char_count] = '\0';
                checksum_hex = strtol(sum, NULL, BASE);
                if(checksum == checksum_hex)
                {
                    decodeState = START;
                    return message;
                }
                else
                {
                    return NMEA_MESSAGE_ERROR;
                }
            }
            break;
        default:
            decodeState = START;
            break;
    }
    return NMEA_MESSAGE_NONE;
}