#include "nmea.h"
#include <stddef.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    NMEA_Config_t nmeaConfig;
    size_t i;

    nmeaConfig.messageMask = 0xFFFFFFFF;

    NMEA_Init(&nmeaConfig);

    const char* nmeaString = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,"
        "545.4,M,46.9,M,,*47";

    for(i = 0; i < strlen(nmeaString); i++)
    {
        switch(NMEA_Decode(nmeaString[i]))
        {
            NMEA_MESSAGE_NONE:
                continue;
            NMEA_MESSAGE_GGA:
                // deal with the data
                break;
            default:
            break;
        }
    }

    return 0;
} 
