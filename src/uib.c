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
#include "serial.h"

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

int main()
{
    uint8_t pOBCBuf[OBC_BUFFER_LEN];
    uint32_t OBCBufIdx = 0;
    if(!initHW())
    {
        LED_setState(LED_STORAGE_STATE, LED_1HZ);
        while(1)
            ;
    }

    LED_setState(LED_COMBINED_STATE, LED_1HZ);
    while(1)
    {
        OBCBufIdx = Serial_Read(pHalSystem->pGPS, pOBCBuf, OBC_BUFFER_LEN);
        Serial_Write(pHalSystem->pOBC, pOBCBuf, OBCBufIdx);
    }
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
