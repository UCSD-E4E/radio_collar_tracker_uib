/*
 * I2C.c
 *
 *  Created on: Sep 1, 2020
 *      Author: ntlhui
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "I2C.h"

#include <avr/io.h>
#include <stdint.h>
#include "cutils.h"

/******************************************************************************
 * File Macro Definitions
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/
typedef enum I2C_MODE_
{
    I2C_MODE_WRITE,
    I2C_MODE_READ,
    I2C_MODE_READREG
}I2C_MODE_e;

typedef enum I2C_STATE_
{
    I2C_STATE_START,
    I2C_STATE_SLAW,
    I2C_STATE_SLAR,
    I2C_STATE_DATA_W,
    I2C_STATE_DATA_R,
    I2C_STATE_ERROR,
    I2C_STATE_STOP,
    I2C_STATE_IDLE
}I2C_STATE_e;

typedef enum I2C_STATUS_
{
    I2C_STATUS_START = 0x08,
    I2C_STATUS_REPEAT_START = 0x10,
    I2C_STATUS_START_W_ACK = 0x18,
    I2C_STATUS_START_W_NACK = 0x20,
    I2C_STATUS_DATA_W_ACK = 0x28,
    I2C_STATUS_DATA_W_NACK = 0x30,
    I2C_STATUS_LOST_ARB = 0x38,
    I2C_STATUS_START_R_ACK = 0x40,
    I2C_STATUS_START_R_NACK = 0x48,
    I2C_STATUS_DATA_R_ACK = 0x50,
    I2C_STATUS_DATA_R_NACK = 0x58,
    I2C_STATUS_BUSY = 0xF8,
    I2C_STATUS_ERROR = 0x00
}I2C_STATUS_e;

typedef struct I2C_Desc_
{
    volatile I2C_STATE_e state;
    volatile I2C_MODE_e mode;
    uint8_t deviceAddr;
    uint8_t* pData;
    size_t dataLen;
    size_t dataIdx;
}I2C_Desc_e;
/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Module Static Data
 ******************************************************************************/
static void I2C_SetStart(void);
/******************************************************************************
 * Module Global Data
 ******************************************************************************/
I2C_Desc_e I2C_Desc;
/******************************************************************************
 * Function Definitions
 ******************************************************************************/
/**
 * Initializes the I2C Controller
 * @return  1 if successful, otherwise 0
 */
int I2C_Init(void)
{
    CLEARBIT(PRTWI, PRR0);
    SETBIT(PORTD0, PORTD);
    SETBIT(PORTD1, PORTD);

    TWBR = 18;
    SETFIELD(0, TWPS0, 0x03, TWSR);

    SETBIT(TWEA, TWCR);
    SETBIT(TWEN, TWCR);
    SETBIT(TWIE, TWCR);
    SETBIT(TWINT, TWCR);
    I2C_Desc.state = I2C_STATE_IDLE;

    return 1;
}

int I2C_MasterTransmit(uint8_t deviceAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms)
{

    return 1;
}

int I2C_MasterReceive(uint8_t deviceAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms)
{
    I2C_Desc.deviceAddr = deviceAddress;
    I2C_Desc.pData = pData;
    I2C_Desc.dataLen = size;
    I2C_Desc.mode = I2C_MODE_READ;
    if(!I2C_SetStart())
    {
        return 0;
    }
    I2C_WaitComplete(timeout_ms);
    return 1;
}

void I2C_DoStart(I2C_Desc_e *I2C_Desc)
{
    if(GETFIELD(0x1F, TWS3, TWSR) == I2C_STATUS_START)
    {
        uint8_t slaw = I2C_Desc->deviceAddr & 0x7F << 1;
        slaw |= I2C_Desc->mode == I2C_MODE_WRITE ? 1 : 0;
        TWDR = slaw;
        switch(I2C_Desc->mode)
        {
        case I2C_MODE_READREG:
        case I2C_MODE_WRITE:
            I2C_Desc->state = I2C_STATE_SLAW;
            break;
        case I2C_MODE_READ:
            I2C_Desc->state = I2C_STATE_SLAR;
            break;
        default:
            I2C_Desc->state = I2C_STATE_ERROR;
        }
        SETBIT(TWINT, TWCR);
    }
}

void I2C_DoSLAW(I2C_Desc_e *I2C_Desc)
{
    switch(GETFIELD(0x1F, TWS3, TWSR))
    {
    case I2C_STATUS_START_W_ACK:
        /*
         * Switch to transmitting data
         */
        TWDR = I2C_Desc->pData[0];
        I2C_Desc->dataIdx = 1;
        I2C_Desc->state = I2C_STATE_DATA_W;
        SETBIT(TWINT, TWCR);
        break;
    default:
    case I2C_STATUS_START_W_NACK:
    case I2C_STATUS_LOST_ARB:
        I2C_Desc->state = I2C_STATE_ERROR;
        break;
    }
}

void I2C_DoDataW(I2C_Desc_e *I2C_Desc)
{
    switch(GETFIELD(0x1F, TWS3, TWSR))
    {
    case I2C_STATUS_DATA_W_ACK:
        if(I2C_Desc->dataIdx < I2C_Desc->dataLen)
        {
            /*
             * Still have data to transmit, transmit
             */
            TWDR = I2C_Desc->pData[I2C_Desc->dataIdx++];
            I2C_Desc->state = I2C_STATE_DATA_W;
            SETBIT(TWINT, TWCR);
        }
        else
        {
            /*
             * Done transmitting.  If ReadReg, repeat start, otherwise stop
             */
            if(I2C_Desc->mode == I2C_MODE_READREG)
            {
                I2C_Desc->state = I2C_STATE_START;
                SETBIT(TWSTA, TWCR);
                SETBIT(TWINT, TWCR);
            }
            else
            {
                I2C_Desc->mode = I2C_STATE_STOP;
                SETBIT(TWSTO, TWCR);
                SETBIT(TWINT, TWCR);
            }
        }
        break;
    case I2C_STATUS_DATA_W_NACK:
    case I2C_STATUS_LOST_ARB:
    default:
        I2C_Desc->state = I2C_STATE_ERROR;
    }
}

ISR(TWI_vect)
{
    switch(I2C_Desc.state)
    {
    case I2C_STATE_START:
        I2C_DoStart(&I2C_Desc);
        break;
    case I2C_STATE_SLAW:
        I2C_DoSLAW(&I2C_Desc);
        break;
    case I2C_STATE_DATA_W:
        I2C_DoDataW(&I2C_Desc);
        break;
    case I2C_STATE_STOP:
        break;
    case I2C_STATE_SLAR:
        switch(GETFIELD(0x1F, TWS3, TWSR))
        {
        case I2C_STATUS_START_R_ACK:
            I2C_Desc.dataIdx = 0;
            I2C_Desc.pData[I2C_Desc.dataIdx++] = TWDR;
            I2C_Desc.state = I2C_STATE_DATA_R;
            SETBIT(TWEA, TWCR);
            SETBIT(TWINT, TWCR);
            break;
        default:
        case I2C_STATUS_START_R_NACK:
        case I2C_STATUS_LOST_ARB:

            break;
        }
        break;
    default:
        I2C_Desc.state = I2C_STATE_ERROR;
        break;
    }
}
static int I2C_SetStart(void)
{
    if(I2C_Desc.state != I2C_STATE_IDLE)
    {
        return 0;
    }
    I2C_Desc.state = I2C_STATE_START;
    SETMASK(TWINT | TWSTA | TWEN, TWCR);
    return 1;
}
