#ifndef _OUTPUT_HANDLE_H
#define _OUTPUT_HANDLE_H
#include "HA01_handle.h"
#define OUTPUT_HANDLE_TIME 5
#define RELAY_OPEN_TIME 60

void output_handle(void);
void pwm_control(HA01_Handle *this);
 void fan_control(HA01_Handle *this);
#endif
