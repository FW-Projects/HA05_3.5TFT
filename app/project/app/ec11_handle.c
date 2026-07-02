#include "ec11_handle.h"
#include "HA01_HANDLE.h"
#include "beep_handle.h"
#include "lcd_handle.h"
#include "key_handle.h"
void ec11_event_handle(void);
void air_handle(void);
void temp_ec11_get_event(EC11_AnalyzeResult state);
void air_ec11_get_event(EC11_AnalyzeResult state_air);
static ec11_event_e ec11_handle_event = EC11_END_EVENT;
static air_ec11_event_e air_ec11_handle_event = AIR_EC11_END_EVENT;
EC11_AnalyzeResult temp_analyze_result;
EC11_AnalyzeResult air_analyze_result;

static uint8_t cal_temp_state = 0x00;

void ec11_handle(void)
{
	EC11_ScanResult temp_scan_result = EC11_Scan(&temp_ec11);
	temp_analyze_result = EC11_Analyze(&temp_ec11, temp_scan_result);
	temp_ec11_get_event(temp_analyze_result);
    ec11_event_handle();
	
	EC11_ScanResult air_scan_result = EC11_Scan(&air_ec11);
	air_analyze_result = EC11_Analyze(&air_ec11, air_scan_result);
	air_ec11_get_event(air_analyze_result);
	air_handle();
}

void air_ec11_get_event(EC11_AnalyzeResult state_air)
{
    static bool first_in = false;
	static bool first_state = false;
    switch (state_air)
    {
    case EC11_ANALYZE_CW:	
        if ((sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE))
        {
            if(sFWHA01_t.run_mode == Cold_Mode)
			{
				air_ec11_handle_event = COLD_AIR_ADD;
			}
			else
				air_ec11_handle_event = AIR_ADD;
		}
		else if (sFWHA01_t.page >= SELECT_SET_WORK_PAGE_CN && sFWHA01_t.page <= SELECT_EXIT_MENU_PAGE_CN)
        {
            ec11_handle_event = PAGE_ADD;
        }
		else if (sFWHA01_t.page == SET_RUN_PAGE_CN)
        { 
            ec11_handle_event = SET_RUN_MODE;
        }
		
        else if (sFWHA01_t.page == SET_UNIT_PAGE_CN)
        {
            ec11_handle_event = SET_TEMP_UNIT;
        }
		
		else if (sFWHA01_t.page == SET_SPEAK_PAGE_CN)
        {
            ec11_handle_event = SET_SPEAK_MODE;
        }
		
		else if (sFWHA01_t.page == SET_KEY_PAGE_CN)
        {
            sFWHA01_t.set_key_mode++;
			if(sFWHA01_t.set_key_mode > RETURN_MENU)
			{
				sFWHA01_t.set_key_mode = LONGKEY_MODE;
			}
        }
		else if (sFWHA01_t.page == SET_LONG_KEY_PAGE_CN)
		{
			sFWHA01_t.set_long_key_mode++;
			if(sFWHA01_t.set_long_key_mode > POWER_MODE)
				sFWHA01_t.set_long_key_mode = COLDWIND_MODE;		

			if(sFWHA01_t.set_long_key_mode == COLDWIND_MODE)
			{
				sFWHA01_t.long_key_mode = COLDWIND_MODE;
			}
			else if(sFWHA01_t.set_long_key_mode == POWER_MODE)
			{
				sFWHA01_t.long_key_mode = POWER_MODE;
			}
		}
		else if (sFWHA01_t.page == SET_SHORT_KEY_PAGE_CN)
		{
			sFWHA01_t.set_short_key_mode++;
			if(sFWHA01_t.set_short_key_mode > POWER_MODE)
				sFWHA01_t.set_short_key_mode = CHANNEL_SWITCHING;		

			if(sFWHA01_t.set_short_key_mode == CHANNEL_SWITCHING)
			{
				sFWHA01_t.short_key_mode = CHANNEL_SWITCHING;
			}
			else if(sFWHA01_t.set_short_key_mode == COLDWIND_MODE)
			{
				sFWHA01_t.short_key_mode = COLDWIND_MODE;
			}
			else if(sFWHA01_t.set_short_key_mode == POWER_MODE)
			{
				sFWHA01_t.short_key_mode = POWER_MODE;
			}
		}
		else if (sFWHA01_t.page == SET_KEY_MODE_PAGE_CN)
		{
			sFWHA01_t.set_adjust_key_mode++;
			if(sFWHA01_t.set_adjust_key_mode > SELECT_CH)
				sFWHA01_t.set_adjust_key_mode = SELECT_TEMP;		

			if(sFWHA01_t.set_adjust_key_mode == SELECT_TEMP)
			{
				adjust_state = TEMP;
				sFWHA01_t.adjust_key_mode = SELECT_TEMP;
			}
			else if(sFWHA01_t.set_adjust_key_mode == SELECT_WIND)
			{
				adjust_state = WIND;
				sFWHA01_t.adjust_key_mode = SELECT_WIND;
			}
			else if(sFWHA01_t.set_adjust_key_mode == SELECT_CH)
			{
				sFWHA01_t.adjust_key_mode = SELECT_CH;
			}
			
		}	
		else if (sFWHA01_t.page == SET_LANGUAGE_PAGE_CN)
        {
            sFWHA01_t.page  = SET_LANGUAGE_PAGE_ENG;
            sFWHA01_t.language_state = ENGLISH;
        }
        else if (sFWHA01_t.page == SET_LANGUAGE_PAGE_ENG)
        {
            sFWHA01_t.page  = SET_LANGUAGE_PAGE_CN;
            sFWHA01_t.language_state = CHINESE;
        }	
        else if (sFWHA01_t.page == SET_TEMP_CAL_PAGE_CN)
        {
			if(sFWHA01_t.set_cal == SET_RETURN_CAL)
			{
				sFWHA01_t.set_cal = SET_SELECT_CAL;
				sFWHA01_t.page = SET_SELECT_TEMP_CAL_PAGE_CN;
			}
        }
		 else if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN)
        {
			if(sFWHA01_t.set_cal == SET_SELECT_CAL)
			{
				sFWHA01_t.set_cal = SET_RETURN_CAL;
				sFWHA01_t.page = SET_TEMP_CAL_PAGE_CN;
			}
			else if(sFWHA01_t.set_cal == SET_CAL)
			{
				 ec11_handle_event = SET_TEMP_CAL_ADD;
				
			}
        }
        else if (sFWHA01_t.page == SET_TEMP_LOCK_PAGE_CN)
        {
            ec11_handle_event = SET_TEMP_LOCK;
        }
        else if (sFWHA01_t.page == SET_SLEEP_PAGE_CN)
        {
           ec11_handle_event = SET_SLEEP_MODE;
        }
        else if (sFWHA01_t.page == SET_RESET_PAGE_CN)
        {
           ec11_handle_event = SET_RESET_MODE;
        }
        sbeep.cmd = BEEP_SHORT;
        break;

    case EC11_ANALYZE_CCW:
         if ((sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE))
        {
            if(sFWHA01_t.run_mode == Cold_Mode)
			{
				air_ec11_handle_event = COLD_AIR_REDUCE;
			}
			else
				air_ec11_handle_event = AIR_REDUCE;
        }
		else if (sFWHA01_t.page >= SELECT_SET_WORK_PAGE_CN && sFWHA01_t.page <= SELECT_EXIT_MENU_PAGE_CN)
        {
            ec11_handle_event = PAGE_REDUCE;
        }
		else if (sFWHA01_t.page == SET_RUN_PAGE_CN)
        {
            ec11_handle_event = SET_RUN_MODE;
        }
		
        else if (sFWHA01_t.page == SET_UNIT_PAGE_CN)
        {
            ec11_handle_event = SET_TEMP_UNIT;
        }
		
		else if (sFWHA01_t.page == SET_SPEAK_PAGE_CN)
        {
            ec11_handle_event = SET_SPEAK_MODE;
        }
		
		else if (sFWHA01_t.page == SET_KEY_PAGE_CN)
        {
            sFWHA01_t.set_key_mode--;
			if(sFWHA01_t.set_key_mode < LONGKEY_MODE)
			{
				sFWHA01_t.set_key_mode = RETURN_MENU;
			}
        }
		else if (sFWHA01_t.page == SET_LONG_KEY_PAGE_CN)
		{
			sFWHA01_t.set_long_key_mode--;
			if(sFWHA01_t.set_long_key_mode < COLDWIND_MODE)
				sFWHA01_t.set_long_key_mode = POWER_MODE;			
			
			if(sFWHA01_t.set_long_key_mode == COLDWIND_MODE)
			{
				sFWHA01_t.long_key_mode = COLDWIND_MODE;
			}
			else if(sFWHA01_t.set_long_key_mode == POWER_MODE)
			{
				sFWHA01_t.long_key_mode = POWER_MODE;
			}
		}
		else if (sFWHA01_t.page == SET_SHORT_KEY_PAGE_CN)
		{
			sFWHA01_t.set_short_key_mode--;
			if(sFWHA01_t.set_short_key_mode < CHANNEL_SWITCHING)
				sFWHA01_t.set_short_key_mode = POWER_MODE;			
			
			if(sFWHA01_t.set_short_key_mode == CHANNEL_SWITCHING)
			{
				sFWHA01_t.short_key_mode = CHANNEL_SWITCHING;
			}
			else if(sFWHA01_t.set_short_key_mode == COLDWIND_MODE)
			{
				sFWHA01_t.short_key_mode = COLDWIND_MODE;
			}
			else if(sFWHA01_t.set_short_key_mode == POWER_MODE)
			{
				sFWHA01_t.short_key_mode = POWER_MODE;
			}
		}
		else if (sFWHA01_t.page == SET_KEY_MODE_PAGE_CN)
		{
			sFWHA01_t.set_adjust_key_mode--;
			if(sFWHA01_t.set_adjust_key_mode < SELECT_TEMP)
				sFWHA01_t.set_adjust_key_mode = SELECT_CH;		
			
			if(sFWHA01_t.set_adjust_key_mode == SELECT_TEMP)
			{
				adjust_state = TEMP;
				sFWHA01_t.adjust_key_mode = SELECT_TEMP;
			}
			else if(sFWHA01_t.set_adjust_key_mode == SELECT_WIND)
			{
				adjust_state = WIND;
				sFWHA01_t.adjust_key_mode = SELECT_WIND;
			}
			else if(sFWHA01_t.set_adjust_key_mode == SELECT_CH)
			{
				sFWHA01_t.adjust_key_mode = SELECT_CH;
			}
		}
		
		
		else if (sFWHA01_t.page == SET_LANGUAGE_PAGE_CN)
        {
            sFWHA01_t.page  = SET_LANGUAGE_PAGE_ENG;
            sFWHA01_t.language_state = ENGLISH;
        }
        else if (sFWHA01_t.page == SET_LANGUAGE_PAGE_ENG)
        {
            sFWHA01_t.page  = SET_LANGUAGE_PAGE_CN;
            sFWHA01_t.language_state = CHINESE;
        }
		
        else if (sFWHA01_t.page == SET_TEMP_CAL_PAGE_CN)
        {
			if(sFWHA01_t.set_cal == SET_RETURN_CAL)
			{
				sFWHA01_t.set_cal = SET_SELECT_CAL;
				sFWHA01_t.page = SET_SELECT_TEMP_CAL_PAGE_CN;
				
			}
        }
		 else if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN)
        {
			if(sFWHA01_t.set_cal == SET_SELECT_CAL)
			{
				sFWHA01_t.set_cal = SET_RETURN_CAL;
				sFWHA01_t.page = SET_TEMP_CAL_PAGE_CN;
			}
			else if(sFWHA01_t.set_cal == SET_CAL)
			{
				 ec11_handle_event = SET_TEMP_CAL_REDUCE;
			}
        }
		   
        else if (sFWHA01_t.page == SET_TEMP_LOCK_PAGE_CN)
        {
            ec11_handle_event = SET_TEMP_LOCK;
        }
        else if (sFWHA01_t.page == SET_SLEEP_PAGE_CN)
        {
           ec11_handle_event = SET_SLEEP_MODE;
        }
        
        
        if (sFWHA01_t.page == SET_RESET_PAGE_CN)
        {
			ec11_handle_event = SET_RESET_MODE;
         
        }
        sbeep.cmd = BEEP_SHORT;
        break;

    case EC11_ANALYZE_FAST_CW:
		if ((sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE))
        {
            if(sFWHA01_t.run_mode == Cold_Mode)
			{
				air_ec11_handle_event = COLD_AIR_ADD_FIVE;
			}
			else
				air_ec11_handle_event = AIR_ADD_FIVE;
        }
		sbeep.cmd = BEEP_SHORT;
        break;
    case EC11_ANALYZE_FAST_CCW:
		if ((sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE))
        {
            if(sFWHA01_t.run_mode == Cold_Mode)
			{
				air_ec11_handle_event = COLD_AIR_REDUCE_FIVE;
			}
			else
				air_ec11_handle_event = AIR_REDUCE_FIVE;
        }
		sbeep.cmd = BEEP_SHORT;
        break;
    case EC11_ANALYZE_SHORT_CLICK:
		 if (sFWHA01_t.page != WORK_PAGE                   && 
				sFWHA01_t.page != CURVE_PAGE                  &&
				sFWHA01_t.page != SELECT_EXIT_MENU_PAGE_CN    &&
				sFWHA01_t.page != SET_RESET_DONE_PAGE_CN      &&
				sFWHA01_t.page != SET_RESET_PAGE_CN      &&
				sFWHA01_t.page != SET_SELECT_TEMP_CAL_PAGE_CN &&
				sFWHA01_t.page != SET_KEY_PAGE_CN &&
				sFWHA01_t.page != SET_LONG_KEY_PAGE_CN &&
				sFWHA01_t.page != SET_SHORT_KEY_PAGE_CN &&
				sFWHA01_t.page != SET_KEY_MODE_PAGE_CN)
        {
            ec11_handle_event = ENTER_SETS;
        }
        else if (sFWHA01_t.page == SELECT_EXIT_MENU_PAGE_CN)
        {
            ec11_handle_event = EXIT_MENU;
        }
		
		else if(sFWHA01_t.page == SET_KEY_PAGE_CN)
		{
			if(sFWHA01_t.set_key_mode == LONGKEY_MODE)
			{
				sFWHA01_t.page = SET_LONG_KEY_PAGE_CN;
				sFWHA01_t.set_long_key_mode = sFWHA01_t.long_key_mode;
			}
			else if(sFWHA01_t.set_key_mode == SHORTKEY_MODE)
			{
				sFWHA01_t.page = SET_SHORT_KEY_PAGE_CN;
				sFWHA01_t.set_short_key_mode = sFWHA01_t.short_key_mode;
			}
			else if(sFWHA01_t.set_key_mode == ADJUSTKEY_MODE)
			{
				sFWHA01_t.page = SET_KEY_MODE_PAGE_CN;
				sFWHA01_t.set_adjust_key_mode = sFWHA01_t.adjust_key_mode;
			}
			else if(sFWHA01_t.set_key_mode == RETURN_MENU)
			{
				sFWHA01_t.page = SELECT_SET_KEY_PAGE_CN;
				sFWHA01_t.long_key_mode = sFWHA01_t.set_long_key_mode;
				sFWHA01_t.short_key_mode = sFWHA01_t.set_short_key_mode;
				sFWHA01_t.adjust_key_mode = sFWHA01_t.set_adjust_key_mode;
			}
		}
		
		else if(sFWHA01_t.page == SET_LONG_KEY_PAGE_CN ||
				sFWHA01_t.page == SET_SHORT_KEY_PAGE_CN ||
				sFWHA01_t.page == SET_KEY_MODE_PAGE_CN)
		{
			sFWHA01_t.page = SET_KEY_PAGE_CN;
			sFWHA01_t.long_key_mode = sFWHA01_t.set_long_key_mode;
			sFWHA01_t.short_key_mode = sFWHA01_t.set_short_key_mode;
			sFWHA01_t.adjust_key_mode = sFWHA01_t.set_adjust_key_mode;
		}
		
		else if(sFWHA01_t.page == SET_RESET_PAGE_CN)
		{
			if(sFWHA01_t.set_reset1 == RESET_RETURN)
			{
				sFWHA01_t.page = SELECT_SET_RESET_PAGE_CN;
			}
			else if(sFWHA01_t.set_reset1 == RESET_CONFIRN)
			{
				sFWHA01_t.page = SET_RESET_DONE_PAGE_CN;
				ec11_handle_event = RESET_HA01_VALUE;
			}
		}
		
		else if(sFWHA01_t.page == SET_RESET_DONE_PAGE_CN)
		{
			if(sFWHA01_t.work_mode == WORK_CURVE)
			{
				sFWHA01_t.page = CURVE_PAGE;
			}
			else if(sFWHA01_t.work_mode == WORK_NORMAL)
			{
				sFWHA01_t.page = WORK_PAGE;
			}
		}
        else if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN)
        {
            if (sFWHA01_t.set_cal == SET_SELECT_CAL)
            {
				sFWHA01_t.set_cal = SET_CAL;
				sFWHA01_t.system_parameter.cal_temp_c_display = sFWHA01_t.system_parameter.set_temp;
				sFWHA01_t.system_parameter.cal_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display;
				sFWHA01_t.system_parameter.last_cal_temp_c_display = RESET_VALUE;
				sFWHA01_t.system_parameter.last_cal_temp_f_display = RESET_VALUE;
            }
            else
            {
				sFWHA01_t.set_cal = SET_SELECT_CAL;
				sFWHA01_t.system_parameter.cal_data += sFWHA01_t.system_parameter.set_temp - sFWHA01_t.system_parameter.cal_temp_c_display;

				if (sFWHA01_t.system_parameter.cal_data >= MAX_CAL_TEMP)
				{
					sFWHA01_t.system_parameter.cal_data = MAX_CAL_TEMP;
				}
				if (sFWHA01_t.system_parameter.cal_data<= MIN_CAL_TEMP)
				{
					sFWHA01_t.system_parameter.cal_data = MIN_CAL_TEMP;
				}
				sFWHA01_t.system_parameter.last_cal_temp_c_display = RESET_VALUE;
				sFWHA01_t.system_parameter.last_cal_temp_f_display = RESET_VALUE;
				sFWHA01_t.system_parameter.last_cal_temp_c_display = RESET_VALUE;
				sFWHA01_t.system_parameter.last_cal_temp_f_display = RESET_VALUE;
            }
        }
		sbeep.cmd = BEEP_SHORT;
        break;
    case EC11_ANALYZE_DOUBLE_CLICK:
		 if (sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE)
        {
            if (sFWHA01_t.page == WORK_PAGE )
            {
                sFWHA01_t.page = CURVE_PAGE;
                sFWHA01_t.work_mode = WORK_CURVE;
				
            }
            else if (sFWHA01_t.page == CURVE_PAGE)
            {
                sFWHA01_t.page = WORK_PAGE;
                sFWHA01_t.work_mode = WORK_NORMAL;
            }
			sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
			last_coldwind_state = RESET_VALUE;
            sbeep.cmd = BEEP_SHORT;
        }
        break;
    case EC11_ANALYZE_LONG_PRESS:
		if (first_state == false)
        {
            first_state = true;

            if (sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE)
            {
                ec11_handle_event = ENTER_MENU;
            }
            else if (sFWHA01_t.page >= SELECT_SET_WORK_PAGE_CN && sFWHA01_t.page <= SET_SUPPORT_PAGE_CN || sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN || sFWHA01_t.page == SET_SUPPORT_PAGE_CN_HA05)
            {
				if(sFWHA01_t.reset_flag == false)
                ec11_handle_event = EXIT_MENU;
            }

            sbeep.cmd = BEEP_LONG;
        }
        break;

    case EC11_ANALYZE_LONG_RELEASE:
		first_state = false;
        break;
    case EC11_ANALYZE_KEY_CW:
        // 按下并顺时针旋转
        break;
    case EC11_ANALYZE_KEY_CCW:
        // 按下并逆时针旋转
        break;
    default:
        // 无动作或未处理的动作
        break;
    }
}

void temp_ec11_get_event(EC11_AnalyzeResult state_temp)
{
    static bool first_in = false;

    switch (state_temp)
    {
    case EC11_ANALYZE_CW:
        if (sFWHA01_t.page >= SELECT_SET_WORK_PAGE_CN && sFWHA01_t.page <= SELECT_EXIT_MENU_PAGE_CN)
        {
            ec11_handle_event = PAGE_ADD;
        }
		
        else if ((sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE))
        {
            ec11_handle_event = TEMP_ADD;
        }
		
        else if (sFWHA01_t.page == SET_RUN_PAGE_CN)
        { 
            ec11_handle_event = SET_RUN_MODE;
        }
		
        else if (sFWHA01_t.page == SET_UNIT_PAGE_CN)
        {
            ec11_handle_event = SET_TEMP_UNIT;
        }
		
		else if (sFWHA01_t.page == SET_SPEAK_PAGE_CN)
        {
            ec11_handle_event = SET_SPEAK_MODE;
        }
		
		else if (sFWHA01_t.page == SET_KEY_PAGE_CN)
        {
            sFWHA01_t.set_key_mode++;
			if(sFWHA01_t.set_key_mode > RETURN_MENU)
			{
				sFWHA01_t.set_key_mode = LONGKEY_MODE;
			}
        }
		else if (sFWHA01_t.page == SET_LONG_KEY_PAGE_CN)
		{
			sFWHA01_t.set_long_key_mode++;
			if(sFWHA01_t.set_long_key_mode > POWER_MODE)
				sFWHA01_t.set_long_key_mode = COLDWIND_MODE;		

			if(sFWHA01_t.set_long_key_mode == COLDWIND_MODE)
			{
				sFWHA01_t.long_key_mode = COLDWIND_MODE;
			}
			else if(sFWHA01_t.set_long_key_mode == POWER_MODE)
			{
				sFWHA01_t.long_key_mode = POWER_MODE;
			}
		}
		else if (sFWHA01_t.page == SET_SHORT_KEY_PAGE_CN)
		{
			sFWHA01_t.set_short_key_mode++;
			if(sFWHA01_t.set_short_key_mode > POWER_MODE)
				sFWHA01_t.set_short_key_mode = CHANNEL_SWITCHING;		

			if(sFWHA01_t.set_short_key_mode == CHANNEL_SWITCHING)
			{
				sFWHA01_t.short_key_mode = CHANNEL_SWITCHING;
			}
			else if(sFWHA01_t.set_short_key_mode == COLDWIND_MODE)
			{
				sFWHA01_t.short_key_mode = COLDWIND_MODE;
			}
			else if(sFWHA01_t.set_short_key_mode == POWER_MODE)
			{
				sFWHA01_t.short_key_mode = POWER_MODE;
			}
		}
		else if (sFWHA01_t.page == SET_KEY_MODE_PAGE_CN)
		{
			sFWHA01_t.set_adjust_key_mode++;
			if(sFWHA01_t.set_adjust_key_mode > SELECT_CH)
				sFWHA01_t.set_adjust_key_mode = SELECT_TEMP;		

			if(sFWHA01_t.set_adjust_key_mode == SELECT_TEMP)
			{
				adjust_state = TEMP;
				sFWHA01_t.adjust_key_mode = SELECT_TEMP;
			}
			else if(sFWHA01_t.set_adjust_key_mode == SELECT_WIND)
			{
				adjust_state = WIND;
				sFWHA01_t.adjust_key_mode = SELECT_WIND;
			}
			else if(sFWHA01_t.set_adjust_key_mode == SELECT_CH)
			{
				sFWHA01_t.adjust_key_mode = SELECT_CH;
			}
			
		}
		
		else if (sFWHA01_t.page == SET_LANGUAGE_PAGE_CN)
        {
            sFWHA01_t.page  = SET_LANGUAGE_PAGE_ENG;
            sFWHA01_t.language_state = ENGLISH;
        }
        else if (sFWHA01_t.page == SET_LANGUAGE_PAGE_ENG)
        {
            sFWHA01_t.page  = SET_LANGUAGE_PAGE_CN;
            sFWHA01_t.language_state = CHINESE;
        }
		
        else if (sFWHA01_t.page == SET_TEMP_CAL_PAGE_CN)
        {
			if(sFWHA01_t.set_cal == SET_RETURN_CAL)
			{
				sFWHA01_t.set_cal = SET_SELECT_CAL;
				sFWHA01_t.page = SET_SELECT_TEMP_CAL_PAGE_CN;
			}
        }
		 else if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN)
        {
			if(sFWHA01_t.set_cal == SET_SELECT_CAL)
			{
				sFWHA01_t.set_cal = SET_RETURN_CAL;
				sFWHA01_t.page = SET_TEMP_CAL_PAGE_CN;
			}
			else if(sFWHA01_t.set_cal == SET_CAL)
			{
				 ec11_handle_event = SET_TEMP_CAL_ADD;
			}
        }
		   
        else if (sFWHA01_t.page == SET_TEMP_LOCK_PAGE_CN)
        {
            ec11_handle_event = SET_TEMP_LOCK;
        }
        else if (sFWHA01_t.page == SET_SLEEP_PAGE_CN)
        {
           ec11_handle_event = SET_SLEEP_MODE;
        }
        
        
        else if (sFWHA01_t.page == SET_RESET_PAGE_CN)
        {
           ec11_handle_event = SET_RESET_MODE;
        }

        sbeep.cmd = BEEP_SHORT;
        break;

    case EC11_ANALYZE_CCW:
        if (sFWHA01_t.page >= SELECT_SET_WORK_PAGE_CN && sFWHA01_t.page <= SELECT_EXIT_MENU_PAGE_CN)
        {
            ec11_handle_event = PAGE_REDUCE;
        }
         else if ((sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE))
        {
            ec11_handle_event = TEMP_REDUCE;
        }

        else if (sFWHA01_t.page == SET_RUN_PAGE_CN)
        {
            ec11_handle_event = SET_RUN_MODE;
        }
		
        else if (sFWHA01_t.page == SET_UNIT_PAGE_CN)
        {
            ec11_handle_event = SET_TEMP_UNIT;
        }
		
		else if (sFWHA01_t.page == SET_SPEAK_PAGE_CN)
        {
            ec11_handle_event = SET_SPEAK_MODE;
        }
		
		else if (sFWHA01_t.page == SET_KEY_PAGE_CN)
        {
            sFWHA01_t.set_key_mode--;
			if(sFWHA01_t.set_key_mode < LONGKEY_MODE)
			{
				sFWHA01_t.set_key_mode = RETURN_MENU;
			}
        }
		else if (sFWHA01_t.page == SET_LONG_KEY_PAGE_CN)
		{
			sFWHA01_t.set_long_key_mode--;
			if(sFWHA01_t.set_long_key_mode < COLDWIND_MODE)
				sFWHA01_t.set_long_key_mode = POWER_MODE;			
			
			if(sFWHA01_t.set_long_key_mode == COLDWIND_MODE)
			{
				sFWHA01_t.long_key_mode = COLDWIND_MODE;
			}
			else if(sFWHA01_t.set_long_key_mode == POWER_MODE)
			{
				sFWHA01_t.long_key_mode = POWER_MODE;
			}
		}
		else if (sFWHA01_t.page == SET_SHORT_KEY_PAGE_CN)
		{
			sFWHA01_t.set_short_key_mode--;
			if(sFWHA01_t.set_short_key_mode < CHANNEL_SWITCHING)
				sFWHA01_t.set_short_key_mode = POWER_MODE;			
			
			if(sFWHA01_t.set_short_key_mode == CHANNEL_SWITCHING)
			{
				sFWHA01_t.short_key_mode = CHANNEL_SWITCHING;
			}
			else if(sFWHA01_t.set_short_key_mode == COLDWIND_MODE)
			{
				sFWHA01_t.short_key_mode = COLDWIND_MODE;
			}
			else if(sFWHA01_t.set_short_key_mode == POWER_MODE)
			{
				sFWHA01_t.short_key_mode = POWER_MODE;
			}
		}
		else if (sFWHA01_t.page == SET_KEY_MODE_PAGE_CN)
		{
			sFWHA01_t.set_adjust_key_mode--;
			if(sFWHA01_t.set_adjust_key_mode < SELECT_TEMP)
				sFWHA01_t.set_adjust_key_mode = SELECT_CH;		
			
			if(sFWHA01_t.set_adjust_key_mode == SELECT_TEMP)
			{
				adjust_state = TEMP;
				sFWHA01_t.adjust_key_mode = SELECT_TEMP;
			}
			else if(sFWHA01_t.set_adjust_key_mode == SELECT_WIND)
			{
				adjust_state = WIND;
				sFWHA01_t.adjust_key_mode = SELECT_WIND;
			}
			else if(sFWHA01_t.set_adjust_key_mode == SELECT_CH)
			{
				sFWHA01_t.adjust_key_mode = SELECT_CH;
			}
		}
		
		
		else if (sFWHA01_t.page == SET_LANGUAGE_PAGE_CN)
        {
            sFWHA01_t.page  = SET_LANGUAGE_PAGE_ENG;
            sFWHA01_t.language_state = ENGLISH;
        }
        else if (sFWHA01_t.page == SET_LANGUAGE_PAGE_ENG)
        {
            sFWHA01_t.page  = SET_LANGUAGE_PAGE_CN;
            sFWHA01_t.language_state = CHINESE;
        }
		
        else if (sFWHA01_t.page == SET_TEMP_CAL_PAGE_CN)
        {
			if(sFWHA01_t.set_cal == SET_RETURN_CAL)
			{
				sFWHA01_t.set_cal = SET_SELECT_CAL;
				sFWHA01_t.page = SET_SELECT_TEMP_CAL_PAGE_CN;
				
			}
        }
		 else if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN)
        {
			if(sFWHA01_t.set_cal == SET_SELECT_CAL)
			{
				sFWHA01_t.set_cal = SET_RETURN_CAL;
				sFWHA01_t.page = SET_TEMP_CAL_PAGE_CN;
			}
			else if(sFWHA01_t.set_cal == SET_CAL)
			{
				 ec11_handle_event = SET_TEMP_CAL_REDUCE;
			}
        }
		   
        else if (sFWHA01_t.page == SET_TEMP_LOCK_PAGE_CN)
        {
            ec11_handle_event = SET_TEMP_LOCK;
        }
        else if (sFWHA01_t.page == SET_SLEEP_PAGE_CN)
        {
           ec11_handle_event = SET_SLEEP_MODE;
        }
        
        
        if (sFWHA01_t.page == SET_RESET_PAGE_CN)
        {
			ec11_handle_event = SET_RESET_MODE;
         
        }

        sbeep.cmd = BEEP_SHORT;
        break;

    case EC11_ANALYZE_FAST_CW:
        if ((sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE))
        {
            ec11_handle_event = TEMP_ADD_FIVE;
        }

        else if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN && sFWHA01_t.set_cal == SET_CAL)
        {
            ec11_handle_event = SET_TEMP_CAL_ADD_FIVE;
        }
        break;

    case EC11_ANALYZE_FAST_CCW:
        if ((sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE))
        {
            ec11_handle_event = TEMP_REDUCE_FIVE;
        }
          else if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN && sFWHA01_t.set_cal == SET_CAL)
        {
            ec11_handle_event = SET_TEMP_CAL_REDUCE_FIVE;
        }
        break;

    case EC11_ANALYZE_SHORT_CLICK:
            if (sFWHA01_t.page != WORK_PAGE                   && 
				sFWHA01_t.page != CURVE_PAGE                  &&
				sFWHA01_t.page != SELECT_EXIT_MENU_PAGE_CN    &&
				sFWHA01_t.page != SET_RESET_DONE_PAGE_CN      &&
				sFWHA01_t.page != SET_RESET_PAGE_CN      &&
				sFWHA01_t.page != SET_SELECT_TEMP_CAL_PAGE_CN &&
				sFWHA01_t.page != SET_KEY_PAGE_CN &&
				sFWHA01_t.page != SET_LONG_KEY_PAGE_CN &&
				sFWHA01_t.page != SET_SHORT_KEY_PAGE_CN &&
				sFWHA01_t.page != SET_KEY_MODE_PAGE_CN)
        {
            ec11_handle_event = ENTER_SETS;
        }
        else if (sFWHA01_t.page == SELECT_EXIT_MENU_PAGE_CN)
        {
            ec11_handle_event = EXIT_MENU;
        }
		
		else if(sFWHA01_t.page == SET_KEY_PAGE_CN)
		{
			if(sFWHA01_t.set_key_mode == LONGKEY_MODE)
			{
				sFWHA01_t.page = SET_LONG_KEY_PAGE_CN;
				sFWHA01_t.set_long_key_mode = sFWHA01_t.long_key_mode;
			}
			else if(sFWHA01_t.set_key_mode == SHORTKEY_MODE)
			{
				sFWHA01_t.page = SET_SHORT_KEY_PAGE_CN;
				sFWHA01_t.set_short_key_mode = sFWHA01_t.short_key_mode;
			}
			else if(sFWHA01_t.set_key_mode == ADJUSTKEY_MODE)
			{
				sFWHA01_t.page = SET_KEY_MODE_PAGE_CN;
				sFWHA01_t.set_adjust_key_mode = sFWHA01_t.adjust_key_mode;
			}
			else if(sFWHA01_t.set_key_mode == RETURN_MENU)
			{
				sFWHA01_t.page = SELECT_SET_KEY_PAGE_CN;
				sFWHA01_t.long_key_mode = sFWHA01_t.set_long_key_mode;
				sFWHA01_t.short_key_mode = sFWHA01_t.set_short_key_mode;
				sFWHA01_t.adjust_key_mode = sFWHA01_t.set_adjust_key_mode;
			}
		}
		
		else if(sFWHA01_t.page == SET_LONG_KEY_PAGE_CN ||
				sFWHA01_t.page == SET_SHORT_KEY_PAGE_CN ||
				sFWHA01_t.page == SET_KEY_MODE_PAGE_CN)
		{
			sFWHA01_t.page = SET_KEY_PAGE_CN;
			sFWHA01_t.long_key_mode = sFWHA01_t.set_long_key_mode;
			sFWHA01_t.short_key_mode = sFWHA01_t.set_short_key_mode;
			sFWHA01_t.adjust_key_mode = sFWHA01_t.set_adjust_key_mode;
		}
		
		else if(sFWHA01_t.page == SET_RESET_PAGE_CN)
		{
			if(sFWHA01_t.set_reset1 == RESET_RETURN)
			{
				sFWHA01_t.page = SELECT_SET_RESET_PAGE_CN;
			}
			else if(sFWHA01_t.set_reset1 == RESET_CONFIRN)
			{
				sFWHA01_t.page = SET_RESET_DONE_PAGE_CN;
				ec11_handle_event = RESET_HA01_VALUE;
			}
		}
		
		else if(sFWHA01_t.page == SET_RESET_DONE_PAGE_CN)
		{
			if(sFWHA01_t.work_mode == WORK_CURVE)
			{
				sFWHA01_t.page = CURVE_PAGE;
			}
			else if(sFWHA01_t.work_mode == WORK_NORMAL)
			{
				sFWHA01_t.page = WORK_PAGE;
			}
		}
        else if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN)
        {
            if (sFWHA01_t.set_cal == SET_SELECT_CAL)
            {
				sFWHA01_t.set_cal = SET_CAL;
				sFWHA01_t.system_parameter.cal_temp_c_display = sFWHA01_t.system_parameter.set_temp;
				sFWHA01_t.system_parameter.cal_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display;
				sFWHA01_t.system_parameter.last_cal_temp_c_display = RESET_VALUE;
				sFWHA01_t.system_parameter.last_cal_temp_f_display = RESET_VALUE;
            }
            else
            {
				sFWHA01_t.set_cal = SET_SELECT_CAL;
				sFWHA01_t.system_parameter.cal_data += sFWHA01_t.system_parameter.set_temp - sFWHA01_t.system_parameter.cal_temp_c_display;

				if (sFWHA01_t.system_parameter.cal_data >= MAX_CAL_TEMP)
				{
					sFWHA01_t.system_parameter.cal_data = MAX_CAL_TEMP;
				}
				if (sFWHA01_t.system_parameter.cal_data<= MIN_CAL_TEMP)
				{
					sFWHA01_t.system_parameter.cal_data = MIN_CAL_TEMP;
				}
				sFWHA01_t.system_parameter.last_cal_temp_c_display = RESET_VALUE;
				sFWHA01_t.system_parameter.last_cal_temp_f_display = RESET_VALUE;
				sFWHA01_t.system_parameter.last_cal_temp_c_display = RESET_VALUE;
				sFWHA01_t.system_parameter.last_cal_temp_f_display = RESET_VALUE;
            }
        }
        sbeep.cmd = BEEP_SHORT;
        break;

    case EC11_ANALYZE_DOUBLE_CLICK:
        if (sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE)
        {
            if (sFWHA01_t.page == WORK_PAGE )
            {
                sFWHA01_t.page = CURVE_PAGE;
                sFWHA01_t.work_mode = WORK_CURVE;
				
            }
            else if (sFWHA01_t.page == CURVE_PAGE)
            {
                sFWHA01_t.page = WORK_PAGE;
                sFWHA01_t.work_mode = WORK_NORMAL;
            }
			sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
			last_coldwind_state = RESET_VALUE;
            sbeep.cmd = BEEP_SHORT;
        }
        break;

    case EC11_ANALYZE_LONG_PRESS:
#if 1
        if (first_in == false)
        {
            first_in = true;

            if (sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE)
            {
                ec11_handle_event = ENTER_MENU;
            }
            else if (sFWHA01_t.page >= SELECT_SET_WORK_PAGE_CN && sFWHA01_t.page <= SET_SUPPORT_PAGE_CN || sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN || sFWHA01_t.page == SET_SUPPORT_PAGE_CN_HA05 )
            {
				if(sFWHA01_t.reset_flag == false)
                ec11_handle_event = EXIT_MENU;
            }

            sbeep.cmd = BEEP_LONG;
        }

#endif
        break;

    case EC11_ANALYZE_LONG_RELEASE:
        first_in = false;
        break;

    case EC11_ANALYZE_KEY_CW:
        // 按下并顺时针旋转
        break;

    case EC11_ANALYZE_KEY_CCW:
        // 按下并逆时针旋转
        break;

    default:
        // 无动作或未处理的动作
        break;
    }

//    if (temp_analyze_result != EC11_ANALYZE_NO_ACTION &&
//            temp_analyze_result != EC11_ANALYZE_SHORT_CLICK)
//    {
//        if (sFWHA01_t.Work_handle_state == HANDLE_SLEEP)
//        {
//            sFWHA01_t.Work_handle_state = HANDLE_WAKEN;
//			sFWHA01_t.system_parameter.waken_time_count = WAKEN_TIMES;
//        }
//        else if (sFWHA01_t.Work_handle_state == HANDLE_WAKEN)
//        {
//            sFWHA01_t.system_parameter.waken_time_count = WAKEN_TIMES;
//        }
//    }
}

void ec11_event_handle(void)
{
    switch (ec11_handle_event)
    {
    case ENTER_MENU:
        sFWHA01_t.page = SELECT_SET_WORK_PAGE_CN;
        ec11_handle_event = EC11_END_EVENT;
        break;

    case EXIT_MENU:
        if (sFWHA01_t.work_mode == WORK_NORMAL)
        {
            if (sFWHA01_t.handle_position == IN_POSSITION || sFWHA01_t.handle_position == NOT_IN_POSSITION )
            {
                sFWHA01_t.page = WORK_PAGE;
            }
        }
        else if (sFWHA01_t.work_mode == WORK_CURVE)
        {
            sFWHA01_t.page = CURVE_PAGE;
        }
		if(cal_temp_state == 1)
		{
			sFWHA01_t.run_mode = Power_Mode;
			cal_temp_state = 0;
			first_draw = false;
			sFWHA01_t.last_run_mode = RESET_VALUE;
		}
		actual_temp_refesh_time = 0x00;
//		sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
		if(sFWHA01_t.run_mode == Cold_Mode)
		{
			last_coldwind_state = RESET_VALUE;
			sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
			sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
		}
		else
		{
			sFWHA01_t.general_parameter.set_wind_time = 0x00;
			sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
			sFWHA01_t.system_parameter.last_air_data_actual = RESET_VALUE;
		}
        ec11_handle_event = EC11_END_EVENT;
        break;

    case ENTER_SETS:
        if (sFWHA01_t.page == SELECT_SET_WORK_PAGE_CN)
        {
            sFWHA01_t.page = SET_RUN_PAGE_CN;
			if(sFWHA01_t.run_mode == Cold_Mode)
				sFWHA01_t.set_run_mode = sFWHA01_t.before_cold_run_mode;
			else
				sFWHA01_t.set_run_mode = sFWHA01_t.run_mode;
        }
		
        else if (sFWHA01_t.page == SELECT_SET_UNIT_PAGE_CN)
        {
           sFWHA01_t.page = SET_UNIT_PAGE_CN;
			sFWHA01_t.set_temp_unit = sFWHA01_t.temp_unit;
        }
		
        else if (sFWHA01_t.page == SELECT_SET_SPEAK_PAGE_CN)
        {
           sFWHA01_t.page = SET_SPEAK_PAGE_CN;
			sFWHA01_t.set_speak_state = sFWHA01_t.speak_state;
        }
		
		else if (sFWHA01_t.page == SELECT_SET_KEY_PAGE_CN)
        {
			sFWHA01_t.page = SET_KEY_PAGE_CN;
			sFWHA01_t.set_key_mode = RETURN_MENU;
        }
		
        else if (sFWHA01_t.page == SELECT_SET_LANGUAGE_PAGE_CN)
        {
           if(sFWHA01_t.language_state == CHINESE)
			   sFWHA01_t.page = SET_LANGUAGE_PAGE_CN;
		   else if(sFWHA01_t.language_state == ENGLISH)
			   sFWHA01_t. page = SET_LANGUAGE_PAGE_ENG;
		   sFWHA01_t.set_language_state = sFWHA01_t.language_state;
        }
		
		else if (sFWHA01_t.page == SELECT_SET_TEMP_CAL_PAGE_CN)
        {
			sFWHA01_t.set_cal = SET_RETURN_CAL;
			sFWHA01_t.page = SET_TEMP_CAL_PAGE_CN;
			if(sFWHA01_t.run_mode == Power_Mode)
			{
				sFWHA01_t.run_mode = Standard_Mode;
				cal_temp_state = 1;
				first_draw = false;
				sFWHA01_t.last_run_mode = RESET_VALUE;
			}
        }   
		
		else if (sFWHA01_t.page == SELECT_SET_TEMP_LOCK_PAGE_CN)
        { 
            sFWHA01_t.page = SET_TEMP_LOCK_PAGE_CN;
			sFWHA01_t.set_display_lock_state = sFWHA01_t.display_lock_state;
        } 

		else if (sFWHA01_t.page == SELECT_SET_SLEEP_PAGE_CN)
        {
           sFWHA01_t.page = SET_SLEEP_PAGE_CN;
			sFWHA01_t.set_sleep_state = sFWHA01_t.sleep_state;
        } 
		 
		else if (sFWHA01_t.page == SELECT_SET_VERSION_PAGE_CN)
        {
           sFWHA01_t.page = SET_VERSION_PAGE_CN;
        }
		
		else if (sFWHA01_t.page == SELECT_SET_RESET_PAGE_CN)
        {
           sFWHA01_t.page = SET_RESET_PAGE_CN;
			sFWHA01_t.set_reset1 = RESET_RETURN;
        }
		
		else if (sFWHA01_t.page == SELECT_SET_SUPPORT_PAGE_CN)
        {
			sFWHA01_t.page = SET_SUPPORT_PAGE_CN_HA05;
        }
		
		else if(sFWHA01_t.page == SET_RUN_PAGE_CN)
		{
			if(sFWHA01_t.set_run_mode == Standard_Mode)
			{
				if(sFWHA01_t.run_mode == Cold_Mode)
					sFWHA01_t.before_cold_run_mode = Standard_Mode;
				else
					sFWHA01_t.run_mode = Standard_Mode;
			}
			else if(sFWHA01_t.set_run_mode == Power_Mode)
			{
				if(sFWHA01_t.run_mode == Cold_Mode)
					sFWHA01_t.before_cold_run_mode = Power_Mode;
				else 
					sFWHA01_t.run_mode = Power_Mode;
			}
			sFWHA01_t.page = SELECT_SET_WORK_PAGE_CN;
		}
		
		else if(sFWHA01_t.page == SET_UNIT_PAGE_CN)
		{
			if(sFWHA01_t.set_temp_unit == CELSIUS)
			{
				sFWHA01_t.temp_unit = CELSIUS;
			}
			if(sFWHA01_t.set_temp_unit == FAHRENHEIT)
			{
				sFWHA01_t.temp_unit = FAHRENHEIT;
			}
			sFWHA01_t.page = SELECT_SET_UNIT_PAGE_CN;
		}
		
		else if(sFWHA01_t.page == SET_SPEAK_PAGE_CN)
		{
			if(sFWHA01_t.set_speak_state == SPEAKER_OPEN)
			{
				sFWHA01_t.speak_state = SPEAKER_OPEN;
			}
			else if(sFWHA01_t.set_speak_state == SPEAKER_CLOSE)
			{
				sFWHA01_t.speak_state = SPEAKER_CLOSE;
			}
			sFWHA01_t.page = SELECT_SET_SPEAK_PAGE_CN;
		}
		
		else if(sFWHA01_t.page == SET_LANGUAGE_PAGE_CN)
		{
			if(sFWHA01_t.set_language_state== CHINESE)
			{
				sFWHA01_t.language_state = CHINESE;
			}
			sFWHA01_t.page = SELECT_SET_LANGUAGE_PAGE_CN;
		}
		else if(sFWHA01_t.page == SET_LANGUAGE_PAGE_ENG)
		{
			if(sFWHA01_t.set_language_state== ENGLISH)
			{
				sFWHA01_t.language_state = ENGLISH;
			}
			sFWHA01_t.page = SELECT_SET_LANGUAGE_PAGE_CN;
		}
		
		else if(sFWHA01_t.page == SET_TEMP_CAL_PAGE_CN)
		{
			if(sFWHA01_t.set_cal == SET_RETURN_CAL)
			{
				sFWHA01_t.page = SELECT_SET_TEMP_CAL_PAGE_CN;
				if(cal_temp_state == 1)
				{
					sFWHA01_t.run_mode = Power_Mode;
					cal_temp_state = 0;
					first_draw = false;
					sFWHA01_t.last_run_mode = RESET_VALUE;
				}
			}
		}
		else if(sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN)
		{
			if(sFWHA01_t.set_cal == SET_SELECT_CAL)
			{
				sFWHA01_t.set_cal = SET_CAL;
			}
			else if(sFWHA01_t.set_cal == SET_CAL)
			{
				sFWHA01_t.set_cal = SET_SELECT_CAL;
			}
		}
		
		else if(sFWHA01_t.page == SET_TEMP_LOCK_PAGE_CN)
		{
			if(sFWHA01_t.set_display_lock_state == UNLOCK)
			{
				sFWHA01_t.display_lock_state = UNLOCK;
			}
			else if(sFWHA01_t.set_display_lock_state == LOCK)
			{
				sFWHA01_t.display_lock_state = LOCK;
			}
			sFWHA01_t.page = SELECT_SET_TEMP_LOCK_PAGE_CN;
		}
		
		else if(sFWHA01_t.page == SET_SLEEP_PAGE_CN)
		{
			if(sFWHA01_t.set_sleep_state == SLEEP_OPEN)
			{
				sFWHA01_t.sleep_state = SLEEP_OPEN;
			}
			else if(sFWHA01_t.set_sleep_state == SLEEP_CLOSE)
			{
				sFWHA01_t.sleep_state = SLEEP_CLOSE;
			}
			sFWHA01_t.page = SELECT_SET_SLEEP_PAGE_CN;
		}
		
		else if(sFWHA01_t.page == SET_VERSION_PAGE_CN)
		{
			sFWHA01_t.page = SELECT_SET_VERSION_PAGE_CN;
		}

		else if(sFWHA01_t.page == SET_SUPPORT_PAGE_CN || sFWHA01_t.page == SET_SUPPORT_PAGE_CN_HA05)
		{
			sFWHA01_t.page = SELECT_SET_SUPPORT_PAGE_CN;
		} 
        ec11_handle_event = EC11_END_EVENT;
        break;

    case PAGE_ADD:
		if (sFWHA01_t.page >= SELECT_EXIT_MENU_PAGE_CN)
		{
			sFWHA01_t.page = SELECT_SET_WORK_PAGE_CN;
		}
		else
		{
			sFWHA01_t.page += 2;
		}

        ec11_handle_event = EC11_END_EVENT;
        break;

    case PAGE_REDUCE:
		if (sFWHA01_t.page <= SELECT_SET_WORK_PAGE_CN)
		{
			sFWHA01_t.page = SELECT_EXIT_MENU_PAGE_CN;
		}
		else
		{
			sFWHA01_t.page-=2;  
		}
        ec11_handle_event = EC11_END_EVENT;
        break;
    case TEMP_ADD:
		if (sFWHA01_t.handle_error_state == HANDLE_OK)
		{
			lock_temp_flag = false;
			sbeep.cmd = BEEP_SHORT;
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
		}
        ec11_handle_event = EC11_END_EVENT;
        break;

    case TEMP_REDUCE:
		if (sFWHA01_t.handle_error_state == HANDLE_OK)
		{
			lock_temp_flag = false;
			sbeep.cmd = BEEP_SHORT;
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
		}
        ec11_handle_event = EC11_END_EVENT;
        break;

    case TEMP_ADD_FIVE:
		if (sFWHA01_t.handle_error_state == HANDLE_OK)
		{
			lock_temp_flag = false;
			sbeep.cmd = BEEP_SHORT;
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
		}
        ec11_handle_event = EC11_END_EVENT;
        break;

    case TEMP_REDUCE_FIVE:
		if (sFWHA01_t.handle_error_state == HANDLE_OK)
		{
			lock_temp_flag = false;
			sbeep.cmd = BEEP_SHORT;
			sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			if (sFWHA01_t.temp_unit == CELSIUS)
			{
				if (sFWHA01_t.system_parameter.set_temp - 10 <= MIN_SET_TEMP)
				{
					sFWHA01_t.system_parameter.last_set_temp = 0;
					sFWHA01_t.system_parameter.set_temp = MIN_SET_TEMP;
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
		}
        ec11_handle_event = EC11_END_EVENT;
        break;

    case SET_RUN_MODE:
		if(sFWHA01_t.set_run_mode == Standard_Mode)
			{
				if(sFWHA01_t.run_mode == Cold_Mode)
				{
					sFWHA01_t.before_cold_run_mode = Power_Mode;
					sFWHA01_t.set_run_mode = Power_Mode;
					first_draw = false;
					sFWHA01_t.last_run_mode = RESET_VALUE;
				}
				else
				{
					sFWHA01_t.set_run_mode = Power_Mode;
					sFWHA01_t.run_mode = Power_Mode;
				}

			}
			else if(sFWHA01_t.set_run_mode == Power_Mode)
			{
				if(sFWHA01_t.run_mode == Cold_Mode)
				{
					sFWHA01_t.before_cold_run_mode = Standard_Mode;
					sFWHA01_t.set_run_mode = Standard_Mode;
					first_draw = false;
					sFWHA01_t.last_run_mode = RESET_VALUE;
				}
				else
				{
					sFWHA01_t.set_run_mode = Standard_Mode;
					sFWHA01_t.run_mode = Standard_Mode;
				}
			}
        ec11_handle_event = EC11_END_EVENT;
        break;

    case SET_TEMP_UNIT:
        if (sFWHA01_t.set_temp_unit == CELSIUS)
        {
			sFWHA01_t.set_temp_unit = FAHRENHEIT;
            sFWHA01_t.temp_unit = FAHRENHEIT;
        }
        else if (sFWHA01_t.set_temp_unit == FAHRENHEIT)
        {
			sFWHA01_t.set_temp_unit = CELSIUS;
            sFWHA01_t.temp_unit = CELSIUS;
        }
        ec11_handle_event = EC11_END_EVENT;
        break;

	case SET_SPEAK_MODE:
	if (sFWHA01_t.set_speak_state == SPEAKER_CLOSE)
	{
		sFWHA01_t.set_speak_state = SPEAKER_OPEN;
		sFWHA01_t.speak_state = SPEAKER_OPEN;
	}
	else if (sFWHA01_t.set_speak_state == SPEAKER_OPEN)
	{
		sFWHA01_t.set_speak_state = SPEAKER_CLOSE;
		sFWHA01_t.speak_state = SPEAKER_CLOSE;
	}
	ec11_handle_event = EC11_END_EVENT;
	break;
	
	case SET_TEMP_LOCK:
	if (sFWHA01_t.set_display_lock_state == LOCK)
	{
		sFWHA01_t.set_display_lock_state = UNLOCK;
		sFWHA01_t.display_lock_state = UNLOCK;
	}
	else if (sFWHA01_t.set_display_lock_state == UNLOCK)
	{
		sFWHA01_t.set_display_lock_state = LOCK;
		sFWHA01_t.display_lock_state = LOCK;
	}
	ec11_handle_event = EC11_END_EVENT;
	break;
	
	case SET_SLEEP_MODE:
	if (sFWHA01_t.set_sleep_state == SLEEP_CLOSE)
	{
		sFWHA01_t.set_sleep_state = SLEEP_OPEN;
		sFWHA01_t.sleep_state = SLEEP_OPEN;
	}
	else if (sFWHA01_t.set_sleep_state == SLEEP_OPEN)
	{
		sFWHA01_t.set_sleep_state = SLEEP_CLOSE;
		sFWHA01_t.sleep_state = SLEEP_CLOSE;
	}
	ec11_handle_event = EC11_END_EVENT;
	break;

    case SET_TEMP_CAL_ADD:
        if (sFWHA01_t.temp_unit == CELSIUS)
        {
            if (sFWHA01_t.system_parameter.cal_temp_c_display >= sFWHA01_t.system_parameter.set_temp + MAX_CAL_TEMP)
            {
                sFWHA01_t.system_parameter.cal_temp_c_display = sFWHA01_t.system_parameter.set_temp + MAX_CAL_TEMP;
            }
            else
            {
                sFWHA01_t.system_parameter.cal_temp_c_display++;
            }

            sFWHA01_t.system_parameter.cal_temp_f_display = 9 * sFWHA01_t.system_parameter.set_temp / 5 + 32;
        }
        else if (sFWHA01_t.temp_unit == FAHRENHEIT)
        {
            if (sFWHA01_t.system_parameter.cal_temp_f_display >= sFWHA01_t.system_parameter.set_temp_f_display + MAX_CAL_TEMP_F)
            {
                sFWHA01_t.system_parameter.cal_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display + MAX_CAL_TEMP_F;
            }
            else
            {
                sFWHA01_t.system_parameter.cal_temp_f_display++;
            }

            sFWHA01_t.system_parameter.cal_temp_c_display = (sFWHA01_t.system_parameter.cal_temp_f_display - 32) * 5 / 9;
        }

        ec11_handle_event = EC11_END_EVENT;
        break;

    case SET_TEMP_CAL_REDUCE:
        if (sFWHA01_t.temp_unit == CELSIUS)
        {
            if (sFWHA01_t.system_parameter.cal_temp_c_display <= sFWHA01_t.system_parameter.set_temp + MIN_CAL_TEMP)
            {
                sFWHA01_t.system_parameter.cal_temp_c_display = sFWHA01_t.system_parameter.set_temp + MIN_CAL_TEMP;
            }
            else
            {
                sFWHA01_t.system_parameter.cal_temp_c_display--;
            }

            sFWHA01_t.system_parameter.cal_temp_f_display = 9 * sFWHA01_t.system_parameter.set_temp / 5 + 32;
        }
        else if (sFWHA01_t.temp_unit == FAHRENHEIT)
        {
            if (sFWHA01_t.system_parameter.cal_temp_f_display <= sFWHA01_t.system_parameter.set_temp_f_display + MIN_CAL_TEMP_F)
            {
                sFWHA01_t.system_parameter.cal_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display + MIN_CAL_TEMP_F;
            }
            else
            {
                sFWHA01_t.system_parameter.cal_temp_f_display--;
            }

            sFWHA01_t.system_parameter.cal_temp_c_display = (sFWHA01_t.system_parameter.cal_temp_f_display - 32) * 5 / 9;
        }

        ec11_handle_event = EC11_END_EVENT;
        break;

    case SET_TEMP_CAL_ADD_FIVE:
        if (sFWHA01_t.temp_unit == CELSIUS)
        {
            if (sFWHA01_t.system_parameter.cal_temp_c_display + 10 >= sFWHA01_t.system_parameter.set_temp + MAX_CAL_TEMP)
            {
                sFWHA01_t.system_parameter.cal_temp_c_display = sFWHA01_t.system_parameter.set_temp + MAX_CAL_TEMP;
            }
            else
            {
                sFWHA01_t.system_parameter.cal_temp_c_display += 10;
            }

            sFWHA01_t.system_parameter.cal_temp_f_display = 9 * sFWHA01_t.system_parameter.set_temp / 5 + 32;
        }
        else if (sFWHA01_t.temp_unit == FAHRENHEIT)
        {
            if (sFWHA01_t.system_parameter.cal_temp_f_display + 10 >= sFWHA01_t.system_parameter.set_temp_f_display + MAX_CAL_TEMP_F)
            {
                sFWHA01_t.system_parameter.cal_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display + MAX_CAL_TEMP_F;
            }
            else
            {
                sFWHA01_t.system_parameter.cal_temp_f_display += 10;
            }

            sFWHA01_t.system_parameter.cal_temp_c_display = (sFWHA01_t.system_parameter.cal_temp_f_display - 32) * 5 / 9;
        }

        ec11_handle_event = EC11_END_EVENT;
        break;

    case SET_TEMP_CAL_REDUCE_FIVE:
        if (sFWHA01_t.temp_unit == CELSIUS)
        {
            if (sFWHA01_t.system_parameter.cal_temp_c_display - 10 <= sFWHA01_t.system_parameter.set_temp + MIN_CAL_TEMP)
            {
                sFWHA01_t.system_parameter.cal_temp_c_display = sFWHA01_t.system_parameter.set_temp + MIN_CAL_TEMP;
            }
            else
            {
                sFWHA01_t.system_parameter.cal_temp_c_display -= 10;
            }

            sFWHA01_t.system_parameter.cal_temp_f_display = 9 * sFWHA01_t.system_parameter.set_temp / 5 + 32;
        }
        else if (sFWHA01_t.temp_unit == FAHRENHEIT)
        {
            if (sFWHA01_t.system_parameter.cal_temp_f_display - 10 <= sFWHA01_t.system_parameter.set_temp_f_display + MIN_CAL_TEMP_F)
            {
                sFWHA01_t.system_parameter.cal_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display + MIN_CAL_TEMP_F;
            }
            else
            {
                sFWHA01_t.system_parameter.cal_temp_f_display -= 10;
            }

            sFWHA01_t.system_parameter.cal_temp_c_display = (sFWHA01_t.system_parameter.cal_temp_f_display - 32) * 5 / 9;
        }

        ec11_handle_event = EC11_END_EVENT;
        break;

	case SET_RESET_MODE:
		if(sFWHA01_t.set_reset1 == RESET_RETURN)
		{
			sFWHA01_t.set_reset1 = RESET_CONFIRN;
		}
		else if(sFWHA01_t.set_reset1 == RESET_CONFIRN)
		{
		sFWHA01_t.set_reset1 = RESET_RETURN;
		}
		ec11_handle_event = EC11_END_EVENT;
		break;
    case RESET_HA01_VALUE:
        sFWHA01_t.reset_flag = true;
        /* reset value */
        /* reset handle data */
        sFWHA01_t.system_parameter.set_temp = 380;
		sFWHA01_t.system_parameter.air_data = 40;
        sFWHA01_t.system_parameter.cal_data= 0;
        sFWHA01_t.system_parameter.ch1_set_temp = 350;
        sFWHA01_t.system_parameter.ch2_set_temp = 380;
        sFWHA01_t.system_parameter.ch3_set_temp = 400;
		sFWHA01_t.system_parameter.ch1_set_air = 20;
		sFWHA01_t.system_parameter.ch2_set_air = 30;
		sFWHA01_t.system_parameter.ch3_set_air = 40;
		sFWHA01_t.general_parameter.ch = 0;
        sFWHA01_t.system_parameter.set_sleep_time = 0;
        /* reset function data */
		sFWHA01_t.work_mode = WORK_NORMAL;
		sFWHA01_t.set_work_mode = sFWHA01_t.work_mode;
	
		sFWHA01_t.run_mode = Standard_Mode;
		sFWHA01_t.set_run_mode = Standard_Mode;
	
		sFWHA01_t.temp_unit = CELSIUS;
		sFWHA01_t.set_temp_unit = sFWHA01_t.temp_unit;
		
		sFWHA01_t.speak_state = SPEAKER_OPEN;
		sFWHA01_t.set_speak_state = sFWHA01_t.speak_state;
	
		sFWHA01_t.language_state = CHINESE;
	    sFWHA01_t.set_language_state = sFWHA01_t.language_state; 
	
        sFWHA01_t.display_lock_state = LOCK;
		sFWHA01_t.set_display_lock_state = sFWHA01_t.display_lock_state;
		
		sFWHA01_t.sleep_state = SLEEP_OPEN;
		sFWHA01_t.set_sleep_state = sFWHA01_t.sleep_state;
		sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
		sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
		sFWHA01_t.long_key_mode = COLDWIND_MODE;
		sFWHA01_t.set_long_key_mode = COLDWIND_MODE;
		sFWHA01_t.short_key_mode = CHANNEL_SWITCHING;
		sFWHA01_t.set_short_key_mode = CHANNEL_SWITCHING;
		sFWHA01_t.adjust_key_mode = SELECT_TEMP;
		sFWHA01_t.set_adjust_key_mode = SELECT_TEMP;
		
		
        /* reset other data */
        
        ec11_handle_event = EC11_END_EVENT;
        break;

    }
}


void air_handle(void)
{
	switch(air_ec11_handle_event)
	{
		case AIR_ADD:
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
			air_ec11_handle_event = AIR_EC11_END_EVENT;
			break;
		case AIR_REDUCE:
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
			air_ec11_handle_event = AIR_EC11_END_EVENT;
			break;
			 
		case AIR_ADD_FIVE:
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
			 air_ec11_handle_event = AIR_EC11_END_EVENT;
			break;
			
		case AIR_REDUCE_FIVE:
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
			 air_ec11_handle_event = AIR_EC11_END_EVENT;
			break;
			
		case COLD_AIR_ADD:
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
			air_ec11_handle_event = AIR_EC11_END_EVENT;
			break;
		case COLD_AIR_REDUCE:
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
			air_ec11_handle_event = AIR_EC11_END_EVENT;
			break;
			 
		case COLD_AIR_ADD_FIVE:
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
			 air_ec11_handle_event = AIR_EC11_END_EVENT;
			break;
			
		case COLD_AIR_REDUCE_FIVE:
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
			 air_ec11_handle_event = AIR_EC11_END_EVENT;
			break;
			
		case AIR_EC11_END_EVENT:
			break;
	}
}