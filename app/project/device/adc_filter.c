#include "adc_filter.h"

#include <stdio.h>
#include <string.h>

filter_t handle_temp;

u16 get_adcval(adc_channel_select_type adc_channel);
uint32_t move_average_filter(filter_t *filter);


uint8_t filter_init(filter_t * filter,adc_channel_select_type adc_channel)
{
	if(filter == NULL)
	{
	    return 1;
	}
	
	uint8_t i = 0;
	
	for(i = 0;i<WINDOW_SIZE; i++)
	{
	    filter->input_data[i] = 0;
	}

	filter->adc_channel = adc_channel;
    filter->cur_count = 0;
	filter->sum = 0;
	
  return 0;
}


u16 get_adcval(adc_channel_select_type adc_channel)
{
	adc_flag_clear(ADC1, ADC_CCE_FLAG);
    adc_ordinary_channel_set(ADC1, adc_channel, 1,
                             ADC_SAMPLETIME_239_5);	 
    adc_ordinary_software_trigger_enable(ADC1, TRUE);						 
    while (!adc_flag_get(ADC1, ADC_CCE_FLAG));								 
    return adc_ordinary_conversion_data_get(ADC1);							 
}

uint32_t move_average_filter(filter_t *filter)
{
    if (filter->cur_count < WINDOW_SIZE)
    {
        filter->input_data[filter->cur_count] = get_adcval(filter->adc_channel);
        filter->sum += filter->input_data[filter->cur_count];
        filter->cur_count++;
        return filter->sum / filter->cur_count;
    }
    else
    {
        filter->sum -= filter->sum / WINDOW_SIZE;
        filter->sum += get_adcval(filter->adc_channel);
        return filter->sum / WINDOW_SIZE;
    }
}
    

#define ADC_FILTER_N   16   // 滤波窗口大小（可根据需要调整，建议2的幂次以便优化）

u16 get_adcval_filtered(adc_channel_select_type adc_channel)
{
    static u16 buffer[ADC_FILTER_N] = {0};  // 循环缓冲区
    static u8 index = 0;                    // 当前写入位置
    static u32 sum = 0;                      // 窗口内值的和

    u16 raw = get_adcval(adc_channel);       // 调用原始函数读取一次

    sum -= buffer[index];                    // 减去将被覆盖的旧值
    sum += raw;                               // 加上新值
    buffer[index] = raw;                      // 存入缓冲区

    index = (index + 1) % ADC_FILTER_N;       // 更新索引

    return (u16)(sum / ADC_FILTER_N);         // 返回平均值
}


#define ADC_FILTER_ALPHA  4   // 滤波系数范围0~10，数值越大新值权重越大（响应越快）

u16 get_adc_filtered(adc_channel_select_type adc_channel)
{
    static u16 filtered = 0;          // 上一次滤波值
    static u8 first = 1;               // 首次采样标志

    u16 raw = get_adcval(adc_channel);

    if (first) {
        filtered = raw;                 // 首次直接赋值
        first = 0;
    } else {
        // 公式：filtered = (alpha*raw + (10-alpha)*filtered) / 10
        filtered = (u16)(( (u32)ADC_FILTER_ALPHA * raw + (10 - ADC_FILTER_ALPHA) * filtered ) / 10);
    }

    return filtered;
}

/*
*获取adc平均值
*adc_channel：要获取的通道
*times：总次数
*去掉第一和最后一个数据再取平均值。
*/

uint16_t get_adcval_average(adc_channel_select_type adc_channel, uint8_t times)
{
    uint16_t adc_val[times] ;
    uint32_t adc_valsum = 0;
    uint8_t cut;

    for (cut = 0; cut < times; cut++)							//多次获取adc的值
    {
        adc_val[cut] = get_adcval(adc_channel);
    }

    for (cut = 1; cut < times - 1; cut++)						//去掉第一次和最后一次的值
    {
        adc_valsum += adc_val[cut];
    }

    return adc_valsum / (times - 2);						//返回平均值
}

