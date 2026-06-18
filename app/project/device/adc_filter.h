#ifndef __ADC_FILTER_H
#define __ADC_FILTER_H

#include "at32f415.h"

#define WINDOW_SIZE 10
typedef struct
{
    float input_data[WINDOW_SIZE];
    uint16_t sum;
    uint16_t cur_count;
    adc_channel_select_type adc_channel;
} filter_t;

extern filter_t handle_temp;
u16 get_adcval(adc_channel_select_type adc_channel);
uint32_t move_average_filter(filter_t *filter);
uint8_t filter_init(filter_t *filter, adc_channel_select_type adc_channel);
u16 get_adcval_filtered(adc_channel_select_type adc_channel);
u16 get_adc_filtered(adc_channel_select_type adc_channel);
uint16_t get_adcval_average(adc_channel_select_type adc_channel, uint8_t times);

#endif