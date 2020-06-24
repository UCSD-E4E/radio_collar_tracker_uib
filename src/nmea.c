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

/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * Global Data
 ******************************************************************************/

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
NMEA_GGA_t GGA;

void fixTime(char token[])
{
    if(token[0] == '\0')
    {
        GGA.fixHour = 0xFF;
        GGA.fixMinute = 0xFF;
        GGA.fixSecond = 0xFF;
    }
    else
    {
        GGA.fixSecond = atoi(&token[4]);
        token[4] = '\0';
        GGA.fixMinute = atoi(&token[2]);
        token[2] = '\0';
        GGA.fixHour = atoi(&token[0]);
    }
}

void findLat(char token[])
{
    int temp_int;
    float temp_frac;
    if(token[0] == '\0')
    {
        GGA.latitude = 0;
    }
    else
    {
        temp_frac = atof(&token[2]);
        token[2] = '\0';
        temp_int = atoi(&token[0]);
        GGA.latitude = temp_int + temp_frac/60;
    }
}

void findLong(char token[])
{
    int temp_int;
    float temp_frac;
    if(token[0] == '\0')
    {
        GGA.longitude = 0;
    }
    else
    {
        temp_frac = atof(&token[3]);
        token[3] = '\0';
        temp_int = atoi(&token[0]);
        GGA.longitude = temp_int + temp_frac/60;
        printf("%f\n", temp_frac);
        printf("%d", temp_int);
    }
}

void fixQuality(char token[])
{
    if(token[0] == '\0')
    {
        GGA.fixQuality = 0;
    }
    else
    {
        GGA.fixQuality = atoi(token);
    }
}

void nSatellites(char token[])
{
    if(token[0] == '\0')
    {
        GGA.nSatellites = 0;
    }
    else
    {
        GGA.nSatellites = atoi(token);
    }
}

void hdop(char token[])
{
    if(token[0] == '\0')
    {
        GGA.hdop = 0;
    }
    else
    {
        GGA.hdop = atof(token);
    }
}

void altitude(char token[])
{
    if(token[0] == '\0')
    {
        GGA.altitude = 0;
    }
    else
    {
        GGA.altitude = atof(token);
    }
}

void elevation(char token[])
{
    if(token[0] == '\0')
    {
        GGA.elevation = 0;
    }
    else
    {
        GGA.elevation = atof(token);
    }
}

void dGpsStale(char token[])
{
    if(token[0] == '\0')
    {
        GGA.dGpsStale = 0xFF;
    }
    else
    {
        GGA.dGpsStale = atoi(token);
    }
}

void dGpsID(char token[])
{
    if(token[0] == '\0')
    {
        GGA.dGpsID = 0xFF;
    }
    else
    {
        GGA.dGpsID = atoi(token);
    }
}

void dtoh(int quotient, char *hexadecimal)
{
    int remainder;
    int j = 0;
    char temp;

    while (quotient != 0)
    {
        remainder = quotient % 16;
        if (remainder < 10)
        {
            hexadecimal[j++] = 48 + remainder;
        }
        else
        {
            hexadecimal[j++] = 55 + remainder;
        }
        quotient = quotient / 16;
    }
    temp = hexadecimal[0];
    hexadecimal[0] = hexadecimal[1];
    hexadecimal[1] = temp;
}

NMEA_Message_e NMEA_Decode(char c)
{
    static state_e decodeState = START;
    static NMEA_Message_e message = NMEA_MESSAGE_NONE;
    static char checksum = 0;
    static int counter = 0;
    static int char_count = 0;
    static char sum[3] = {0, 0, 0};
    static char *ptr1;
    static char *ptr2;
    static char next_expected_char = 0;
    static int token_num = 0;
    static int i = 0;
    static char str[256];

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
            switch(message)
            {
                case NMEA_MESSAGE_GGA:
                    switch(token_num)
                    {
                        case 0:
                            GGA.talkerID = talkerID;
                            token_num = 1;
                            break;
                        case 1:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                fixTime(str);
                                token_num = 2;
                            }
                            break;
                        case 2:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else 
                            {
                                str[i] = '\0';
                                i = 0;
                                findLat(str);
                                token_num = 3;
                            }
                            break;
                        case 3:
                            if(c != ',')
                            {
                                if(c == 'S')
                                {
                                    GGA.latitude = -1 * GGA.latitude;
                                }
                            }
                            else
                            {
                                token_num = 4;
                            }
                            break;
                        case 4:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                findLong(str);
                                token_num = 5;
                            }
                            break;
                        case 5:
                            if(c != ',')
                            {
                                if(c == 'W')
                                {
                                    GGA.longitude = -1 * GGA.longitude;
                                }
                            }
                            else
                            {
                                token_num = 6;
                            }
                            break;
                        case 6:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                fixQuality(str);
                                token_num = 7;
                            }
                            break;
                        case 7:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                nSatellites(str);
                                token_num = 8;
                            }
                            break;
                        case 8:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                hdop(str);
                                token_num = 9;
                            }
                            break;
                        case 9:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                altitude(str);
                                token_num = 10;
                            }
                            break;
                        case 10:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                token_num = 11;
                            }
                            break;
                        case 11:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                elevation(str);
                                token_num = 12;
                            }
                            break;
                        case 12:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                token_num = 13;
                            }
                            break;
                        case 13:
                            if(c != ',')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                dGpsStale(str);
                                token_num = 14;
                            }
                            break;
                        case 14:
                            if(c != ',' && c != '*')
                            {
                                str[i++] = c;
                            }
                            else
                            {
                                str[i] = '\0';
                                i = 0;
                                dGpsID(str);
                                token_num = 0;
                                decodeState = CHECKSUM;
                            }
                            break;
                    }
                case NMEA_MESSAGE_GLL:
                    break;
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
                char hexadecimal[3];
                memset(hexadecimal, 0, sizeof(hexadecimal));
                sum[char_count] = '\0';
                dtoh(checksum, hexadecimal);
                if(!strcmp(hexadecimal, sum))
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