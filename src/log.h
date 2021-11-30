#ifndef __LOG_H__
#define __LOG_H__
#include "serial.h"
#include <stdarg.h>

#define LOG_NUM_FILTERS 8

#define LOG_DEFAULT_PRIORITY    LOG_PRIORITY_WARNING

#define LOG_BUFFER_LEN  256

#define LOG_CRIT(subsystem, fmt, ...) Log_Write(LOG_PRIORITY_CRITICAL, subsystem, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(subsystem, fmt, ...) Log_Write(LOG_PRIORITY_ERROR, subsystem, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(subsystem, fmt, ...) Log_Write(LOG_PRIORITY_WARNING, subsystem, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(subsystem, fmt, ...) Log_Write(LOG_PRIORITY_INFO, subsystem, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(subsystem, fmt, ...) Log_Write(LOG_PRIORITY_DEBUG, subsystem, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define LOG_WRITE(priority, subsystem, fmt, ...) Log_Write(priority, subsystem, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

typedef enum LOG_PRIORITY_
{
    LOG_PRIORITY_CRITICAL,
    LOG_PRIORITY_ERROR,
    LOG_PRIORITY_WARNING,
    LOG_PRIORITY_INFO,
    LOG_PRIORITY_DEBUG,
    LOG_PRIORITY__NELEMS
}LOG_PRIORITY_e;

typedef enum LOG_SUBSYSTEM_
{
    LOG_SUBSYSTEM_NMEA,
    LOG_SUBSYSTEM_SYSTEM,
    LOG_SUBSYSTEM__NELEMS
}LOG_SUBSYSTEM_e;

typedef struct LOG_FilterInstance_
{
    LOG_SUBSYSTEM_e subsystem;
    LOG_PRIORITY_e priority;
    int initialized;
    struct LOG_FilterInstance_* pNext;
}LOG_FilterInstance_t;

typedef struct LOG_Filter_
{
    LOG_PRIORITY_e globalPriority;
    LOG_FilterInstance_t filters[LOG_NUM_FILTERS];
    LOG_FilterInstance_t* pActive;
    LOG_FilterInstance_t* pFree;
    int initialized;
}LOG_Filter_t;

typedef struct LOG_
{
    LOG_Filter_t filters;
    SerialDesc_t* currentEndpoint;
    char buffer[LOG_BUFFER_LEN + 1];
    uint32_t lineCounter;
    int initialized;
}LOG_t;

int Log_Open(SerialDesc_t* pEndpoint);

int Log_Write(LOG_PRIORITY_e priority, LOG_SUBSYSTEM_e subsystem, const char* file, uint32_t line, const char* fmt, ...);

int Log_Close();

int Log_SetPriority(LOG_PRIORITY_e priority, LOG_SUBSYSTEM_e subsystem);
#endif