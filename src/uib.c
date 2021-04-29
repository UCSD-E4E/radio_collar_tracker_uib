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
#include <stdio.h>

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
    int eight = 8;
    
    //For using I2C.c
    uint8_t address = 0x1E;
    uint8_t data[32] = {}; 
    uint8_t register_address = 0x00;
    uint8_t *data_ptr = data;
    uint16_t data_size = 1;
    uint32_t timeout_ms = 0x00; //not yet implemented
    int read_check = 0;
    int init_check = 0;
    int write_check = 0;
    int i = 0;
    int j = 0;

    ///////Setting up Compass for continuous measurement mode///////////

    data_size = 0x01;
    //Set up CRA with TS enabled, 8 averaging, Data output to 30Hz
    register_address = 0x00;
    data[0] = 0xF4; 
    init_check = I2C_Init();
    write_check = I2C_MasterRegisterTransmit(address, register_address, data_ptr, data_size, timeout_ms); //with *data_ptr being 0x00, this will tell the compass to address it's 0th       
    //register (which is Config register A)
    Serial_Printf(HAL_SystemDesc.pOBC, "write_check 1: %d\n\r", write_check);
    
    //Set up CRB with a gain of 1090
    register_address = 0x01;
    data[0] = 0x20;
    write_check = I2C_MasterRegisterTransmit(address, register_address, data_ptr, data_size, timeout_ms);
    Serial_Printf(HAL_SystemDesc.pOBC, "write_check 2: %d\n\r", write_check);
    
    //Set up MR with continuos measurement mode.
    register_address = 0x02;
    data[0] = 0x00;
    write_check = I2C_MasterRegisterTransmit(address, register_address, data_ptr, data_size, timeout_ms);

        
    while(1)
    {

        //for write testing
        Serial_Printf(HAL_SystemDesc.pOBC, "write_check: %d\n\r", write_check);

        
        register_address = 0x06;
        data_size = 0x06;
        //resetting all elements in data to 0 for read

        /////////////Reading from all six XYZ data registers////////////////

        for(i = 0; i < sizeof(data)/sizeof(data[0]); i++){
            data[i] = 0;
        }

        Serial_Printf(HAL_SystemDesc.pOBC, "-----DATA CLEAR ---- \n\r");
        for(i = 0; i < (int)data_size; i++){
            if(i == 0){
                j = 1;
            }

            Serial_Printf(HAL_SystemDesc.pOBC, "data %d: %X\n\r", j, data[i]);
            j++;
        }
    
        read_check = I2C_MasterRegisterReceive(address, register_address, data_ptr, data_size, timeout_ms);
    
        //for read testing 
        Serial_Printf(HAL_SystemDesc.pOBC, "read_check: %d\n\r", read_check);

        Serial_Printf(HAL_SystemDesc.pOBC, "-----DATA OUT---- \n\r");
        for(i = 0; i < (int)data_size; i++){
            if(i == 0){
                j = 1;
            }
            Serial_Printf(HAL_SystemDesc.pOBC, "data %d: %X\n\r", j, data[i]);
            j++;
        }
        Serial_Printf(HAL_SystemDesc.pOBC, "TWSR read: %X\n\r", TWSR);
        Serial_Printf(HAL_SystemDesc.pOBC, "TWCR read: %X\n\r", TWCR);

        register_address = 0x03;
        I2C_SetRegisterPointer(address, register_address, timeout_ms);
        
    }
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
