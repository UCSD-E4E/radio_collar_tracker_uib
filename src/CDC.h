/*
 * CDC.h
 *
 *  Created on: Aug 30, 2020
 *      Author: ntlhui
 */

#ifndef CDC_H_
#define CDC_H_

#include <stdint.h>
#include "USBCore.h"

int USBSerial_Read(uint8_t* pBuf, uint32_t len);
int USBSerial_Write(uint8_t* pBuf, uint32_t len);
int CDC_Setup(USBSetup* setup);
int CDC_GetInterface(uint8_t* interfaceNum);


#endif /* CDC_H_ */
