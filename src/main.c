#include "nmea.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
extern char *strtok_e(char *str, char delim);

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
                printf("NMEA_MESSAGE_GGA\n");
                NMEA_GGA_t GGA;
                char str[256];
                char* token;
                char delimiter = ',';
                /*copy nmeaString to temp buffer*/
                strcpy(str, nmeaString);

                /*find nmea type*/
                token = strtok_e(str, delimiter);
                printf("token = %s\n", token);

                /*store talkerID*/
                GGA.talkerID = token[2];

                /*find and store hour minute second*/
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(token == NULL)
                {
                    GGA.fixHour = 0;
                    GGA.fixMinute = 0;
                    GGA.fixSecond = 0;
                }
                else
                {
                    sscanf(token, "%2d%2d%2d", &GGA.fixHour, &GGA.fixMinute, &GGA.fixSecond);
                }
                printf("fixHour = %d\n", GGA.fixHour);
                printf("fixMinute = %d\n", GGA.fixMinute);
                printf("fixSecond = %d\n", GGA.fixSecond);

                /*equation for GGA latitude/longitude: first 2 digits + last 4/60*/
                /*find and store latitude*/
                int temp_int;
                float temp_frac;
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(token == NULL)
                {
                    GGA.latitude = 0;
                }
                else
                {
                    sscanf(token, "%2d%f", &temp_int, &temp_frac);
                    printf("temp_int = %d\n", temp_int);
                    printf("temp_frac = %f\n", temp_frac);
                    GGA.latitude = temp_int + temp_frac/60;
                }
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(GGA.latitude != 0 && token[0] == 'S')
                {
                    GGA.latitude = -1 * GGA.latitude;
                }
                printf("latitude = %f\n", GGA.latitude);

                /*find and store longitude*/
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(token == NULL)
                {
                    GGA.longitude = 0;
                }
                else
                {
                    sscanf(token, "%3d%f", &temp_int, &temp_frac);
                    printf("temp_int = %d\n", temp_int);
                    printf("temp_frac = %f\n", temp_frac);
                    GGA.longitude = temp_int + temp_frac/60;
                }
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(GGA.longitude != 0 && token[0] == 'W')
                {
                    GGA.longitude = -1 * GGA.longitude;
                }
                printf("longitude = %f\n", GGA.longitude);

                /*find and store fixQuality*/
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(token == NULL)
                {
                    GGA.fixQuality = 0;
                }
                else
                {
                    sscanf(token, "%d", &GGA.fixQuality);
                }
                printf("fixQuality = %d\n", GGA.fixQuality);

                /*find and store nSatellites*/
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(token == NULL)
                {
                    GGA.nSatellites = 0;
                }
                else
                {
                    sscanf(token, "%d", &GGA.nSatellites);
                }
                printf("nSatellites = %d\n", GGA.nSatellites);

                /*find and store hdop*/
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(token == NULL)
                {
                    GGA.hdop = 0;
                }
                else
                {
                    sscanf(token, "%f", &GGA.hdop);
                }
                printf("hdop = %f\n", GGA.hdop);

                /*find and store altitude*/
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(token == NULL)
                {
                    GGA.altitude = 0;
                }
                else
                {
                    sscanf(token, "%f", &GGA.altitude);
                }
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                printf("altitude = %.3f\n", GGA.altitude);
                
                /*find and store elevation*/
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(token == NULL)
                {
                    GGA.elevation = 0;
                }
                else
                {
                    sscanf(token, "%f", &GGA.elevation);
                }
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                printf("elevation = %.3f\n", GGA.elevation);

                /*find and store dGpsStale*/
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(token == NULL)
                {
                    GGA.dGpsStale = 0xFF;
                }
                else
                {
                    sscanf(token, "%d", &GGA.dGpsStale);
                }
                printf("dGpsStale = %d\n", GGA.dGpsStale);

                /*find and store dGpsID*/
                token = strtok_e(NULL, delimiter);
                printf("token = %s\n", token);
                if(token == NULL)
                {
                    GGA.dGpsID = 0xFF;
                }
                else
                {
                    sscanf(token, "%d", &GGA.dGpsID);
                }
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