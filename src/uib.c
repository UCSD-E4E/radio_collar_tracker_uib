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
    
    //For testing I2C.c
    uint8_t address = 0x1E;
    uint8_t data[32] = {}; 
    uint8_t register_address = 0x00;
    uint8_t *data_ptr = data;
    uint16_t data_size = 1;//sizeof(*data_ptr)/sizeof(data_ptr[0]);
    uint32_t timeout_ms = 0x00; //not yet implemented
    int read_check = 0;
    int init_check = 0;
    int write_check = 0;
    init_check = 0;
    read_check = 0;
    write_check = 0;
    unsigned char user_input = 'b';
    
    //data[0] = 0x70; 
    //init_check = I2C_Init();
    //write_check = I2C_MasterRegisterTransmit(address, register_address, data_ptr, data_size, timeout_ms); //with *data_ptr being 0x00, this will tell the compass to address it's 0th register
                                                                                  //(which is Config register A)
    //register_address = 0x01;
    //data[0] = 0xA0;
    //write_check = I2C_MasterRegisterTransmit(address, register_address, data_ptr, data_size, timeout_ms);
    //register_address = 0x02;
    //data[0] = 0x01;
    //write_check = I2C_MasterRegisterTransmit(address, register_address, data_ptr, data_size, timeout_ms);

    //register_address = 0x06;
    //read_check = I2C_MasterRegisterReceive(address, register_address, data_ptr, data_size, timeout_ms);
    

        
    while(1)
    {
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

      register_address = 0x06;
      data_size = 0x03;
      read_check = I2C_MasterRegisterReceive(address, register_address, data_ptr, data_size, timeout_ms);
      Serial_Printf(HAL_SystemDesc.pOBC, "Press 'a' to continue \n\r");
      while(user_input != 'a'){
          Serial_Read(HAL_SystemDesc.pOBC, &user_input, sizeof(user_input));
      }
      user_input = 'b';

      //read_check = I2C_MasterRegisterReceivePt1(address, register_address, data_ptr, data_size, timeout_ms);
      Serial_Printf(HAL_SystemDesc.pOBC, "Press 'a' to continue \n\r");
      while(user_input != 'a'){
          Serial_Read(HAL_SystemDesc.pOBC, &user_input, sizeof(user_input));
      }
      user_input = 'b';
      TW_Stop();
    

        // nchars = Serial_Read(HAL_SystemDesc.pGPS, rxBuf, 63);
        // if(nchars > 0)
        // {
        //     for(int i = 0; i < nchars; i++)
        //     {
        //         if(sensorParse(rxBuf[i]) == 1)
        //         {
        //             int init_check = 0;
        //             init_check = I2C_Init();
        //             printf("init_check: %d\n", init_check);
        //             Serial_Write(HAL_SystemDesc.pOBC, (uint8_t*) &sensor_packet,
        //                 sizeof(sensor_packet));


		//             }
        //     }
        // }

        // nchars = Serial_Read(HAL_SystemDesc.pOBC, rxBuf, 63);
        // if(nchars > 0)
        // {
        //     for(int i = 0; i < nchars; i++)
        //     {
        //         if(decodeStatusPacket(&status_data, rxBuf[i]))
        //         {
        //             StatusDecoder_encodeLEDs(&status_data.payload);
        //         }
        //     }
        // }
        ///////////I2C.c Master Read Write Testing///////////////

        //for init testing
        //Serial_Printf(HAL_SystemDesc.pOBC, "init_check: %d\n", init_check);
        //Serial_Printf(HAL_SystemDesc.pOBC, "TWSR init: %X\n\r", TWSR);
        //Serial_Printf(HAL_SystemDesc.pOBC, "TWCR init: %X\n\r", TWCR);
        
        //for write testing
        Serial_Printf(HAL_SystemDesc.pOBC, "write_check: %d\n\r", write_check);
        //Serial_Printf(HAL_SystemDesc.pOBC, "data: %X\n\r", *data_ptr);
        //Serial_Printf(HAL_SystemDesc.pOBC, "TWSR write: %X\n\r", TWSR);
        //Serial_Printf(HAL_SystemDesc.pOBC, "TWCR write: %X\n\r", TWCR);
        
        //for read testing 
        Serial_Printf(HAL_SystemDesc.pOBC, "read_check: %d\n\r", read_check);
        Serial_Printf(HAL_SystemDesc.pOBC, "data 1: %X\n\r", data[0]);
        Serial_Printf(HAL_SystemDesc.pOBC, "data 2: %X\n\r", data[1]);
        Serial_Printf(HAL_SystemDesc.pOBC, "data 3: %X\n\r", data[2]);
        Serial_Printf(HAL_SystemDesc.pOBC, "TWSR read: %X\n\r", TWSR);
        Serial_Printf(HAL_SystemDesc.pOBC, "TWCR read: %X\n\r", TWCR);
        
        
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
