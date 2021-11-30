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
 *  08/23/20  NH  Added a message id lookup table, moved state enumeration into
 *                  module, moved function pointer type into module, implemented
 *                  message masking, fixed lat/lon parsing, renamed parsing
 *                  functions to be generic, removed nextField field from
 *                  tables and made message definitions null terminated, fixed
 *                  field parsing to be modular.
 *  08/16/20  EL  Merged similar functions
 *  06/30/20  EL  Added ZDA Parser
 *  06/29/20  EL  Added Documentation
 *  06/12/20  NH  Initial commit
 */
/******************************************************************************
 * Includes
 ******************************************************************************/
#include "nmea.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include "uib.h"
/******************************************************************************
 * Defines
 ******************************************************************************/
#define STRLENGTH 256
#define BASE 16
/******************************************************************************
 * Typedefs
 ******************************************************************************/
/**
 * Message Table lookup
 */
typedef struct messageIDEntry_
{
	NMEA_Message_e msg;	//!< Message enumeration
	char* str;	//!< 3 character string to match.  Must be null terminated
}messageIDEntry_t;

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
    CHECKSUM,
    state__NELEMS
} state_e;

typedef struct NMEA_StateStr_
{
    state_e state;
    const char* name;
}NMEA_StateStr_t;

/**
 * Describes the table structure for the tables in nmea.c
 */
typedef struct NMEA_Function_Ptr_
{
    /**
     * Function pointer to the decoding function
     * @param   Pointer to the input token
     * @param   Pointer to the output variable
     */
    void (*field_func_ptr)(char *, void *);
    /**
     * Pointer to the variable to populate
     */
    void *arg0;
} NMEA_Function_Ptr_t;
/******************************************************************************
 * Global Data
 ******************************************************************************/
NMEA_Data_u NMEA_Data;
/******************************************************************************
 * Module Static Data
 ******************************************************************************/
static char dataID;
static char talkerID;
static uint32_t messageMask;
static int initialized = 0;

static const NMEA_StateStr_t NMEA_StateMap[] = 
{
    {START, "START"},
    {DATA_ID, "DATA_ID"},
    {TALKER_ID, "TALKER_ID"},
    {MESSAGE_TYPE, "MESSAGE_TYPE"},
    {FIELDS, "FIELDS"},
    {CHECKSUM, "CHECKSUM"},
    {state__NELEMS, NULL}
};

/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/
static void setID(char token[], void *ID);
static void fixTime(char token[], void *time);
static void latDir(char token[], void *lat);
static void findCoord(char token[], void *coord);
static void longDir(char token[], void *longitude);
static void parseU8_0(char token[], void *qs);
static void parseFloat_0(char token[], void *hae);
static void parseU8_FF(char token[], void *data_value);
static void parseS8_FF(char token[], void *type);
static void parseS32_FF(char token[], void *year);
static NMEA_Message_e decodeMessage(char* messageString);
static const NMEA_Function_Ptr_t* getTable(NMEA_Message_e messageType);

static const char* NMEA_GetStateName(state_e state);

/******************************************************************************
 * Tables
 ******************************************************************************/
/**
 * GGA_table[] is a table in the format {function, function input, next field}
 * including all the functions necessary to parse a GGA string.
 */
static const NMEA_Function_Ptr_t GGA_table[] =
{
    {setID, &NMEA_Data.GGA.talkerID},
    {fixTime, &NMEA_Data.GGA.fixTime},
    {findCoord, &NMEA_Data.GGA.latitude},
    {latDir, &NMEA_Data.GGA.latitude},
    {findCoord, &NMEA_Data.GGA.longitude},
    {longDir, &NMEA_Data.GGA.longitude},
    {parseU8_0, &NMEA_Data.GGA.fixQuality},
    {parseU8_0, &NMEA_Data.GGA.nSatellites},
    {parseFloat_0, &NMEA_Data.GGA.hdop},
    {parseFloat_0, &NMEA_Data.GGA.altitude},
    {parseU8_0, &NMEA_Data.GGA.altitudeUnit},
    {parseFloat_0, &NMEA_Data.GGA.elevation},
    {parseU8_0, &NMEA_Data.GGA.elevationUnit},
    {parseU8_FF, &NMEA_Data.GGA.dGpsStale},
    {parseU8_FF, &NMEA_Data.GGA.dGpsID},
    {NULL, NULL}
};

/**
 * GLL_table[] is a table in the format {function, function input, next field}
 * including all the functions necessary to parse a GLL string.
 */
static const NMEA_Function_Ptr_t GLL_table[] =
{
    {setID, &NMEA_Data.GLL.talkerID},
    {findCoord, &NMEA_Data.GLL.latitude},
    {latDir, &NMEA_Data.GLL.latitude},
    {findCoord, &NMEA_Data.GLL.longitude},
    {longDir, &NMEA_Data.GLL.longitude},
    {fixTime, &NMEA_Data.GLL.fixTime},
    {parseS8_FF, &NMEA_Data.GLL.fixType},
    {NULL, NULL}
};

/**
 * ZDA_table[] is a table in the format {function, function input, next field}
 * including all the functions necessary to parse a ZDA string.
 */
static const NMEA_Function_Ptr_t ZDA_table[] =
{
    {setID, &NMEA_Data.ZDA.talkerID},
    {fixTime, &NMEA_Data.ZDA.fixTime},
    {parseU8_FF, &NMEA_Data.ZDA.day},
    {parseU8_FF, &NMEA_Data.ZDA.month},
    {parseS32_FF, &NMEA_Data.ZDA.year},
    {parseU8_FF, &NMEA_Data.ZDA.zoneHours},
    {parseU8_FF, &NMEA_Data.ZDA.zoneMinutes},
    {NULL, NULL}
};

/**
 * Message ID table to match
 */
static const messageIDEntry_t MessageID_Table[] =
{
    {NMEA_MESSAGE_GGA, "GGA"},
    {NMEA_MESSAGE_GLL, "GLL"},
    {NMEA_MESSAGE_GSV, "GSV"},
    {NMEA_MESSAGE_RMC, "RMC"},
    {NMEA_MESSAGE_ZDA, "ZDA"},
    {NMEA_MESSAGE_VTG, "VTG"},
    {NMEA_MESSAGE_NONE, NULL}
};

/******************************************************************************
 * Function Definitions
 ******************************************************************************/
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
 * Converts coordinate from degrees minutes seconds to decimal degrees
 * 
 * findLat takes the token with the coordinate in degrees, minute form and
 * calculates the coordinate in degrees then sets the value to the
 * Latitude variable for the message type.
 * @param      token  The token with the coordinate which will be parsed
 * @param      lat    The coordinate variable to be set
 */
static void findCoord(char token[], void *coord)
{
    int temp_int;
    float temp_frac;
    char* decimalIndex;

    if(token[0] == '\0')
    {
        *(float *)coord = 0;
    }
    else
    {
        decimalIndex = strchr(token, '.');
        temp_frac = atof(decimalIndex - 2);
        decimalIndex -= 2;
        *decimalIndex = '\0';
        temp_int = atoi(&token[0]);
        *(float *)coord = temp_int + temp_frac/60;
    }
}

/**
 * Puts a sign on the latitude N/S
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
 * Puts a sign on the longitude E/W
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
 * Parses the quality and number of satellites
 *
 * fixQS takes the quality or number of satellites from the token
 * and sets it as a uint8 value in the respective message type
 * @param      token    The token which contains the quality
 *                      or number of satellites and will be parsed
 * @param      qs       The variable to be set
 */
static void parseU8_0(char token[], void *qs)
{
    if(token[0] == '\0')
    {
        *(uint8_t *)qs = 0;
    }
    else
    {
        *(uint8_t *)qs = atoi(token);
    }
}

/**
 * Parses the Horizontal Dilution of precision, altitude, and elevation
 *
 * fixHAE takes the token containing the Horizontal Dilution of precision,
 * altitude, or elevation and sets the variable hae from the respective
 * message type to the token value
 * @param      token  The token containing the Horizontal Dilution of precision,
 *                    altitude, or elevation
 * @param      hae   The variable to be set
 */
static void parseFloat_0(char token[], void *hae)
{
    if(token[0] == '\0')
    {
        *(float *)hae = 0;
    }
    else
    {
        *(float *)hae = atof(token);
    }
}

/**
 * Parses the dGpsStale, dGpsID, day, month, zone hours, and zone minute values
 *
 * eightBitInt takes the token and sets it to the stale value
 * @param      token  The token containing dGpsStale, dGpsID, day, month,
 *                    zone hours, or zone minutes
 * @param      data_value  The variable to be set
 */
static void parseU8_FF(char token[], void *data_value)
{
    if(token[0] == '\0')
    {
        *(uint8_t *)data_value = 0xFF;
    }
    else
    {
        *(uint8_t *)data_value = atoi(token);
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
static void parseS8_FF(char token[], void *type)
{
    if(token[0] == '\0')
    {
        *(char *)type = 0xFF;
    }
    else
    {
        *(char *)type = atoi(token);
    }
}

/**
 * Sets the Year
 *
 * setYear sets the number in the token to the year variable in the
 * corresponding message type
 * @param      token  The token containing the year
 * @param      year   The variable to be set
 */
static void parseS32_FF(char token[], void *year)
{
    if(token[0] == '\0')
    {
        *(int *)year = 0xFF;
    }
    else
    {
        *(int *)year = atoi(token);
    }
}

void NMEA_Init(NMEA_Config_t* pConfig)
{
    messageMask = pConfig->messageMask;
    initialized = 1;
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
    static int counter = 0;
    static char str[STRLENGTH];
    static int next_field = 0;
    static const NMEA_Function_Ptr_t *messageTable = NULL;
    if(!initialized)
    {
        LOG_CRIT(LOG_SUBSYSTEM_NMEA, "NMEA not initialized");
    }
    LOG_DEBUG(LOG_SUBSYSTEM_NMEA, "NMEA: %02hhx, %c, %s", c, c, NMEA_GetStateName(decodeState));

    switch(decodeState)
    {
    case START:
        if(c == '$')
        {
            counter = 0;
            checksum = 0;
            next_field = 0;
            message = NMEA_MESSAGE_NONE;
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
        str[counter] = c;
        counter++;
        if(counter == 3)
        {
            str[counter] = '\0';
            message = decodeMessage(str);
            LOG_DEBUG(LOG_SUBSYSTEM_NMEA, "Got message %d", message);
            messageTable = getTable(message);

            counter = 0;
            decodeState = (message != NMEA_MESSAGE_NONE) ? FIELDS : START;
        }
        checksum ^= c;
        break;
    case FIELDS:
        LOG_DEBUG(LOG_SUBSYSTEM_NMEA, "Field: counter: %d, next_field: %d", counter, next_field);
        if(c != ',' && c != '*' && counter < STRLENGTH)
        {
            str[counter++] = c;
        }
        else
        {
            str[counter] = '\0';
            counter = 0;
            if(messageTable)
            {
                if(messageTable[next_field].field_func_ptr != NULL)
                {
                    (messageTable[next_field].field_func_ptr)(str,
                        messageTable[next_field].arg0);
                }
                next_field++;
                if(messageTable[next_field].arg0 == NULL)
                {
                    decodeState = CHECKSUM;
                    next_field = 0;
                    counter = 0;
                }
            }
        }
        if(c != '*')
        {
            checksum ^= c;
        }
        break;
    case CHECKSUM:
        str[counter++] = c;
        if(counter == 2)
        {
            str[counter] = '\0';
            if(checksum != strtol(str, NULL, BASE))
            {
                message = NMEA_MESSAGE_ERROR;
            }

            decodeState = START;
            return message;
        }
        break;
    default:
        decodeState = START;
        break;
    }
    return NMEA_MESSAGE_NONE;
}

/**
 * Retrieves the corresponding NMEA_Message_e enumeration for the given
 * messageString.
 * @param messageString Message string to parse
 * @return  Corresponding NMEA_Message_e if successful, otherwise
 *          NMEA_MESSAGE_NONE
 */
static NMEA_Message_e decodeMessage(char* messageString)
{
    const messageIDEntry_t *pEntry = MessageID_Table;
    LOG_INFO(LOG_SUBSYSTEM_NMEA, "Decoding %s", messageString);
    while(pEntry->msg != NMEA_MESSAGE_NONE)
    {
        LOG_DEBUG(LOG_SUBSYSTEM_NMEA, "Comparing against %s", pEntry->str);
        if(strcmp(messageString, pEntry->str) == 0)
        {
            if((pEntry->msg & messageMask) == 0)
            {
                LOG_WARN(LOG_SUBSYSTEM_NMEA, "%s not enabled", messageString);
            }
            return pEntry->msg & messageMask;
        }
        else
        {
            pEntry++;
        }
    }
    return pEntry->msg;
}

/**
 * Retrieves the corresponding message table.
 * @param messageType   Message type
 * @return  Corresponding message table, otherwise NULL
 */
static const NMEA_Function_Ptr_t* getTable(NMEA_Message_e messageType)
{
    switch(messageType)
    {
    case NMEA_MESSAGE_GGA:
        return GGA_table;
    case NMEA_MESSAGE_GLL:
        return GLL_table;
    case NMEA_MESSAGE_ZDA:
        return ZDA_table;
    default:
        return NULL;
    }
}

static const char* NMEA_GetStateName(state_e state)
{
    const NMEA_StateStr_t* pNode;
    for(pNode = NMEA_StateMap; pNode->name; pNode++)
    {
        if(pNode->state == state)
        {
            return pNode->name;
        }
    }
    return NULL;
}