/******************************************************************************
 * Includes
 ******************************************************************************/
#include "compass_sim.h"
#include "nmea.h"
#include "voltage_sim.h"
#include "sensor_module.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
/******************************************************************************
 * Defines
 ******************************************************************************/
extern uint32_t sensorParse(char c);
extern void LEDInit();
extern int LEDcontrol();
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
static int testVoltage(void);
/******************************************************************************
 * Local Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Function Definitions
 ******************************************************************************/
int main(int argc, char const *argv[])
{
    NMEA_Config_t nmeaConfig;

    nmeaConfig.messageMask = 0xFFFFFFFF;

    NMEA_Init(&nmeaConfig);

    // const char* nmeaString = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    //const char* nmeaString = "$GPGLL,4916.45,N,12311.12,W,225444,A,*1D";
    //const char* nmeaString = "$GPZDA,201530.00,04,07,2002,00,00*60";
    /*format: $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*/

    LEDInit();

    while(1)
    {
        LEDcontrol();
            
        FILE* nmeaFP;
        char c;

        nmeaFP = fopen("gps.txt", "r");
        if(nmeaFP == NULL)
        {
            printf("gps.txt does not exist\n");
            return -1;
        }
        while((c = getc(nmeaFP)) != EOF)
        {
            sensorParse(c);
        }
        fclose(nmeaFP);

        if(!testCompass())
        {
            printf("testCompass failed\n");
        }
        else
        {
            printf("testCompass passed\n");
        }

        if(!testVoltage())
        {
            printf("testVoltage failed\n");
        }
        else
        {
            printf("testVoltage passed\n");
        }
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
    int16_t compassValue;

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

    if(!Compass_Read(&compassValue))
    {
        printf("testCompass: Failed to read\n");
        return 0;
    }
    printf("Compass: %d\n", compassValue);

    Compass_Sim_Deinit();

    return 1;

}

/**
 * Simple test function for the Voltage Simulation module
 * @return  1 if successful, otherwise 0
 */
static int testVoltage(void)
{
    Voltage_Sim_Config_t simConfig;
    uint16_t voltageValue;

    /* Local file voltageSim.bin - this should work regardless of platform */
    simConfig.path = "voltageSim.bin";

    /* This will be how the final API will be used. */
    if(!Voltage_Sim_Init(&simConfig))
    {
        printf("testVoltage: failed to init simulator\n");
        return 0;
    }

    if(!Voltage_Init())
    {
        printf("testVoltage: failed to init module\n");
        return 0;
    }

    if(!Voltage_Read(&voltageValue))
    {
        printf("testVoltage: Failed to read\n");
        return 0;
    }

    printf("Voltage: %u\n", voltageValue);

    Voltage_Sim_Deinit();
    return 1;
}