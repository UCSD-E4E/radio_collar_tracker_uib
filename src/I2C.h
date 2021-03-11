/*
 * I2C.h
 *
 *  Created on: Sep 1, 2020
 *      Author: ntlhui
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
/******************************************************************************
 * Includes
 ******************************************************************************/

/******************************************************************************
 * Header Macro Definitions
 ******************************************************************************/

/******************************************************************************
 * Typedefs
 ******************************************************************************/

/******************************************************************************
 * Symbol Prototypes
 ******************************************************************************/
int I2C_Init(void);
int I2C_MasterTransmit(uint8_t deviceAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms);
int I2C_MasterReceive(uint8_t deviceAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms);
int I2C_MasterRegisterReceive(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms);
int I2C_MasterRegisterTransmit(uint8_t deviceAddress, uint8_t registerAddress, uint8_t* pData, uint16_t size, uint32_t timeout_ms);
int TW_Start();
int TW_Stop();
int TW_ClearInterrupt();
#endif /* I2C_H_ */
