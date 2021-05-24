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
#include "serial.h"
#include <stddef.h>


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

// typedef struct HAL_System_
// {
//     SerialDesc_t* pOBC;
//     SerialDesc_t* pGPS;
// }HAL_System_t;
// HAL_System_t HAL_SystemDesc, *pHalSystem = &HAL_SystemDesc;
/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Module Static Data
 ******************************************************************************/
//static void I2C_SetStart(void);
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

    //digitalWrite(SDA, 1);
    //digitalWrite(SCL, 1);

    CLEARBIT(PRTWI, PRR0); //Power reduction register to enable TWI 7.9.2
    SETBIT(PORTD0, PORTD); // read thru 10.2.1
    SETBIT(PORTD1, PORTD); // PORT Pins set to 1 activate their pullup resistor. PORTD0 and PORTD1 are SCL and SDA

    TWBR = 18;
    //SETFIELD(0, TWPS0, 0x03, TWSR); //this is setting the first prescaler, see what that value needs to be
    CLEARMASK((1 << TWPS0) | (1 << 0x03) , TWSR); //might need to be a setmask instead

    SETMASK((1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT), TWCR); //learn how to convert to bit mask  1100 0101

    return 1;
}

//Maybe make a compass init, that sends in the correct commands to set up the compass.


/**
 * Master Write Function 
 * Takes in data pointer, amount of data, the address of the device the write is being sent to, and a timeout var
 * @return  1 if successful, otherwise 0
 */

int I2C_MasterTransmit(uint8_t deviceAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms)
{
    uint16_t i;

    //START Command
    if(TW_Start() == 0){
        return 0;
    }

    //Set deviceAddress to SLA+W
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x00 & 0x01); //Check if SLA+W LSb should be 0 or 1
    
    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+W was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 0;
    }

    for(i = 0x00; i < size; i++){
        
        //load TWDR with the current data message
        TWDR = pData[i];

        //Clear Inturrupt
        TW_ClearInterrupt();
        //GETBIT() will make this easier to read

        //check if MT of Data was acknowledged
        CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
        if((TWSR & 0xF8) != I2C_STATUS_DATA_W_ACK){
            return 0;
        }

    }
    //Send STOP command
    TW_Stop();
    return 1;
}
/**
 * Master Write Function after sending the location of the of the register being written to
 * Takes in data pointer, amount of data, the address of the device the write is being sent to, the address of the register
 * at the device that the data is being sent to, and a timeout var
 * @return  1 if successful, otherwise 0
 */
int I2C_MasterRegisterTransmit(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms){

    uint16_t i;

    //START Command
    if(TW_Start() == 0){
        return 2;
    }

    //Set deviceAddress to SLA+W
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x00 & 0x01); 
    
    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+W was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 3;
    }

    //send the register address to the compass
    TWDR = registerAddress;

    //Clear Interrupt
    TW_ClearInterrupt();

    //check if the transmission of the register location was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_DATA_W_ACK){
        return 4;
    }
    
    //run a repeated start
    if(TW_RepeatedStart() == 0){
        return 5;
    };

    //Set deviceAddress to SLA+W
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x00 & 0x01); 
    
    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+W was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 6;
    }

    for(i = 0x00; i < size; i++){ 
        
        //load TWDR with the current data message
        TWDR = pData[i];

        //Clear Interrupt
        TW_ClearInterrupt();
        //GETBIT() will make this easier to read

        //check if MT of Data was acknowledged
        CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
        if((TWSR & 0xF8) != I2C_STATUS_DATA_W_ACK){
            return 7;
        }

    }

    TW_Stop();
    return 1;
}

/**
 * Master Read Function 
 * Takes in pointer to where the read data will be stored, the amount of read data, 
 * the address of the device being read from, and a timeout var
 * @return  1 if successful, otherwise 0
 */
int I2C_MasterReceive(uint8_t deviceAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms)
{
    uint16_t i;

    //START Command, or Repeated start if STOP was not sent before this function
    if(TW_Start() == 0){
        return 0;
    }

    //Set deviceAddress to SLA+R
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x01 & 0x01); //check LSb for R/W
    //send address to TWDR

    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+R was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR);
    if((TWSR & 0xF8) != I2C_STATUS_START_R_ACK){
        return 0;
    }

    for(i = 0x00; i < size; i++){
        //Record data from the TWDR
        pData[i] = TWDR;

        //Clear Inturrupt
        TW_ClearInterrupt();

        //check if MT of SLA+W was acknowledged
        CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR);
        if((TWSR & 0xF8) != I2C_STATUS_DATA_R_ACK){
            return 0;
        }
    }
    //Send STOP command
    TW_Stop();
    return 1;
}

/**
 * Master Read Function after sending the location of the of the register being written to
 * Takes in data pointer, amount of data, the address of the device the write is being sent to, the address of the register
 * at the device that the data is being read from, and a timeout var
 * @return  1 if successful, otherwise 0
 */
int I2C_MasterRegisterReceive(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms){
    
    uint16_t i;
    //unsigned char user_input = 'b'; //for testing

    //START Command
    if(TW_Start() == 0){
        return 2;
    }

    //Set deviceAddress to SLA+W
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x00 & 0x01); 
    
    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+W was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 3;
    }

    //send the register address to the compass
    TWDR = registerAddress;

    //Clear Interrupt
    TW_ClearInterrupt();

    //check if the transmission of the register location was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_DATA_W_ACK){
        return 4;
    }
    
    /////////For Testing//////////
    // Serial_Printf(HAL_SystemDesc.pOBC, "Press 'a' to continue \n\r");
    // while(user_input != 'a'){
    //       Serial_Read(HAL_SystemDesc.pOBC, &user_input, sizeof(user_input));
    // }
    //////////////////////////////
    
    //run a repeated start
    if(TW_RepeatedStart() == 0){
        return 5;
    };

    //Set deviceAddress to SLA+R
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x01 & 0x01);

    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+R was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR);
    if((TWSR & 0xF8) != I2C_STATUS_START_R_ACK){
        return 6;
    }

    for(i = 0x00; i < size; i++){
        

        //Record data from the TWDR
        pData[i] = TWDR;

        //if the last byte of data is reached (indicated by a NACK in TWSR) leave the loop
        if((TWSR & 0xF8) == I2C_STATUS_DATA_R_NACK){
            break;
        }
        
        //Clear Inturrupt
        TW_ClearInterrupt();

        //check if MT of SLA+W was acknowledged
        CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR);
        if((TWSR & 0xF8) != I2C_STATUS_DATA_R_ACK && (TWSR & 0xF8) != I2C_STATUS_DATA_R_NACK){
            return 7;
        }


    }
    /////////For Testing//////////
    // Serial_Printf(HAL_SystemDesc.pOBC, "Press 'b' to continue \n\r");
    // while(user_input != 'b'){
    //       Serial_Read(HAL_SystemDesc.pOBC, &user_input, sizeof(user_input));
    // }
    //////////////////////////////
    TW_Stop();
    return 1;
}

int I2C_SetRegisterPointer(uint8_t deviceAddress, uint8_t registerAddress, uint32_t timeout_ms){

    uint16_t i;

    //START Command
    if(TW_Start() == 0){
        return 2;
    }

    //Set deviceAddress to SLA+W
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x00 & 0x01); 
    
    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+W was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 3;
    }

    //send the register address to the compass
    TWDR = registerAddress;

    //Clear Interrupt
    TW_ClearInterrupt();

    //check if the transmission of the register location was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_DATA_W_ACK){
        return 4;
    }

    TW_Stop();
    return 1;

}

int TW_Start(){
    
    //START Command
    //CLEARMASK((1 << TWSTO), TWCR);
    //SETMASK((1 << TWINT) | (1 << TWEN) | (1 << TWSTA), TWCR);
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

    //Wait for TWINT
    while(!(TWCR & (1<<TWINT))){
        //how does this exit in case of an error?
    }

    //check if start condition was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) == I2C_STATUS_START || (TWSR & 0xF8) == I2C_STATUS_REPEAT_START){
        return 1;
    }

    return 0;

}

int TW_RepeatedStart(){
    
    //START Command
    //CLEARMASK((1 << TWSTO), TWCR);
    //SETMASK((1 << TWINT) | (1 << TWEN) | (1 << TWSTA), TWCR);
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

    //Wait for TWINT
    while(!(TWCR & (1<<TWINT))){
        //how does this exit in case of an error?
    }

    //check if the repeated start condition was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_REPEAT_START){
        return 0;
    }

    return 1;

}

int TW_Stop(){
    //STOP command
    //SETMASK((1 << TWINT) | (1 << TWEN) | (1 << TWSTO), TWCR); //1001 0100
    //CLEARMASK((1 << TWSTA) | (1 << 0x02), TWCR); 
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    return 1;
}

int TW_ClearInterrupt(){
    //Clear Inturrupt
    //SETMASK((1 << TWINT) | (1 << TWEN), TWCR);
    //CLEARMASK((1 << TWSTA) | (1 << TWSTO) | (1 << 0x02), TWCR);
    TWCR = (1 << TWINT) | (1 << TWEN);
    //wait for TWINT
    while(!(TWCR & (1<<TWINT))){

    }
    return 1;
}

//This lower section will not be used

//////////Testing Functions/////////////
int I2C_MasterRegisterReceivePt1(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms){
    
    uint16_t i;
    //unsigned char user_input = 'b'; //for testing

    //START Command
    if(TW_Start() == 0){
        return 2;
    }

    //Set deviceAddress to SLA+W
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x00 & 0x01); 
    
    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+W was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 3;
    }

    //send the register address to the compass
    TWDR = registerAddress;

    //Clear Interrupt
    TW_ClearInterrupt();

    //check if the transmission of the register location was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR); 
    if((TWSR & 0xF8) != I2C_STATUS_DATA_W_ACK){
        return 4;
    }
    return 1;
}

int I2C_MasterRegisterReceivePt2(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms){
    
    uint16_t i;
    //run a repeated start
    if(TW_RepeatedStart() == 0){
        //return 5;
    };

    //Set deviceAddress to SLA+R
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x01 & 0x01);

    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+R was acknowledged
    CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR);
    if((TWSR & 0xF8) != I2C_STATUS_START_R_ACK){
        //return 6;
    }

    for(i = 0x00; i < size; i++){
        //if((TWSR & 0xF8) == I2C_STATUS_DATA_R_NACK){
            //Record data from the TWDR
            pData[i] = TWDR;

            //Clear Inturrupt
            TW_ClearInterrupt();

            //check if MT of SLA+W was acknowledged
            CLEARMASK((1 << TWPS0) | (1 << TWPS1), TWSR);
            if((TWSR & 0xF8) != I2C_STATUS_DATA_R_ACK && (TWSR & 0xF8) != I2C_STATUS_DATA_R_NACK){
                //return 7;
            }
        
        //}

    }

    //TW_Stop();
    return 1;
}
/*

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
        
         // Switch to transmitting data
         
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
            
             // Still have data to transmit, transmit
             
            TWDR = I2C_Desc->pData[I2C_Desc->dataIdx++];
            I2C_Desc->state = I2C_STATE_DATA_W;
            SETBIT(TWINT, TWCR);
        }
        else
        {
            
             // Done transmitting.  If ReadReg, repeat start, otherwise stop
             
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

*/