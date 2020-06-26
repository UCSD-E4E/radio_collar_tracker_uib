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
 * DATE      WHO DESCRIPTION
 * ----------------------------------------------------------------------------
 * 06/12/20  NH  Initial commit
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

/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Function Definitions
 ******************************************************************************/
/**
 * Initialization of variable state
 */
void NMEA_Init(NMEA_Config_t* pConfig)
{

}

/**
 * Parses one character from an NMEA stream.
 * @param  c Character to parse
 * @return   NMEA_Message_e denoting message detected
 */
char dataID;
char talkerID;

void setID(char token[], char *ID)
{
    *ID = talkerID;
}

void fixTime(char token[], uint8_t *hour, uint8_t *minute, uint8_t *second)
{
    if(token[0] == '\0')
    {
        *hour = 0xFF;
        *minute = 0xFF;
        *second = 0xFF;
    }
    else
    {
        *second = atoi(&token[4]);
        token[4] = '\0';
        *minute = atoi(&token[2]);
        token[2] = '\0';
        *hour = atoi(&token[0]);
    }
}

void findLat(char token[], float *lat)
{
    int temp_int;
    float temp_frac;
    if(token[0] == '\0')
    {
        *lat = 0;
    }
    else
    {
        temp_frac = atof(&token[2]);
        token[2] = '\0';
        temp_int = atoi(&token[0]);
        *lat = temp_int + temp_frac/60;
    }
}

void latDir(char token[], float *lat)
{
    if(token[0] == 'S')
    {
        *lat = -1 * (*lat);
    }
}

void findLong(char token[], float *longitude)
{
    int temp_int;
    float temp_frac;
    if(token[0] == '\0')
    {
        *longitude = 0;
    }
    else
    {
        temp_frac = atof(&token[3]);
        token[3] = '\0';
        temp_int = atoi(&token[0]);
        *longitude = temp_int + temp_frac/60;
    }
}

void longDir(char token[], float *longitude)
{
    if(token[0] == 'W')
    {
        *longitude = -1 * (*longitude);
    }
}

void fixQuality(char token[], uint8_t *quality)
{
    if(token[0] == '\0')
    {
        *quality = 0;
    }
    else
    {
        *quality = atoi(token);
    }
}

void nSatellites(char token[], uint8_t *satellite)
{
    if(token[0] == '\0')
    {
        *satellite = 0;
    }
    else
    {
        *satellite = atoi(token);
    }
}

void hdop(char token[], float *hdop)
{
    if(token[0] == '\0')
    {
        *hdop = 0;
    }
    else
    {
        *hdop = atof(token);
    }
}

void altitude(char token[], float *alt)
{
    if(token[0] == '\0')
    {
        *alt = 0;
    }
    else
    {
        *alt = atof(token);
    }
}

void elevation(char token[], float *elevation)
{
    if(token[0] == '\0')
    {
        *elevation = 0;
    }
    else
    {
        *elevation = atof(token);
    }
}

void dGpsStale(char token[], uint8_t *stale)
{
    if(token[0] == '\0')
    {
        *stale = 0xFF;
    }
    else
    {
        *stale = atoi(token);
    }
}

void dGpsID(char token[], uint8_t *ID)
{
    if(token[0] == '\0')
    {
        *ID = 0xFF;
    }
    else
    {
        *ID = atoi(token);
    }
}

void fixType(char token[], char *type)
{
    if(token[0] == '\0')
    {
        *type = 0xFF;
    }
    else
    {
        *type = token[0];
    }
}

NMEA_Function_Ptr_t GGA_table[] =
{
    {(base_func)setID, CHAR_ONE, &NMEA_Data.GGA.talkerID, 0, 0, FIELD_GGA_TIME},
    {(base_func)fixTime, UINT8_THREE, &NMEA_Data.GGA.fixHour, &NMEA_Data.GGA.fixMinute, &NMEA_Data.GGA.fixSecond, FIELD_GGA_LAT},
    {(base_func)findLat, FLOAT_ONE, &NMEA_Data.GGA.latitude, 0, 0, FIELD_GGA_LAT_DIR},
    {(base_func)latDir, FLOAT_ONE, &NMEA_Data.GGA.latitude, 0, 0, FIELD_GGA_LONGITUDE},
    {(base_func)findLong, FLOAT_ONE, &NMEA_Data.GGA.longitude, 0, 0, FIELD_GGA_LONGITUDE_DIR},
    {(base_func)longDir, FLOAT_ONE, &NMEA_Data.GGA.longitude, 0, 0, FIELD_GGA_QUALITY},
    {(base_func)fixQuality, UINT8_ONE, &NMEA_Data.GGA.fixQuality, 0, 0, FIELD_GGA_SATELLITES},
    {(base_func)nSatellites, UINT8_ONE, &NMEA_Data.GGA.nSatellites, 0, 0, FIELD_GGA_HDOP},
    {(base_func)hdop, FLOAT_ONE, &NMEA_Data.GGA.hdop, 0, 0, FIELD_GGA_ALTITUDE},
    {(base_func)altitude, FLOAT_ONE, &NMEA_Data.GGA.altitude, 0, 0, FIELD_GGA_ALTITUDE_UNIT},
    {(base_func)NULL, 0, 0, 0, 0, FIELD_GGA_ELEVATION},
    {(base_func)elevation, FLOAT_ONE, &NMEA_Data.GGA.elevation, 0, 0, FIELD_GGA_ELEVATION_UNIT},
    {(base_func)NULL, 0, 0, 0, 0, FIELD_GGA_DGPSSTALE},
    {(base_func)dGpsStale, UINT8_ONE, &NMEA_Data.GGA.dGpsStale, 0, 0, FIELD_GGA_DGPSID},
    {(base_func)dGpsID, UINT8_ONE, &NMEA_Data.GGA.dGpsID, 0, 0, FIELD_GGA_END}
};

NMEA_Function_Ptr_t GLL_table[] =
{
    {(base_func)setID, CHAR_ONE, &NMEA_Data.GLL.talkerID, 0, 0, FIELD_GLL_LAT},
    {(base_func)findLat, FLOAT_ONE, &NMEA_Data.GLL.latitude, 0, 0, FIELD_GLL_LAT_DIR},
    {(base_func)latDir, FLOAT_ONE, &NMEA_Data.GLL.latitude, 0, 0, FIELD_GLL_LONGITUDE},
    {(base_func)findLong, FLOAT_ONE, &NMEA_Data.GLL.longitude, 0, 0, FIELD_GLL_LONGITUDE_DIR},
    {(base_func)longDir, FLOAT_ONE, &NMEA_Data.GLL.longitude, 0, 0, FIELD_GLL_TIME},
    {(base_func)fixTime, UINT8_THREE, &NMEA_Data.GLL.fixHour, &NMEA_Data.GLL.fixMinute, &NMEA_Data.GLL.fixSecond, FIELD_GLL_FIXTYPE},
    {(base_func)fixType, CHAR_ONE, &NMEA_Data.GLL.fixType, 0, 0, FIELD_GLL_END}
};

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
    static GGA_field_type_e next_field = FIELD_TALKER_ID;

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
                            if(GGA_table[next_field].arg_type == CHAR_ONE)
                            {
                                ((char_one_func)(GGA_table[next_field].field_func_ptr))(str, (char *)GGA_table[next_field].arg0);
                            }
                            else if(GGA_table[next_field].arg_type == UINT8_ONE)
                            {
                                ((uint8_one_func)(GGA_table[next_field].field_func_ptr))(str, (uint8_t *)GGA_table[next_field].arg0);
                            }
                            else if(GGA_table[next_field].arg_type == UINT8_THREE)
                            {
                                ((uint8_three_func)(GGA_table[next_field].field_func_ptr))(str, (uint8_t *)GGA_table[next_field].arg0, (uint8_t *) GGA_table[next_field].arg1, (uint8_t *) GGA_table[next_field].arg2);
                            }
                            else if(GGA_table[next_field].arg_type == FLOAT_ONE)
                            {
                                ((float_one_func)(GGA_table[next_field].field_func_ptr))(str, (float *)GGA_table[next_field].arg0);
                            }                           
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
                            if(GLL_table[next_field].arg_type == CHAR_ONE)
                            {
                                ((char_one_func)(GLL_table[next_field].field_func_ptr))(str, (char *)GLL_table[next_field].arg0);
                            }
                            else if(GLL_table[next_field].arg_type == UINT8_ONE)
                            {
                                ((uint8_one_func)(GLL_table[next_field].field_func_ptr))(str, (uint8_t *)GLL_table[next_field].arg0);
                            }
                            else if(GLL_table[next_field].arg_type == UINT8_THREE)
                            {
                                ((uint8_three_func)(GLL_table[next_field].field_func_ptr))(str, (uint8_t *)GLL_table[next_field].arg0, (uint8_t *) GLL_table[next_field].arg1, (uint8_t *) GLL_table[next_field].arg2);
                            }
                            else if(GLL_table[next_field].arg_type == FLOAT_ONE)
                            {
                                ((float_one_func)(GLL_table[next_field].field_func_ptr))(str, (float *)GLL_table[next_field].arg0);
                            }                           
                        }
                        next_field = GLL_table[next_field].next_field;
                        if(next_field == FIELD_GLL_END)
                        {
                            decodeState = CHECKSUM;
                            next_field = FIELD_TALKER_ID;
                        }
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