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
NMEA_Message_e NMEA_Decode(char c)
{
	static int counter = -1;
	static char *ptr1;
	static char *ptr2;
	static char next_expected_char = 0;
	printf("counter = %d, c = %c\n", counter, c);
	counter++;
	if(!counter)
	{
		if(c == '$')
		{
			return NMEA_MESSAGE_NONE;
		}
		else
		{
			return NMEA_MESSAGE_ERROR;
		}
	}
	else if(counter == 1 || counter == 2)
	{
		return NMEA_MESSAGE_NONE;
	}
	else if(counter == 3)
	{
		ptr1 = strchr("GRZV", c);
        if(ptr1 == NULL)
        {
            return NMEA_MESSAGE_ERROR;
        }
        else
        {
        	return NMEA_MESSAGE_NONE;
        }
    }
    else if(counter == 4)
    {
        switch(*ptr1)
        {
        	case 'G':
                ptr2 = strchr("GLS", c);
        	 	if(ptr2 == NULL)
        		{
        		    return NMEA_MESSAGE_ERROR;
        		}
        		else
        		{
        			return NMEA_MESSAGE_NONE;
        		}
        		break;
        	case 'R':
        	    if(c == 'M')
        	    {
        		    next_expected_char = 'C';
        		    return NMEA_MESSAGE_NONE;
        	    }
        	    else
        	    {
        	    	return NMEA_MESSAGE_ERROR;
        	    }
        		break;
        	case 'Z':
        	    if(c == 'D')
        	    {
        		    next_expected_char = 'A';
        		    return NMEA_MESSAGE_NONE;
        	    }
        	    else
        	    {
        	    	return NMEA_MESSAGE_ERROR;
        	    }
        		break;
        	case 'V':
        	    if(c == 'T')
        	    {
        		    next_expected_char = 'G';
        		    return NMEA_MESSAGE_NONE;
        	    }
        	    else
        	    {
        	    	return NMEA_MESSAGE_ERROR;
        	    }
        		break;
        }
    }
    else if(counter == 5)
    {
    	if(next_expected_char == 0)
    	{
    		if(*ptr2 == 'G' && c == 'A')
    		{
    			return NMEA_MESSAGE_GGA;
    		}
    		else if(*ptr2 == 'L' && c == 'L')
    		{
    			return NMEA_MESSAGE_GLL;
    		}
    		else if(*ptr2 == 'S' && c == 'V')
    		{
    			return NMEA_MESSAGE_GSV;
    		}
    		else
    		{
    			return NMEA_MESSAGE_ERROR;
    		}
    	}
    	else if(next_expected_char == c)
    	{
    		if(c == 'C')
    		{
    			return NMEA_MESSAGE_RMC;
    		}
    		else if(c == 'A')
    		{
    			return NMEA_MESSAGE_ZDA;
    		}
    		else if(c == 'G')
    		{
    			return NMEA_MESSAGE_VTG;
    		}
    	}
    	else
    	{
    		return NMEA_MESSAGE_ERROR;
    	}
    }
    else
    {
    	return NMEA_MESSAGE_ERROR;
    }
	return NMEA_MESSAGE_ERROR;
}

/* Decoder using entire string */
NMEA_TYPE_t nmea_type_table[] = 
{
	{"GGA", NMEA_MESSAGE_GGA},
	{"GLL", NMEA_MESSAGE_GLL},
	{"GSV", NMEA_MESSAGE_GSV},
	{"RMC", NMEA_MESSAGE_RMC},
	{"ZDA", NMEA_MESSAGE_ZDA},
	{"VTG", NMEA_MESSAGE_VTG}
};

NMEA_Message_e NMEA_Decode2(char *string)
{
	char str[256];
    char *token;
    const char delimiter[2] = ",";
    strcpy(str, string);
    token = strtok(str, delimiter);
    for(int i = 0; i < sizeof(nmea_type_table)/sizeof(nmea_type_table[0]); i++)
    {
    	if(!strcmp(&token[3], nmea_type_table[i].name))
    	{
    		return nmea_type_table[i].type;
    	}
    }
    return NMEA_MESSAGE_ERROR;
}