#include "nmea.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
extern NMEA_GGA_t GGA;

int main(int argc, char const *argv[])
{
    NMEA_Config_t nmeaConfig;
    size_t i;

    nmeaConfig.messageMask = 0xFFFFFFFF;

    NMEA_Init(&nmeaConfig);

    const char* nmeaString = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    //const char* nmeaString = "$GPGLL,4916.45,N,12311.12,W,225444,A,*1D";
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
                    printf("talkerID = %c\n", NMEA_Data.GGA.talkerID);
                    printf("fixHour = %d\n", NMEA_Data.GGA.fixTime[0]);
                    printf("fixMinute = %d\n", NMEA_Data.GGA.fixTime[1]);
                    printf("fixSecond = %d\n", NMEA_Data.GGA.fixTime[2]);
                    printf("latitude = %f\n", NMEA_Data.GGA.latitude);
                    printf("longitude = %f\n", NMEA_Data.GGA.longitude);
                    printf("fixQuality = %d\n", NMEA_Data.GGA.fixQuality);
                    printf("nSatellites = %d\n", NMEA_Data.GGA.nSatellites);
                    printf("hdop = %f\n", NMEA_Data.GGA.hdop);
                    printf("altitude = %f\n", NMEA_Data.GGA.altitude);
                    printf("elevation = %f\n", NMEA_Data.GGA.elevation);
                    printf("dGpsStale = %d\n", NMEA_Data.GGA.dGpsStale);
                    printf("dGpsID = %d\n", NMEA_Data.GGA.dGpsID);
                }
                break;
            case NMEA_MESSAGE_GLL:
                {
                    printf("talkerID = %c\n", NMEA_Data.GLL.talkerID);
                    printf("latitude = %f\n", NMEA_Data.GLL.latitude);
                    printf("longitude = %f\n", NMEA_Data.GLL.longitude);
                    printf("fixHour = %d\n", NMEA_Data.GLL.fixTime[0]);
                    printf("fixMinute = %d\n", NMEA_Data.GGA.fixTime[1]);
                    printf("fixSecond = %d\n", NMEA_Data.GGA.fixTime[2]);
                    printf("fixType = %c\n", NMEA_Data.GLL.fixType);
                }
                break;
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