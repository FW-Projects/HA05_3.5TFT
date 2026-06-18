#include "key_handle.h"
//#include "work_handle.h"
#include "PID_operation.h"
#include "HA01_handle.h"
#include "beep_handle.h"
#define KEY_COUNT 6

KEY_DEFINE(key_ch1, GPIOC, GPIO_PINS_6, 0);

KEY_DEFINE(key_ch2, GPIOC, GPIO_PINS_7, 0);

KEY_DEFINE(key_ch3, GPIOC, GPIO_PINS_8, 0);

KEY_DEFINE(get_key_handle1, GPIOC, GPIO_PINS_3, 0);

KEY_DEFINE(get_key_handle2, GPIOC, GPIO_PINS_2, 0);

KEY_DEFINE(get_key_handle3, GPIOC, GPIO_PINS_1, 0);

key_obj *keys[KEY_COUNT] = {&key_ch1, &key_ch2, &key_ch3, &get_key_handle1,
                            &get_key_handle2, &get_key_handle3};

key_event_t key_event[KEY_COUNT];
							
static bool set_done = false;
static bool last_key_adjust_state = false;  //按键调节温度或风量标志
static int out_times = 80; 
uint8_t adjust_state = TEMP;
static  handle_event event = END_EVENT;
static bool channel_state = false;
void get_key(void);
void event_handle(void);

void key_handle(void)
{
	get_key();
	event_handle();
	if(last_key_adjust_state == true)
	{
		out_times--;
		if(!out_times)
		{
			out_times = 80;
			last_key_adjust_state = false;
			channel_state = false;
		}
	}
}
							
							
void get_key(void)
{
    
    static uint8_t delay_time = 5;
    static uint16_t hel1_time = HEL_TIME;
    static uint8_t hel1_flag = FALSE;
    static uint8_t hel1_mode = 0;   //0:set temp  1:set wind
    static uint8_t set_done = FALSE;
	static bool first_in = false;

	if (key_event[0] == KEY_NONE &&
            key_event[1] == KEY_NONE &&
            key_event[2] == KEY_NONE &&
            key_event[3] == KEY_NONE &&
            key_event[4] == KEY_NONE &&
            key_event[5] == KEY_NONE)
    {
        set_done = false;
    }
    if (first_in == false)
    {
        first_in = true;
        key_set_scan_interval(KEY_HANDLE_TIME);
        key_set_timing_simple(&key_ch1, KEY_HANDLE_TIME, 800, 0, 0);
        key_set_timing_simple(&key_ch2, KEY_HANDLE_TIME, 800, 0, 0);
        key_set_timing_simple(&key_ch3, KEY_HANDLE_TIME, 800, 0, 0);
        key_set_timing_simple(&get_key_handle1, KEY_HANDLE_TIME, 800, 0, 100);
        key_set_timing_simple(&get_key_handle2, KEY_HANDLE_TIME, 800, 0, 100);
        key_set_timing_simple(&get_key_handle3, KEY_HANDLE_TIME, 800, 0, 0);
    }
	
	for (int i = 0; i < KEY_COUNT; i++)
    {
        key_event[i] = key_event_check(keys[i]);
    }
	
	
	switch (key_event[0])
    {
    case KEY_SHORT_PRESS:
		if(sFWHA01_t.run_mode != Cold_Mode)
		{
			set_done = true;
			sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch1_set_temp;
			sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch1_set_air; 
			sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
			sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
			sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
			sFWHA01_t.system_parameter.last_set_temp_f_display = RESET_VALUE;
			sFWHA01_t.general_parameter.ch = 1;
			sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			sbeep.cmd = BEEP_SHORT;
		}
        break;

    case KEY_LONG_PRESS:
        set_done = true;
		if(sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE)
		{
				if(sFWHA01_t.temp_unit == FAHRENHEIT)
			{
				sFWHA01_t.system_parameter.ch1_set_temp = (sFWHA01_t.system_parameter.set_temp_f_display - 32) * 5 / 9;
				sFWHA01_t.system_parameter.ch1_set_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display;
			}
			else
				sFWHA01_t.system_parameter.ch1_set_temp = sFWHA01_t.system_parameter.set_temp;
			sFWHA01_t.system_parameter.ch1_set_air = sFWHA01_t.system_parameter.air_data; 
			sFWHA01_t.general_parameter.ch = 1;
			sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
			sFWHA01_t.general_parameter.save_ch_flag = true;
			sbeep.cmd = BEEP_LONG;
		}
	break;

    default:
        break;
    }
	
	switch (key_event[1])
    {
    case KEY_SHORT_PRESS:
		if(sFWHA01_t.run_mode != Cold_Mode)
		{
			set_done = true;
			sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch2_set_temp;
			sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch2_set_air; 
			sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
			sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
			sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
			sFWHA01_t.system_parameter.last_set_temp_f_display = RESET_VALUE;
			sFWHA01_t.general_parameter.ch = 2;
			sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			sbeep.cmd = BEEP_SHORT;
		}
        break;

    case KEY_LONG_PRESS:
        set_done = true;
		if(sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE)
		{
			if(sFWHA01_t.temp_unit == FAHRENHEIT)
			{
				sFWHA01_t.system_parameter.ch2_set_temp = (sFWHA01_t.system_parameter.set_temp_f_display - 32) * 5 / 9;
				sFWHA01_t.system_parameter.ch2_set_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display;
			}
			else
				sFWHA01_t.system_parameter.ch2_set_temp = sFWHA01_t.system_parameter.set_temp;
			sFWHA01_t.system_parameter.ch2_set_air = sFWHA01_t.system_parameter.air_data; 
			sFWHA01_t.general_parameter.ch = 2;
			sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
			sFWHA01_t.general_parameter.save_ch_flag = true;
			sbeep.cmd = BEEP_LONG;
		}
	break;

    default:
        break;
    }
	
	switch (key_event[2])
    {
    case KEY_SHORT_PRESS:
		if(sFWHA01_t.run_mode != Cold_Mode)
		{
			set_done = true;
			sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch3_set_temp;
			sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch3_set_air; 
			sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
			sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
			sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
			sFWHA01_t.system_parameter.last_set_temp_f_display = RESET_VALUE;
			sFWHA01_t.general_parameter.ch = 3;
			sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			sbeep.cmd = BEEP_SHORT;
		}
        break;

    case KEY_LONG_PRESS:
        set_done = true;
		if(sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE)
		{
			if(sFWHA01_t.temp_unit == FAHRENHEIT)
			{
				sFWHA01_t.system_parameter.ch3_set_temp = (sFWHA01_t.system_parameter.set_temp_f_display - 32) * 5 / 9;
				sFWHA01_t.system_parameter.ch3_set_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display;
			}
			else
				sFWHA01_t.system_parameter.ch3_set_temp = sFWHA01_t.system_parameter.set_temp;
			sFWHA01_t.system_parameter.ch3_set_air = sFWHA01_t.system_parameter.air_data; 
			sFWHA01_t.general_parameter.ch = 3;
			sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
			sFWHA01_t.general_parameter.save_ch_flag = true;
			sbeep.cmd = BEEP_LONG;
		}
	break;

    default:
        break;
    }
	
	switch (key_event[3])
    {
    case KEY_SHORT_PRESS:
        set_done = true;
		sbeep.cmd = BEEP_SHORT;
		if(sFWHA01_t.run_mode != Cold_Mode)
		{
			if(sFWHA01_t.adjust_key_mode == SELECT_TEMP)
			{	
				if(channel_state == true)
				{
					if(adjust_state == TEMP)
					{
						event = TEMP_ADD_EVENT;
					}
					else if(adjust_state == WIND)
					{
						event = WIND_ADD_EVENT;
					}
				}
				else
				{
					adjust_state = TEMP;
					event = TEMP_ADD_EVENT;
				}
			}
			else if(sFWHA01_t.adjust_key_mode == SELECT_WIND)
			{
				if(channel_state == true)
				{
					if(adjust_state == TEMP)
					{
						event = TEMP_ADD_EVENT;
					}
					else if(adjust_state == WIND)
					{
						event = WIND_ADD_EVENT;
					}
				}
				else
				{
					adjust_state = WIND;
					event = WIND_ADD_EVENT;
				}
			}
			else if(sFWHA01_t.adjust_key_mode == SELECT_CH)
			{
				if(sFWHA01_t.run_mode != Cold_Mode)
				{
					sFWHA01_t.general_parameter.ch++;
					if(sFWHA01_t.general_parameter.ch > 3)
						sFWHA01_t.general_parameter.ch = 1;
					if(sFWHA01_t.general_parameter.ch == 1)
					{
						sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch1_set_temp;
						sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch1_set_air;
						sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
					}
					else if(sFWHA01_t.general_parameter.ch == 2)
					{
						sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch2_set_temp;
						sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch2_set_air; 
						sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
					}
					else if(sFWHA01_t.general_parameter.ch == 3)
					{
						sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch3_set_temp;
						sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch3_set_air; 
						sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
					}
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
					sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
					sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_set_temp_f_display = RESET_VALUE;
					sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
					sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
				}
			}
		}
		else
			event = COLD_WIND_ADD;

        break;

    case KEY_LONG_PRESS_CONTINUE:
        set_done = true;
		sbeep.cmd = BEEP_SHORT;
		if(sFWHA01_t.run_mode != Cold_Mode)
		{
			if(sFWHA01_t.adjust_key_mode == SELECT_TEMP)
			{	
				if(channel_state == true)
				{
					if(adjust_state == TEMP)
					{
						event = TEMP_ADD_FIVE_EVENT;
					}
					else if(adjust_state == WIND)
					{
						event = WIND_ADD_FIVE_EVENT;
					}
				}
				else
				{
					adjust_state = TEMP;
					event = TEMP_ADD_FIVE_EVENT;
				}
			}
			else if(sFWHA01_t.adjust_key_mode == SELECT_WIND)
			{
				if(channel_state == true)
				{
					if(adjust_state == TEMP)
					{
						event = TEMP_ADD_FIVE_EVENT;
					}
					else if(adjust_state == WIND)
					{
						event = WIND_ADD_FIVE_EVENT;
					}
				}
				else
				{
					adjust_state = WIND;
					event = WIND_ADD_FIVE_EVENT;
				}
			}
		}
		else
			event = COLD_WIND_ADD_FIVE;	
	break;

    default:
        break;
    }
	
	switch (key_event[4])
    {
    case KEY_SHORT_PRESS:
        set_done = true;
		sbeep.cmd = BEEP_SHORT;
		if(sFWHA01_t.run_mode != Cold_Mode)
		{
			if(sFWHA01_t.adjust_key_mode == SELECT_TEMP)
			{	
				if(channel_state == true)
				{
					if(adjust_state == TEMP)
					{
						event = TEMP_REDUCE_EVENT;
					}
					else if(adjust_state == WIND)
					{
						event = WIND_REDUCE_EVENT;
					}
				}
				else
				{
					adjust_state = TEMP;
					event = TEMP_REDUCE_EVENT;
				}
			}
			else if(sFWHA01_t.adjust_key_mode == SELECT_WIND)
			{
				if(channel_state == true)
				{
					if(adjust_state == TEMP)
					{
						event = TEMP_REDUCE_EVENT;
					}
					else if(adjust_state == WIND)
					{
						event = WIND_REDUCE_EVENT;
					}
				}
				else
				{
					adjust_state = WIND;
					event = WIND_REDUCE_EVENT;
				}
			}
			else if(sFWHA01_t.adjust_key_mode == SELECT_CH)
			{
				if(sFWHA01_t.run_mode != Cold_Mode)
				{
					if(sFWHA01_t.general_parameter.ch == 0)
						sFWHA01_t.general_parameter.ch = 3;
					else
					{
						sFWHA01_t.general_parameter.ch--;
						if(sFWHA01_t.general_parameter.ch < 1)
							sFWHA01_t.general_parameter.ch = 3;
					}
					if(sFWHA01_t.general_parameter.ch == 1)
					{
						sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch1_set_temp;
						sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch1_set_air;
						sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
					}
					else if(sFWHA01_t.general_parameter.ch == 2)
					{
						sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch2_set_temp;
						sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch2_set_air; 
						sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
					}
					else if(sFWHA01_t.general_parameter.ch == 3)
					{
						sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch3_set_temp;
						sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch3_set_air; 
						sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
					}
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
					sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
					sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_set_temp_f_display = RESET_VALUE;
					sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
					sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
				}
			}
		}
		else
			event = COLD_WIND_REDUCE;
        break;

    case KEY_LONG_PRESS_CONTINUE:
        set_done = true;
		sbeep.cmd = BEEP_SHORT;
		if(sFWHA01_t.run_mode != Cold_Mode)
		{
			if(sFWHA01_t.adjust_key_mode == SELECT_TEMP)
			{	
				if(channel_state == true)
				{
					if(adjust_state == TEMP)
					{
						event = TEMP_REDUCE_FIVE_EVENT;
					}
					else if(adjust_state == WIND)
					{
						event = WIND_REDUCE_FIVE_EVENT;
					}
				}
				else
				{
					adjust_state = TEMP;
					event = TEMP_REDUCE_FIVE_EVENT;
				}
			}
			else if(sFWHA01_t.adjust_key_mode == SELECT_WIND)
			{
				if(channel_state == true)
				{
					if(adjust_state == TEMP)
					{
						event = TEMP_REDUCE_FIVE_EVENT;
					}
					else if(adjust_state == WIND)
					{
						event = WIND_REDUCE_FIVE_EVENT;
					}
				}
				else
				{
					event = WIND_REDUCE_FIVE_EVENT;
					adjust_state = WIND;
				}
			}
		}
		else
		{
			event = COLD_WIND_REDUCE_FIVE;
		}
	break;
    default:
        break;
    }
	
	switch (key_event[5])
    {
    case KEY_SHORT_PRESS:
        set_done = true;
		sbeep.cmd = BEEP_SHORT;
		if(sFWHA01_t.short_key_mode == CHANNEL_SWITCHING)
		{
			if(last_key_adjust_state == true)
			{
				out_times = 80;
				channel_state = true;
				
				if(sFWHA01_t.adjust_key_mode == SELECT_TEMP)
				{
					adjust_state++;
					if(adjust_state >= 2)
					{
						last_key_adjust_state = false;
						channel_state = false;
					}
				}
				else if(sFWHA01_t.adjust_key_mode == SELECT_WIND)
				{
					if(adjust_state == WIND)
						adjust_state = TEMP;
					else if(adjust_state == TEMP)
					{
						adjust_state = 2;
						last_key_adjust_state = false;
						channel_state = false;
					}
				}
					
				
				if(adjust_state == TEMP)
				{
					actual_wind_refesh_time = 0x00;
					sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
					sFWHA01_t.general_parameter.set_wind_time = 0x00;
					sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_set_temp_f_display = RESET_VALUE;
					sFWHA01_t.system_parameter.last_air_data_actual = RESET_VALUE;
				}
				else if(adjust_state == WIND)
				{
					actual_temp_refesh_time = 0x00;
					sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
					sFWHA01_t.general_parameter.set_temp_time = 0x00;
					sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp_f_display = RESET_VALUE;
				}
				else
				{
					sFWHA01_t.general_parameter.set_temp_time = 0x00;
					sFWHA01_t.general_parameter.set_wind_time = 0x00;
					sFWHA01_t.system_parameter.last_air_data_actual = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp_f_display = RESET_VALUE;
				}
			}
			else
			{
				if(sFWHA01_t.run_mode != Cold_Mode)
				{
					sFWHA01_t.general_parameter.ch++;
					if(sFWHA01_t.general_parameter.ch > 3)
						sFWHA01_t.general_parameter.ch = 1;
					if(sFWHA01_t.general_parameter.ch == 1)
					{
						sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch1_set_temp;
						sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch1_set_air;
						sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
					}
					else if(sFWHA01_t.general_parameter.ch == 2)
					{
						sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch2_set_temp;
						sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch2_set_air; 
						sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
					}
					else if(sFWHA01_t.general_parameter.ch == 3)
					{
						sFWHA01_t.system_parameter.set_temp = sFWHA01_t.system_parameter.ch3_set_temp;
						sFWHA01_t.system_parameter.air_data = sFWHA01_t.system_parameter.ch3_set_air; 
						sFWHA01_t.system_parameter.set_temp_f_display = (sFWHA01_t.system_parameter.set_temp * 9 / 5) + 32;
					}
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
					sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
					sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_set_temp_f_display = RESET_VALUE;
					sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
					sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
				}
			}
		}
		else if(sFWHA01_t.short_key_mode == COLDWIND_MODE)
		{
			if(last_key_adjust_state == true)
			{
				out_times = 80;
				channel_state = true;
				
				if(sFWHA01_t.adjust_key_mode == SELECT_TEMP)
				{
					adjust_state++;
					if(adjust_state >= 2)
					{
						last_key_adjust_state = false;
						channel_state = false;
					}
				}
				else if(sFWHA01_t.adjust_key_mode == SELECT_WIND)
				{
					if(adjust_state == WIND)
						adjust_state = TEMP;
					else if(adjust_state == TEMP)
					{
						adjust_state = 2;
						last_key_adjust_state = false;
						channel_state = false;
					}
				}
					
				
				if(adjust_state == TEMP)
				{
					actual_wind_refesh_time = 0x00;
					sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
					sFWHA01_t.general_parameter.set_wind_time = 0x00;
					sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_set_temp_f_display = RESET_VALUE;
					sFWHA01_t.system_parameter.last_air_data_actual = RESET_VALUE;
				}
				else if(adjust_state == WIND)
				{
					actual_temp_refesh_time = 0x00;
					sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
					sFWHA01_t.general_parameter.set_temp_time = 0x00;
					sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp_f_display = RESET_VALUE;
				}
				else
				{
					sFWHA01_t.general_parameter.set_temp_time = 0x00;
					sFWHA01_t.general_parameter.set_wind_time = 0x00;
					sFWHA01_t.system_parameter.last_air_data_actual = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp_f_display = RESET_VALUE;
				}
			}
			else
			{	
				if(sFWHA01_t.run_mode == Standard_Mode || sFWHA01_t.run_mode == Power_Mode)
				{
					sFWHA01_t.before_cold_run_mode = sFWHA01_t.run_mode;
					sFWHA01_t.run_mode = Cold_Mode;
				}
				else if(sFWHA01_t.run_mode == Cold_Mode)
				{
					sFWHA01_t.run_mode = sFWHA01_t.before_cold_run_mode;
	//				sFWHA01_t.run_mode = Standard_Mode;
				}
				sFWHA01_t.system_parameter.last_cold_mode_set_air = 0x00;
				sFWHA01_t.system_parameter.last_curve_cold_mode_air_data = 0x00;
				sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			}
		}
		else if(sFWHA01_t.short_key_mode == POWER_MODE)
		{
			if(last_key_adjust_state == true)
			{
				out_times = 80;
				channel_state = true;
				
				if(sFWHA01_t.adjust_key_mode == SELECT_TEMP)
				{
					adjust_state++;
					if(adjust_state >= 2)
					{
						last_key_adjust_state = false;
						channel_state = false;
					}
				}
				else if(sFWHA01_t.adjust_key_mode == SELECT_WIND)
				{
					if(adjust_state == WIND)
						adjust_state = TEMP;
					else if(adjust_state == TEMP)
					{
						adjust_state = 2;
						last_key_adjust_state = false;
						channel_state = false;
					}
				}
					
				if(adjust_state == TEMP)
				{
					actual_wind_refesh_time = 0x00;
					sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
					sFWHA01_t.general_parameter.set_wind_time = 0x00;
					sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_set_temp_f_display = RESET_VALUE;
					sFWHA01_t.system_parameter.last_air_data_actual = RESET_VALUE;
				}
				else if(adjust_state == WIND)
				{
					actual_temp_refesh_time = 0x00;
					sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
					sFWHA01_t.general_parameter.set_temp_time = 0x00;
					sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp_f_display = RESET_VALUE;
				}
				else
				{
					sFWHA01_t.general_parameter.set_temp_time = 0x00;
					sFWHA01_t.general_parameter.set_wind_time = 0x00;
					sFWHA01_t.system_parameter.last_air_data_actual = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp = RESET_VALUE;
					sFWHA01_t.system_parameter.last_actual_temp_f_display = RESET_VALUE;
				}
			}
			else
			{
				if(sFWHA01_t.run_mode == Cold_Mode)
				{
					if(sFWHA01_t.before_cold_run_mode == Power_Mode)
					{
						sFWHA01_t.before_cold_run_mode = Standard_Mode;
					}
					else if(sFWHA01_t.before_cold_run_mode == Standard_Mode)
						sFWHA01_t.before_cold_run_mode = Power_Mode;
					sFWHA01_t.run_mode = Cold_Mode;
					first_draw = false;
					sFWHA01_t.last_run_mode = RESET_VALUE;
					sFWHA01_t.system_parameter.last_cold_mode_set_air = 0x00;
					sFWHA01_t.system_parameter.last_curve_cold_mode_air_data = 0x00;
					sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
				}
				else
				{
					if(sFWHA01_t.run_mode == Standard_Mode)
						sFWHA01_t.run_mode = Power_Mode;
					else if(sFWHA01_t.run_mode == Power_Mode)
						sFWHA01_t.run_mode = Standard_Mode;
//					sFWHA01_t.before_cold_run_mode = sFWHA01_t.run_mode;
					first_draw = false;
					sFWHA01_t.last_run_mode = RESET_VALUE;
				}
			}
		}
        break;

    case KEY_LONG_PRESS:
        set_done = true;
		sbeep.cmd = BEEP_LONG;
		if(sFWHA01_t.long_key_mode == COLDWIND_MODE)
		{
			if(sFWHA01_t.run_mode == Standard_Mode || sFWHA01_t.run_mode == Power_Mode)
			{
				sFWHA01_t.before_cold_run_mode = sFWHA01_t.run_mode;
				sFWHA01_t.run_mode = Cold_Mode;
			}
			else if(sFWHA01_t.run_mode == Cold_Mode)
			{
				sFWHA01_t.run_mode = sFWHA01_t.before_cold_run_mode;
				sFWHA01_t.before_cold_run_mode = Cold_Mode;
//				sFWHA01_t.run_mode = Standard_Mode;
			}
			sFWHA01_t.system_parameter.last_cold_mode_set_air = 0x00;
			sFWHA01_t.system_parameter.last_curve_cold_mode_air_data = 0x00;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			
		}
		else if(sFWHA01_t.long_key_mode == POWER_MODE)
		{
			if(sFWHA01_t.run_mode == Cold_Mode)
			{
				if(sFWHA01_t.before_cold_run_mode == Power_Mode)
				{
					sFWHA01_t.before_cold_run_mode = Standard_Mode;
				}
				else if(sFWHA01_t.before_cold_run_mode == Standard_Mode)
					sFWHA01_t.before_cold_run_mode = Power_Mode;
				first_draw = false;
				sFWHA01_t.last_run_mode = RESET_VALUE;
			}
			else
			{
				if(sFWHA01_t.run_mode == Standard_Mode)
					sFWHA01_t.run_mode = Power_Mode;
				else if(sFWHA01_t.run_mode == Power_Mode)
					sFWHA01_t.run_mode = Standard_Mode;
			}
		}
	break;

    default:
        break;
    }
}


void event_handle(void)
{
	 switch (event)
	 {
		 case TEMP_ADD_EVENT:
			 lock_temp_flag = false;
			 last_key_adjust_state = true;
			out_times = 80;
			 sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			if (sFWHA01_t.temp_unit == CELSIUS)
			{
				if (sFWHA01_t.system_parameter.set_temp >= MAX_SET_TEMP)
				{
					sFWHA01_t.system_parameter.last_set_temp = 0;
					sFWHA01_t.system_parameter.set_temp = MAX_SET_TEMP;
				}                                                                                                                                                           
				else
				{
					sFWHA01_t.system_parameter.set_temp++;
				}

				sFWHA01_t.system_parameter.set_temp_f_display = 9 * sFWHA01_t.system_parameter.set_temp / 5 + 32;
			}
			else if (sFWHA01_t.temp_unit == FAHRENHEIT)
			{
				if (sFWHA01_t.system_parameter.set_temp_f_display >= MAX_SET_TEMP_F)
				{
					sFWHA01_t.system_parameter.last_set_temp_f_display = 0;
					sFWHA01_t.system_parameter.set_temp_f_display = MAX_SET_TEMP_F;
				}
				else
				{
					sFWHA01_t.system_parameter.set_temp_f_display++;
				}

				sFWHA01_t.system_parameter.set_temp = (sFWHA01_t.system_parameter.set_temp_f_display - 32) * 5 / 9;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}
			event = END_EVENT;
			break;
		 case TEMP_REDUCE_EVENT:
			 lock_temp_flag = false;
			 last_key_adjust_state = true;
			out_times = 80;
			sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			if (sFWHA01_t.temp_unit == CELSIUS)
			{
				if (sFWHA01_t.system_parameter.set_temp <= MIN_SET_TEMP)
				{
					sFWHA01_t.system_parameter.last_set_temp = 0;
					sFWHA01_t.system_parameter.set_temp = MIN_SET_TEMP;
				}
				else
				{
					sFWHA01_t.system_parameter.set_temp--;
				}

				sFWHA01_t.system_parameter.set_temp_f_display = 9 * sFWHA01_t.system_parameter.set_temp / 5 + 32;
			}
			else if (sFWHA01_t.temp_unit == FAHRENHEIT)
			{
				if (sFWHA01_t.system_parameter.set_temp_f_display <= MIN_SET_TEMP_F)
				{
					sFWHA01_t.system_parameter.last_set_temp_f_display = 0;
					sFWHA01_t.system_parameter.set_temp_f_display = MIN_SET_TEMP_F;
				}
				else
				{
					sFWHA01_t.system_parameter.set_temp_f_display--;
				}

				sFWHA01_t.system_parameter.set_temp = (sFWHA01_t.system_parameter.set_temp_f_display - 32) * 5 / 9;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}
			event = END_EVENT;
			break;
		 case TEMP_ADD_FIVE_EVENT:
			 lock_temp_flag = false;
			 last_key_adjust_state = true;
			out_times = 80;
			 sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			if (sFWHA01_t.temp_unit == CELSIUS)
			{
				if (sFWHA01_t.system_parameter.set_temp + 10 >= MAX_SET_TEMP)
				 {
					sFWHA01_t.system_parameter.last_set_temp = 0;
					sFWHA01_t.system_parameter.set_temp = MAX_SET_TEMP;
				}
				else
				{
					sFWHA01_t.system_parameter.set_temp += 10;
				}

				sFWHA01_t.system_parameter.set_temp_f_display = 9 * sFWHA01_t.system_parameter.set_temp / 5 + 32;
			}
			else if (sFWHA01_t.temp_unit == FAHRENHEIT)
			{
				if (sFWHA01_t.system_parameter.set_temp_f_display + 50 >= MAX_SET_TEMP_F)
				{
					sFWHA01_t.system_parameter.last_set_temp_f_display = 0;
					sFWHA01_t.system_parameter.set_temp_f_display = MAX_SET_TEMP_F;
				}
				else
				{
					sFWHA01_t.system_parameter.set_temp_f_display += 50;
				}

				sFWHA01_t.system_parameter.set_temp = (sFWHA01_t.system_parameter.set_temp_f_display - 32) * 5 / 9;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}
			event = END_EVENT;
			break;
		 case TEMP_REDUCE_FIVE_EVENT:
			 lock_temp_flag = false;
		 
			last_key_adjust_state = true;
			out_times = 80;
			sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			if (sFWHA01_t.temp_unit == CELSIUS)
			{
				if (sFWHA01_t.system_parameter.set_temp - 10 <= MIN_SET_TEMP)
				{
					sFWHA01_t.system_parameter.last_set_temp = 0;
				}
				else
				{
					sFWHA01_t.system_parameter.set_temp -= 10;
				}

				sFWHA01_t.system_parameter.set_temp_f_display = 9 * sFWHA01_t.system_parameter.set_temp / 5 + 32;
			}
			else if (sFWHA01_t.temp_unit == FAHRENHEIT)
			{
				if (sFWHA01_t.system_parameter.set_temp_f_display <= MIN_SET_TEMP_F + 41)
				{
					sFWHA01_t.system_parameter.last_set_temp_f_display = 0;
					sFWHA01_t.system_parameter.set_temp_f_display = MIN_SET_TEMP_F;
				}
				else
				{
					sFWHA01_t.system_parameter.set_temp_f_display -= 41;
				}

				sFWHA01_t.system_parameter.set_temp = (sFWHA01_t.system_parameter.set_temp_f_display - 32) * 5 / 9;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}	
			event = END_EVENT;
			break;
		 case WIND_ADD_EVENT:
			 last_key_adjust_state = true;
			out_times = 80;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			if(sFWHA01_t.system_parameter.air_data + 1 > MAX_SET_AIR)
			{
				sFWHA01_t.system_parameter.air_data = MAX_SET_AIR;
			}
			else
			{
				sFWHA01_t.system_parameter.air_data++;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}	
			event = END_EVENT;
			break;
		 case WIND_REDUCE_EVENT:
			last_key_adjust_state = true;
			out_times = 80;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			if(sFWHA01_t.system_parameter.air_data - 1 < MIN_SET_AIR)
			{
				sFWHA01_t.system_parameter.air_data = MIN_SET_AIR;
			}
			else
			{
				sFWHA01_t.system_parameter.air_data--;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}
			event = END_EVENT;
			break;
		 case WIND_ADD_FIVE_EVENT:
			last_key_adjust_state = true;
			out_times = 80;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			if(sFWHA01_t.system_parameter.air_data + 5 > MAX_SET_AIR)
			{
				sFWHA01_t.system_parameter.air_data = MAX_SET_AIR;
			}
			else
			{
				sFWHA01_t.system_parameter.air_data += 5;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}
			event = END_EVENT;
			break;
		 case WIND_REDUCE_FIVE_EVENT:
			last_key_adjust_state = true;
			out_times = 80;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			if(sFWHA01_t.system_parameter.air_data - 5 < MIN_SET_AIR)
			{
				sFWHA01_t.system_parameter.air_data = MIN_SET_AIR;
			}
			else
			{
				sFWHA01_t.system_parameter.air_data -= 5;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}
			event = END_EVENT;
			break;
		case COLD_WIND_ADD:
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			if(sFWHA01_t.system_parameter.cold_mode_set_air + 1 > MAX_SET_AIR)
			{
				sFWHA01_t.system_parameter.cold_mode_set_air = MAX_SET_AIR;
				sFWHA01_t.system_parameter.last_cold_mode_set_air = 0;
			}
			else
			{
				sFWHA01_t.system_parameter.cold_mode_set_air++;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}	
			event = END_EVENT;
			break;
		case COLD_WIND_REDUCE:
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			if(sFWHA01_t.system_parameter.cold_mode_set_air - 1 < MIN_SET_AIR)
			{
				sFWHA01_t.system_parameter.cold_mode_set_air = MIN_SET_AIR;
				sFWHA01_t.system_parameter.last_cold_mode_set_air = 0;
			}
			else
			{
				sFWHA01_t.system_parameter.cold_mode_set_air--;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}
			event = END_EVENT;
			break;
			 
		case COLD_WIND_ADD_FIVE:
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			if(sFWHA01_t.system_parameter.cold_mode_set_air + 5 > MAX_SET_AIR)
			{
				sFWHA01_t.system_parameter.cold_mode_set_air = MAX_SET_AIR;
				sFWHA01_t.system_parameter.last_cold_mode_set_air = 0;
			}
			else
			{
				sFWHA01_t.system_parameter.cold_mode_set_air += 5;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}
			 event = END_EVENT;
			break;
			
		case COLD_WIND_REDUCE_FIVE:
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			if(sFWHA01_t.system_parameter.cold_mode_set_air - 5 < MIN_SET_AIR)
			{
				sFWHA01_t.system_parameter.cold_mode_set_air = MIN_SET_AIR;
				sFWHA01_t.system_parameter.last_cold_mode_set_air = 0;
			}
			else
			{
				sFWHA01_t.system_parameter.cold_mode_set_air -= 5;
			}
			if(sFWHA01_t.general_parameter.ch != 0)
			{
				if(sFWHA01_t.general_parameter.ch == 1)
					sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 2)
					sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
				if(sFWHA01_t.general_parameter.ch == 3)
					sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;
				sFWHA01_t.general_parameter.ch = 0;
			}
			 event = END_EVENT;
			break;

		 default:break;
		 
	 }
	
}