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

NMEA_Message_e NMEA_Decode(char c)
{
    static state_e decodeState = START;
    static NMEA_Message_e message = NMEA_MESSAGE_NONE;
    static char checksum = 0;
    static int end_checksum = 0;
    static int counter = 0;
    static int char_count = 0;
    static char sum[3] = {0, 0, 0};
    static char *ptr1;
    static char *ptr2;
    static char next_expected_char = 0;

    if(c != '$')
    {
        if(c == '*')
        {
            end_checksum = 1;
        }
        if(!end_checksum)
        {
            checksum ^= c;
        }
    }

    switch(decodeState)
    {
        case START:
            if(c == '$')
            {
                decodeState = DATA_ID;
            }
            else
            {
                decodeState = START;
            }
            break;
        case DATA_ID:
            dataID = c;
            decodeState = TALKER_ID;
            break;
        case TALKER_ID:
            talkerID = c;
            decodeState = MESSAGE_TYPE;
            counter = 0;
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
            break;
        case FIELDS:
            if(c == '*')
            {
                decodeState = CHECKSUM;
            }
            else
            {
                decodeState = FIELDS;
            }
            break;
        case CHECKSUM:
            sum[char_count++] = c;
            if(char_count == 2)
            {
                int read_checksum;
                /* %x makes read_checksum hexadecimal */
                sscanf(sum, "%x", &read_checksum);
                if(checksum == read_checksum)
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