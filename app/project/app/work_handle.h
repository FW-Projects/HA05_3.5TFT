#ifndef _WORK_HANDLE_H
#define _WORK_HANDLE_H
#include "at32f415.h"


#define WORK_HANDLE_TIME      200
#define BEEP_LONG             2
#define BEEP_SHORT            1
#define ERROR_TIME_OUT        15  
typedef enum
{
    INIT = 0,
    STOP,
    WORKING,
	COLD_MODE,
	OVER_TEMP_ERR,
	LOW_TEMP_ERR,
    SLEEP,
} work_state_t;

typedef struct
{
    work_state_t state;
    uint16_t sleep_time;
	uint16_t error_time;
    uint8_t beep_open;
} work_handle_t;

extern work_handle_t swork;
void WorkProc(void);

#endif
