/*
 * cutils.c
 *
 *  Created on: Aug 29, 2020
 *      Author: ntlhui
 */

#include <stdint.h>


inline int readDiscardInt(volatile int value)
{
    return value;
}

inline volatile void* readDiscardPointer(volatile void* ptr)
{
    return ptr;
}

uint8_t readDiscardByte(volatile uint8_t value)
{
    return value;
}
