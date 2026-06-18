#ifndef __DISPLAY_HANDLE_H
#define __DISPLAY_HANDLE_H
#include "lcd.h"
#include "lcd_init.h"
#include "at32_spiflash.h"

#define DISPLAY_TASK_TIME 1
extern int8_t actual_refesh_time;

typedef struct
{
    uint16_t x1;
    uint16_t y1;
    uint16_t length;
    uint16_t winth;
} icon_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
    float *num;
    uint8_t len;
    uint16_t fc;
    uint16_t bc;
    uint8_t sizey;
    uint8_t mode;
} number;

typedef enum
{
    SHOW_PAGE,
    SHOW_ICON,

} display_e;

void display_handle(void);
#endif
