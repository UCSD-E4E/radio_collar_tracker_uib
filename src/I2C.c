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

    CLEARBIT(PRTWI, PRR0); //Power reduction register to enable TWI 7.9.2
    SETBIT(PORTD0, PORTD); // read thru 10.2.1
    SETBIT(PORTD1, PORTD); // PORT Pins set to 1 activate their pullup resistor. PORTD0 and PORTD1 are SCL and SDA

    TWBR = 18;
    
    SETBIT(TWPS0, TWSR);
    CLEARBIT(0x03, TWSR);

    SETMASK((1 << TWEN) | (1 << TWIE) | (1 << TWEA) | (1 << TWINT), TWCR); 

    return 1;
}


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
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x00 & 0x01);
    
    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+W was acknowledged
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 0;
    }

    for(i = 0x00; i < size; i++){
        
        //load TWDR with the current data message
        TWDR = pData[i];

        //Clear Inturrupt
        TW_ClearInterrupt();

        //check if MT of Data was acknowledged
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
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 3;
    }

    //send the register address to the compass
    TWDR = registerAddress;

    //Clear Interrupt
    TW_ClearInterrupt();

    //check if the transmission of the register location was acknowledged
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
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 6;
    }

    for(i = 0x00; i < size; i++){ 
        
        //load TWDR with the current data message
        TWDR = pData[i];

        //Clear Interrupt
        TW_ClearInterrupt();

        //check if MT of Data was acknowledged
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
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x01 & 0x01);

    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+R was acknowledged
    if((TWSR & 0xF8) != I2C_STATUS_START_R_ACK){
        return 6;
    }

    //Clear Inturrupt
    //TW_ClearInterrupt();
    for(i = 0x00; i < size; i++){
        if(i == (size - 1)){
            pData[i] = I2C_ReadNack();
        }else{
            pData[i] = I2C_ReadAck();
        }

    }

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

    //START Command
    if(TW_Start() == 0){
        return 2;
    }

    //Set deviceAddress to SLA+W
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x00 & 0x01); 
    
    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+W was acknowledged
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 3;
    }

    //send the register address to the compass
    TWDR = registerAddress;

    //Clear Interrupt
    TW_ClearInterrupt();

    //check if the transmission of the register location was acknowledged
     
    if((TWSR & 0xF8) != I2C_STATUS_DATA_W_ACK){
        return 4;
    }

    //run a repeated start
    if(TW_RepeatedStart() == 0){
        return 5;
    };

    //Set deviceAddress to SLA+R
    TWDR = ((deviceAddress << 1) & 0xFEu) | (0x01 & 0x01);

    //Clear Inturrupt
    TW_ClearInterrupt();

    //check if MT of SLA+R was acknowledged
    if((TWSR & 0xF8) != I2C_STATUS_START_R_ACK){
        return 6;
    }

    for(i = 0x00; i < size; i++){
        if(i == (size - 1)){
            pData[i] = I2C_ReadNack();
        }else{
            pData[i] = I2C_ReadAck();
        }

    }

    TW_Stop();
    return 1;
}
/**
 * Set Register Function (For Compass)
 * Writes the pointer value for the compass' register select
 * @return  1 if successful, otherwise 0
 */
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
    if((TWSR & 0xF8) != I2C_STATUS_START_W_ACK){
        return 3;
    }

    //send the register address to the compass
    TWDR = registerAddress;

    //Clear Interrupt
    TW_ClearInterrupt();

    //check if the transmission of the register location was acknowledged 
    if((TWSR & 0xF8) != I2C_STATUS_DATA_W_ACK){
        return 4;
    }

    return 1;

    

}

/******************************************************************************
 * Helper Functions
 ******************************************************************************/

/**
 * Set Start Function
 * Write a Start command to the TWCR and check if it gets acknowledged
 * @return  1 if successful, otherwise 0
 */
int TW_Start(){
    
    //START Command
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

    //Wait for TWINT
    while(!(TWCR & (1<<TWINT))){
        //how does this exit in case of an error?
    }

    //check if start condition was acknowledged
    if(((TWSR & 0xF8) != I2C_STATUS_START) && ((TWSR & 0xF8) != I2C_STATUS_REPEAT_START)){
        return 0;
    }

    return 1;

}
/**
 * Set Repeated Start Function
 * Write a Repeated Start command to the TWCR and check if it gets acknowledged
 * @return  1 if successful, otherwise 0
 */
int TW_RepeatedStart(){
    
    //START Command
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);

    //Wait for TWINT
    while(!(TWCR & (1<<TWINT))){
        //how does this exit in case of an error?
    }

    //check if the repeated start condition was acknowledged

    if((TWSR & 0xF8) != I2C_STATUS_REPEAT_START){
        return 0;
    }

    return 1;

}
/**
 * Set Stop Function
 * Write a Stop command to the TWCR
 * @return  1 if successful, otherwise 0
 */
int TW_Stop(){
    //STOP command

    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    return 1;
}
/**
 * Clear Interrupt Function
 *Clear the interrupt bit on the TWCR
 * @return  1 if successful, otherwise 0
 */
int TW_ClearInterrupt(){
    //Clear Inturrupt

    TWCR = (1 << TWINT) | (1 << TWEN);
    //wait for TWINT
    while(!(TWCR & (1<<TWINT)));
    return 1;
}

/**
 * Set Acknowledge Function
 * Set the TWCR to acknowledge a read
 * @return  1 if successful, otherwise 0
 */
uint8_t I2C_ReadAck(){
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    //wait for TWINT
    while(!(TWCR & (1<<TWINT)));
    return TWDR;
}
/**
 * Set Not Acknowledge Function
 * Set the TWCR to a "not acknowledge", either for an incorrect Status, or for the last bit being read during a read operation
 * @return  1 if successful, otherwise 0
 */
uint8_t I2C_ReadNack(){
    TWCR = (1 << TWINT) | (1 << TWEN);
    //wait for TWINT
    while(!(TWCR & (1<<TWINT)));
    return TWDR;
}

