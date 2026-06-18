#ifndef _KEY_HANDLE_H
#define _KEY_HANDLE_H
#include "key.h"
#include "lcd_handle.h"
#define KEY_HANDLE_TIME          30
#define HEL_TIME                 90

typedef enum
{
    TEMP_ADD_EVENT = 0,
    TEMP_ADD_FIVE_EVENT,
    TEMP_REDUCE_EVENT,
    TEMP_REDUCE_FIVE_EVENT,
    WIND_ADD_EVENT,
    WIND_ADD_FIVE_EVENT,
    WIND_REDUCE_EVENT,
    WIND_REDUCE_FIVE_EVENT,
	COLD_WIND_ADD_FIVE,
	COLD_WIND_ADD,
	COLD_WIND_REDUCE,
	COLD_WIND_REDUCE_FIVE,
    END_EVENT,

} handle_event;

extern uint8_t adjust_state;

void KeyProc(void);
void key_handle(void);
#endif
