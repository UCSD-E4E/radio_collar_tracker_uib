/*
 * bootloader.c
 *
 *  Created on: Aug 29, 2020
 *      Author: ntlhui
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "cutils.h"
#include "LED_module.h"
#include "sensor_module.h"
#include "serial.h"
#include "status_decoder.h"
#include "I2C.h"
#include "timerfile.h"
#include <stdio.h>
#include <util/atomic.h>
#include <avr/interrupt.h>



#define OBC_BUFFER_LEN  256

typedef struct HAL_System_
{
    SerialDesc_t* pOBC;
    SerialDesc_t* pGPS;
}HAL_System_t;
HAL_System_t HAL_SystemDesc, *pHalSystem = &HAL_SystemDesc;

static int initHW(void);
static void initTimer(void);
static int initSerial(void);
static int initUSBSerial(void);
static int initHWSerial(void);

void appMain(void)
{
    uint8_t rxBuf[64];
    int nchars;
    DataStatusPacket_t status_data;

    while(1)
    {
        nchars = Serial_Read(HAL_SystemDesc.pGPS, rxBuf, 63);
        if(nchars > 0)
        {
            for(int i = 0; i < nchars; i++)
            {
                if(sensorParse(rxBuf[i]) == 1)
                {
                    Serial_Write(HAL_SystemDesc.pOBC, (uint8_t*) &sensor_packet,
                        sizeof(sensor_packet));
                }
            }
        }

        nchars = Serial_Read(HAL_SystemDesc.pOBC, rxBuf, 63);
        if(nchars > 0)
        {
            for(int i = 0; i < nchars; i++)
            {
                if(decodeStatusPacket(&status_data, rxBuf[i]))
                {
                    StatusDecoder_encodeLEDs(&status_data.payload);
                }
            }
        }
    }
    //while(1){}; //inf loop for testing (do nothing until reset button is hit)
}

int main()
{
    if(!initHW())
    {
        LED_setState(LED_STORAGE_STATE, LED_1HZ);
        while(1)
            ;
    }

    LED_setState(LED_COMBINED_STATE, LED_1HZ);
    appMain();
}

static int initHW(void)
{
    LED_init();
    initTimer();

    if(!initSerial())
    {
        return 0;
    }
    return 1;
}
static void initTimer(void)
{
    cli();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 2560;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);
    sei();
}

ISR(TIMER1_COMPA_vect)
{
    LED_control();
}

static int initSerial(void)
{
    if(!initUSBSerial())
    {
        return 0;
    }
    if(!initHWSerial())
    {
        return 0;
    }
    return 1;
}

static int initUSBSerial(void)
{
    SerialConfig_t pConfig;
    pConfig.device = SerialDevice_OBC;
    HAL_SystemDesc.pOBC = Serial_Init(&pConfig);
    if(!HAL_SystemDesc.pOBC)
    {
        return 0;
    }

    return 1;
}

static int initHWSerial(void)
{
    SerialConfig_t pConfig;
    pConfig.device = SerialDevice_GPS;
    HAL_SystemDesc.pGPS = Serial_Init(&pConfig);
    if(!HAL_SystemDesc.pGPS)
    {
        return 0;
    }
    return 1;
}
