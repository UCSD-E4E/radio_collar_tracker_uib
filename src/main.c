#include "nmea.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
extern NMEA_GGA_t GGA;

int main(int argc, char const *argv[])
{
    NMEA_Config_t nmeaConfig;
    size_t i;
    int j;

    nmeaConfig.messageMask = 0xFFFFFFFF;

    NMEA_Init(&nmeaConfig);

    const char* nmeaString = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    /*format: $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*/

    for(i = 0; i < strlen(nmeaString); i++)
    {
        switch(NMEA_Decode(nmeaString[i]))
        {
            case NMEA_MESSAGE_NONE:
            {
                continue;
            }
            case NMEA_MESSAGE_GGA:
            {
                printf("talkerID = %c\n", GGA.talkerID);
                printf("fixHour = %d\n", GGA.fixHour);
                printf("fixMinute = %d\n", GGA.fixMinute);
                printf("fixSecond = %d\n", GGA.fixSecond);
                printf("latitude = %f\n", GGA.latitude);
                printf("longitude = %f\n", GGA.longitude);
                printf("fixQuality = %d\n", GGA.fixQuality);
                printf("nSatellites = %d\n", GGA.nSatellites);
                printf("hdop = %f\n", GGA.hdop);
                printf("altitude = %f\n", GGA.altitude);
                printf("elevation = %f\n", GGA.elevation);
                printf("dGpsStale = %d\n", GGA.dGpsStale);
                printf("dGpsID = %d\n", GGA.dGpsID);

                break;
            }
            case NMEA_MESSAGE_ERROR:
            {
                printf("error\n");
                return 1;
            }
            default:
                break;
        }
    }
    return 0;
}