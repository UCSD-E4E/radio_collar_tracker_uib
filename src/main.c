#include "nmea.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    NMEA_Config_t nmeaConfig;
    size_t i;

    nmeaConfig.messageMask = 0xFFFFFFFF;

    NMEA_Init(&nmeaConfig);

    const char* nmeaString = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";

    /*format: $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*/

    for(i = 0; i < 6; i++)
    {
        switch(NMEA_Decode(nmeaString[i]))
        {
            case NMEA_MESSAGE_NONE:
            {
                continue;
            }
            case NMEA_MESSAGE_GGA:
            {
                printf("NMEA_MESSAGE_GGA\n");
                NMEA_GGA_t GGA;
                char str[256];
                char *token;
                char *save;
                const char delimiter[2] = ",";
                /*copy nmeaString to temp buffer*/
                strcpy(str, nmeaString);

                /*find nmea type*/
                token = strtok_r(str, delimiter, &save);

                /*store talkerID*/
                GGA.talkerID = token[2];

                /*find and store hour minute second*/
                token = strtok_r(NULL, delimiter, &save);
                sscanf(token, "%2d%2d%2d", &GGA.fixHour, &GGA.fixMinute, &GGA.fixSecond);
                printf("hour = %d\n", GGA.fixHour);
                printf("minute = %d\n", GGA.fixMinute);
                printf("second = %d\n", GGA.fixSecond);

                /*equation for GGA latitude/longitude: first 2 digits + last 4/60*/
                /*find and store latitude*/
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);
                int temp_int;
                float temp_frac;
                sscanf(token, "%2d%f", &temp_int, &temp_frac);
                printf("temp_int = %d\n", temp_int);
                printf("temp_frac = %f\n", temp_frac);
                GGA.latitude = temp_int + temp_frac/60;
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);
                if(token[0] == 'S')
                {
                    GGA.latitude = -1 * GGA.latitude;
                }
                printf("latitude = %f\n", GGA.latitude);

                /*find and store longitude*/
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);
                sscanf(token, "%3d%f", &temp_int, &temp_frac);
                printf("temp_int = %d\n", temp_int);
                printf("temp_frac = %f\n", temp_frac);
                GGA.longitude = temp_int + temp_frac/60;
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);
                if(token[0] == 'W')
                {
                    GGA.longitude = -1 * GGA.longitude;
                }
                printf("longitude = %f\n", GGA.longitude);

                /*find and store fixQuality*/
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);
                sscanf(token, "%d", &temp_int);
                GGA.fixQuality = temp_int;
                printf("fixQuality = %d\n", GGA.fixQuality);

                /*find and store nSatellites*/
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);
                sscanf(token, "%d", &temp_int);
                GGA.nSatellites = temp_int;
                printf("nSatellites = %d\n", GGA.nSatellites);

                /*find and store hdop*/
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);
                sscanf(token, "%f", &temp_frac);
                GGA.hdop = temp_frac;
                printf("hdop = %f\n", GGA.hdop);

                /*find and store altitude*/
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);
                sscanf(token, "%f", &temp_frac);
                GGA.altitude = temp_frac;
                printf("altitude = %.3f\n", GGA.altitude);
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);
                
                /*find and store elevation*/
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);
                sscanf(token, "%f", &temp_frac);
                GGA.elevation = temp_frac;
                printf("elevation = %.3f\n", GGA.elevation);
                token = strtok_r(NULL, delimiter, &save);
                printf("token = %s\n", token);

                /*find and store dGpsStale*/
                token = strtok_r(NULL, delimiter, &save);
                printf("empty token = %s\n", token);

                /*find and store dGpsID*/
                token = strtok_r(NULL, delimiter, &save);
                printf("empty token = %s\n", token);

                // deal with the data
            }
                break;
            default:
            break;
        }
    }
    return 0;
}