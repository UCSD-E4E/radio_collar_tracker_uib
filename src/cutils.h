/*
 * cutils.h
 *
 *  Created on: Aug 29, 2020
 *      Author: ntlhui
 */

#ifndef CUTILS_H_
#define CUTILS_H_

#include <stdint.h>

#define SETBIT(x, n) (n |= (1 << x))
#define GETBIT(x, n) (n & (1 << x))
#define CLEARBIT(x, n) (n &= ~(1 << x))
#define CLEARMASK(MASK, REG) (REG &= ~MASK)
#define SETMASK(MASK, REG) (REG |= MASK)

uint8_t readDiscardByte(volatile uint8_t value);

#endif /* CUTILS_H_ */
