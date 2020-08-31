/*
 * product.h
 *
 *  Created on: Aug 29, 2020
 *      Author: ntlhui
 */

#ifndef PRODUCT_H_
#define PRODUCT_H_

#ifdef __AVR_ATmega32U4__
#define E4E_RTT_UIBv2
#elif defined(__unix__)
#define E4E_RTT_SIM
#endif

#endif /* PRODUCT_H_ */
