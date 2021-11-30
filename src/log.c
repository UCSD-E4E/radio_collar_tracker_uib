#include "log.h"
#include <string.h>
#include <stdio.h>
#include "timerfile.h"
#include "uib.h"

const char* LOG_PriorityMap[] = 
{
    "CRITICAL",
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG",
    NULL
};

const char* LOG_SubsystemMap[] = 
{
    "NMEA",
    "SYSTEM",
    NULL
};

LOG_t LOG_instance;

int LOG_FilterInit(LOG_Filter_t* pDesc);

int LOG_FilterApply(LOG_SUBSYSTEM_e subsystem, LOG_PRIORITY_e msgPriority);

int Log_Open(SerialDesc_t* pEndpoint)
{
    LOG_instance.currentEndpoint = pEndpoint;
    LOG_instance.initialized = 1;
    if(LOG_FilterInit(&LOG_instance.filters))
    {
        return 1;
    }
    LOG_instance.lineCounter = 0;
    return 0;
}

int Log_Write(LOG_PRIORITY_e priority, LOG_SUBSYSTEM_e subsystem, 
    const char* file, uint32_t line, const char* fmt, ...)
{
    int pass;
    uint32_t now;
    const char* priorityStr;
    const char* moduleStr;
    va_list vargs;
    size_t printIdx;

    if(!LOG_instance.initialized)
    {
        Serial_Printf(pHalSystem->pOBC, "Log not initialized\n");
        return 0;
    }

    pass = LOG_FilterApply(subsystem, priority);
    switch(pass)
    {
        case -1:
            return 1;
        case 0:
            return 0;
        default:
            break;
    }

    // filter passes, we should print
    now = LOG_instance.lineCounter;
    LOG_instance.lineCounter += 1;
    priorityStr = LOG_PriorityMap[priority];
    moduleStr = LOG_SubsystemMap[subsystem];
    printIdx = snprintf(LOG_instance.buffer, LOG_BUFFER_LEN, "%lu - %s - %s - %s:%lu:", now, moduleStr, priorityStr, file, line);
    if(printIdx > LOG_BUFFER_LEN)
    {
        return 1;
    }
    va_start(vargs, fmt);
    printIdx += vsnprintf(LOG_instance.buffer + printIdx, LOG_BUFFER_LEN - printIdx, fmt, vargs);
    va_end(vargs);
    printIdx += snprintf(LOG_instance.buffer + printIdx, LOG_BUFFER_LEN - printIdx, "\n");
    Serial_Write(LOG_instance.currentEndpoint, LOG_instance.buffer, printIdx);

    return 0;
}

int Log_Close()
{
    LOG_instance.initialized = 0;
    return 0;
}

int Log_SetPriority(LOG_PRIORITY_e priority, LOG_SUBSYSTEM_e subsystem);

int LOG_FilterInit(LOG_Filter_t* pFilter)
{
    LOG_FilterInstance_t* pNode;
    size_t i;
    if(NULL == pFilter)
    {
        return 1;
    }
    pFilter->globalPriority = LOG_DEFAULT_PRIORITY;

    // Initialize filter instances;
    memset(pFilter->filters, 0, sizeof(LOG_FilterInstance_t) * LOG_NUM_FILTERS);
    pNode = &pFilter->filters[0];
    pFilter->pFree = pNode;
    for(i = 1; i < LOG_NUM_FILTERS-1; i++)
    {
        pNode->pNext = &pFilter->filters[i];
        pNode = pNode->pNext;
    }
   pNode->pNext = NULL;

    // Initialize active list
    pFilter->pActive = NULL;

    // Initialize flag
    pFilter->initialized = 1;
    return 0;
}

int LOG_FilterApply(LOG_SUBSYSTEM_e subsystem, LOG_PRIORITY_e msgPriority)
{
    LOG_FilterInstance_t* pNode;
    int found = 0;
    int pass = 1;
    size_t i;
    if(!LOG_instance.initialized)
    {
        return -1;
    }
    if(!LOG_instance.filters.initialized)
    {
        return -1;
    }
    
    for(i = 0; i < LOG_NUM_FILTERS; i++)
    {
        pNode = &LOG_instance.filters.filters[i];
        if(!pNode->initialized)
        {
            continue;
        }
        if(pNode->subsystem == subsystem)
        {
            found = 1;
            if(msgPriority > pNode->priority)
            {
                pass = 0;
            }
        }
    }
    if(!found)
    {
        if(msgPriority > LOG_instance.filters.globalPriority)
        {
            pass = 0;
        }
    }

    return pass;
}