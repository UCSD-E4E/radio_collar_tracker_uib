/******************************************************************************
 * Includes
 ******************************************************************************/
#include "compass_sim.h"
#include "nmea.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
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
static int testCompass(void);
/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Function Definitions
 ******************************************************************************/
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
            case NMEA_MESSAGE_NONE:
                continue;
            case NMEA_MESSAGE_GGA:
                // deal with the data
                break;
            default:
                break;
        }
    }

    if(!testCompass())
    {
        printf("testCompass failed\n");
    }

    return 0;
}

/**
 * Simple test function for the Compass Simulation module
 * @return 1 if successful, otherwise 0
 */
static int testCompass(void)
{
    Compass_Sim_Config_t simConfig;
    Compass_Config_t compassConfig;
    double compassValue;

    /*
     * Local file dataStore.bin - this should work regardless of platform
     */
    simConfig.path = "dataStore.bin";

    /*
     * This will be how the final API will be used.
     */
    compassConfig.measMode = Compass_Mode_Continuous;

    if(!Compass_Sim_Init(&simConfig))
    {
        printf("testCompass: Failed to init simulator\n");
        return 0;
    }

    if(!Compass_Init(&compassConfig))
    {
        printf("testCompass: Failed to init module\n");
        return 0;
    }

    compassValue = Compass_Read();
    printf("Compass: %lf\n", compassValue);

    Compass_Sim_Deinit();

    return 1;

}