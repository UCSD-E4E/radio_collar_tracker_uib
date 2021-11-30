#ifndef __UIB_H__
#define __UIB_H__

#include "serial.h"
typedef struct HAL_System_
{
    SerialDesc_t* pOBC;
    SerialDesc_t* pGPS;
}HAL_System_t;
extern HAL_System_t* pHalSystem;
#endif