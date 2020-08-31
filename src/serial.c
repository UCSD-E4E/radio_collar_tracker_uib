/*
 * serial.c
 *
 *  Created on: Aug 30, 2020
 *      Author: ntlhui
 */

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "serial.h"

#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "CDC.h"
#include "cutils.h"
#include "LED_module.h"
#include "USBCore.h"
#include "USBDesc.h"
/******************************************************************************
 * Defines
 ******************************************************************************/
#define RX_BUFFER_LEN   256
#define TX_BUFFER_LEN   16

/******************************************************************************
 * Typedefs
 ******************************************************************************/
typedef enum SerialType_
{
    SerialType_USB,
    SerialType_HW,
    SerialType_NULL
}SerialType_e;

typedef struct USBSerial_
{
    uint8_t init;
}USBSerial_t;

typedef struct SerialBuffer_
{
    size_t len;
    size_t mask;
    uint8_t* pBuffer;
    volatile size_t head;
    volatile size_t tail;
}SerialBuffer_t;

typedef struct HWSerial_
{
    SerialBuffer_t rxBuffer;
    SerialBuffer_t txBuffer;
}HWSerial_t;

typedef struct SerialHandle_
{
    SerialDevice_e device;
    void* pDesc;
    SerialType_e type;
}SerialHandle_t;
/******************************************************************************
 * Static Function Prototypes
 ******************************************************************************/
static SerialHandle_t* Serial_getDeviceHandle(SerialDevice_e device);
static void HWSerial_Init(SerialConfig_t* pConfig);
static int HWSerial_Read(HWSerial_t* pDesc, uint8_t* pData, uint32_t len);

static int SerialBuffer_isFull(SerialBuffer_t* pBuffer);
static int SerialBuffer_get(SerialBuffer_t* pBuffer, uint8_t* pData, size_t len);
static int SerialBuffer_isEmpty(SerialBuffer_t* pBuffer);
static int SerialBuffer_put(SerialBuffer_t* pBuffer, uint8_t byte);

/******************************************************************************
 * Module Static Data
 ******************************************************************************/
static USBSerial_t Serial_OBC;
static HWSerial_t Serial_GPS;
static SerialHandle_t SerialDeviceTable[] =
{
    {SerialDevice_GPS, &Serial_GPS, SerialType_HW},
    {SerialDevice_OBC, &Serial_OBC, SerialType_USB},
    {SerialDevice_NULL, NULL, SerialType_NULL}
};

static uint8_t Serial_GPSRxBuffer[RX_BUFFER_LEN];
static uint8_t Serial_GPSTxBuffer[TX_BUFFER_LEN];
/******************************************************************************
 * Global Data
 ******************************************************************************/

/******************************************************************************
 * Function Definitions
 ******************************************************************************/
/**
 * Initializes the Serial descriptor
 * @param  pConfig Serial Configuration
 * @return         1 if successful, otherwise 0
 */
SerialDesc_t* Serial_Init(SerialConfig_t* pConfig)
{
    USBSerial_t* pUSBSerial;
    HWSerial_t* pHWSerial;
    SerialHandle_t* pHandle = Serial_getDeviceHandle(pConfig->device);
    switch(pConfig->device)
    {
    case SerialDevice_OBC:
        USBDevice_attach();
        pUSBSerial->init = 1;
        pUSBSerial = pHandle->pDesc;
        return (void*)pHandle;
    case SerialDevice_GPS:
        HWSerial_Init(pConfig);
        pHWSerial = pHandle->pDesc;
        memset(&pHWSerial->rxBuffer, 0, sizeof(SerialBuffer_t));
        memset(&pHWSerial->txBuffer, 0, sizeof(SerialBuffer_t));
        pHWSerial->rxBuffer.pBuffer = Serial_GPSRxBuffer;
        pHWSerial->txBuffer.pBuffer = Serial_GPSTxBuffer;
        pHWSerial->rxBuffer.len = RX_BUFFER_LEN;
        pHWSerial->rxBuffer.mask = pHWSerial->rxBuffer.len - 1;
        pHWSerial->txBuffer.len = TX_BUFFER_LEN;
        pHWSerial->txBuffer.mask = pHWSerial->txBuffer.len - 1;
        return (void*) pHandle;
    default:
        return NULL;
    }
}

/**
 * Attempts to read data into the provided buffer.  If data is not available,
 * this function shall return 0 immediately.  If there is less than len bytes
 * available, this function shall copy those bytes into pBuf and return the
 * number of bytes copied into pBuf.  If there is more than len bytes available,
 * this function shall copy len bytes into pBuf and return len.
 * @param  pDesc Serial Descriptor
 * @param  pBuf  Pointer to Buffer
 * @param  len   Length of Buffer
 * @return       Number of bytes read
 */
int Serial_Read(SerialDesc_t* pDesc, uint8_t* pBuf, uint32_t len)
{
    SerialHandle_t* pHandle = (SerialHandle_t*)pDesc;
    switch(pHandle->type)
    {
    case SerialType_USB:
        return USBSerial_Read(pBuf, len);
    case SerialType_HW:
        return HWSerial_Read(pHandle->pDesc, pBuf, len);
    default:
        return 0;
    }
}

/**
 * Queues the specified data for transmit.  This function shall copy the
 * data provided into a buffer for transmit.  If more data is specified than can
 * be buffered, this function shall fail.
 * @param  pDesc Serial Descriptor
 * @param  pBuf  Pointer to data
 * @param  len   Length of data
 * @return       1 if successful, otherwise 0
 */
int Serial_Write(SerialDesc_t* pDesc, uint8_t* pBuf, uint32_t len)
{
    SerialHandle_t* pHandle = (SerialHandle_t*) pDesc;
    switch(pHandle->type)
    {
    case SerialType_USB:
        return USBSerial_Write(pBuf, len);
    default:
        return 0;
    }
}

static SerialHandle_t* Serial_getDeviceHandle(SerialDevice_e device)
{
    SerialHandle_t* pHandle = SerialDeviceTable;
    while(pHandle->pDesc)
    {
        if(pHandle->device == device)
        {
            return pHandle;
        }
        pHandle++;
    }
    return NULL;
}

static void HWSerial_Init(SerialConfig_t* pConfig)
{
    cli();
    SETBIT(U2X1, UCSR1A);
    UBRR1 = 16;


    UCSR1B = 0;
    SETBIT(RXEN1, UCSR1B);
    SETBIT(TXEN1, UCSR1B);

    UCSR1C = 0;
    SETBIT(USBS1, UCSR1C);
    SETBIT(UCSZ10, UCSR1C);
    SETBIT(UCSZ11, UCSR1C);

    SETBIT(RXCIE1, UCSR1B);
    SETBIT(UDRIE1, UCSR1B);
    sei();
}

/**
 * Rx Complete Interrupt
 */
ISR(USART1_RX_vect)
{
    if(GETBIT(UPE1, UCSR1A))
    {
        readDiscardByte(UDR1);
        return;
    }
    SerialBuffer_put(&Serial_GPS.rxBuffer, UDR1);
}

/**
 * Tx Data Register Empty vector
 */
ISR(USART1_UDRE_vect)
{

}

static int SerialBuffer_isFull(SerialBuffer_t* pBuffer)
{
    return (pBuffer->head - pBuffer->tail) == pBuffer->len ? 1 : 0;
}

static int SerialBuffer_isEmpty(SerialBuffer_t* pBuffer)
{
    return (pBuffer->head - pBuffer->tail) == 0U ? 1 : 0;
}

static int SerialBuffer_put(SerialBuffer_t* pBuffer, uint8_t byte)
{
    size_t offset;
    if(SerialBuffer_isFull(pBuffer))
    {
        return 0;
    }

    offset = pBuffer->head & pBuffer->mask;
    pBuffer->pBuffer[offset] = byte;
    pBuffer->head++;
    return 1;
}

static int SerialBuffer_get(SerialBuffer_t* pBuffer, uint8_t* pData, size_t len)
{
    size_t nElements;
    size_t offset;
    if(SerialBuffer_isEmpty(pBuffer))
    {
        return 0;
    }
    offset = pBuffer->tail & pBuffer->mask;
    nElements = pBuffer->head - pBuffer->tail;
    if(nElements < len)
    {
        len = nElements;
    }
    memcpy(pData, pBuffer->pBuffer + offset, len);
    pBuffer->tail += len;
    return len;
}

static int HWSerial_Read(HWSerial_t* pDesc, uint8_t* pData, uint32_t len)
{
    return SerialBuffer_get(&pDesc->rxBuffer, pData, len);
}

