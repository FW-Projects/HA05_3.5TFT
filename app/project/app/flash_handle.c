#include "flash_handle.h"
#include "lcd_handle.h"
#include "HA01_handle.h"


void get_data_from_b(void);
void get_data_from_a(void);
void get_reset_data(void);
void check_data_all(void);
uint16_t data_check(uint32_t address)
{
    static uint16_t data;
    data = flash_wred_halfword(address);

    if (data == 0xffff)
    {
        return FALSE; 
    }
    else
    {
        return data;
    }
}


uint16_t data_check_len(uint32_t address, uint8_t len)
{
    volatile uint16_t data;
    uint8_t i;

    for (i = 0; i < len; i += DATA_SIZE)
    {
        data = flash_wred_halfword(address + i);

        if (data == 0xFFFF)
        {
            break;
        }
    }

    return data;
}


void FlashProc(void)
{
    static flash_handle_t sflash;
    static uint16_t last_set_temp;
    static uint16_t last_set_wind;
    static uint16_t last_ch1_set_temp;
    static uint16_t last_ch1_set_wind;
    static uint16_t last_ch2_set_temp;
    static uint16_t last_ch2_set_wind;
    static uint16_t last_ch3_set_temp;
    static uint16_t last_ch3_set_wind;
    static uint8_t last_work_mode;
	static uint8_t last_temp_unit_mode;
    static uint8_t last_speak_mode;
	static uint8_t last_language_mode;
	static uint16_t last_cal_data;
    static uint8_t last_display_lock_mode;
	static uint8_t last_sleep_mode;
	static uint8_t last_run_mode;
	static uint8_t last_longkey_mode;
	static uint8_t last_shortkey_mode;
	static uint8_t last_adjustkey_mode;
	static uint16_t last_cold_set_wind;
	static uint8_t last_model;
    static uint16_t flash_version = 0;
    static uint8_t flash_count = 0;
    static uint16_t a_ver;
    static uint16_t b_ver;

    switch (sflash.state)
    {
    case FLASH_START:
		if (sFWHA01_t.init_flag == false)
        {
            /* 检测主存储区和备份存储区数据 */
            if (data_check_len(A_SET_TEMP_ADDRESS, FLASH_MENBER) != 0xFFFF
                    && data_check_len(A_SET_TEMP_ADDRESS, FLASH_MENBER) != 0xFFFF)
            {
                /* 检查哪个区的数据最新 */
                a_ver = flash_wred_halfword(A_FLASH_VERSION_ADDRESS);
                b_ver = flash_wred_halfword(B_FLASH_VERSION_ADDRESS);

                if (a_ver > b_ver)
                {
					get_data_from_a();
                    
                }
                else
                {
					get_data_from_b();
                }
            }
            /* 检查主储区数据 */
            else if (data_check_len(A_SET_TEMP_ADDRESS, FLASH_MENBER) != 0xFFFF)
            {
                get_data_from_a();
            }
            /* 检查备份存储区数据 */
            else if (data_check_len(B_SET_TEMP_ADDRESS, FLASH_MENBER) != 0xFFFF)
            {
               get_data_from_b();
            }
            else
            {
				get_reset_data();
            }
			
			check_data_all();
            sFWHA01_t.init_flag  = TRUE;
        }
        else
        {
            sflash.state = FLASH_HANDLE_DATA;
            break;
        }

        break;

    case FLASH_HANDLE_DATA:
        if (last_set_temp != sFWHA01_t.system_parameter.set_temp             ||
                last_set_wind != sFWHA01_t.system_parameter.air_data         ||
                last_ch1_set_temp != sFWHA01_t.system_parameter.ch1_set_temp ||
                last_ch1_set_wind != sFWHA01_t.system_parameter.ch1_set_air  ||
                last_ch1_set_temp != sFWHA01_t.system_parameter.ch2_set_temp ||
                last_ch1_set_wind != sFWHA01_t.system_parameter.ch2_set_air  ||
                last_ch1_set_temp != sFWHA01_t.system_parameter.ch3_set_temp ||
                last_ch1_set_wind != sFWHA01_t.system_parameter.ch3_set_air  ||
                last_work_mode != sFWHA01_t.work_mode                        ||
				last_temp_unit_mode != sFWHA01_t.temp_unit                   ||
                last_speak_mode != sFWHA01_t.speak_state                     ||
				last_language_mode != sFWHA01_t.language_state               ||
				last_cal_data != sFWHA01_t.system_parameter.cal_data         ||
                last_display_lock_mode != sFWHA01_t.display_lock_state       ||
                last_sleep_mode != sFWHA01_t.sleep_state                     ||
				last_run_mode != sFWHA01_t.run_mode							 ||
				last_longkey_mode != sFWHA01_t.long_key_mode                 ||
				last_shortkey_mode != sFWHA01_t.short_key_mode               ||
				last_adjustkey_mode != sFWHA01_t.adjust_key_mode 	         ||
				last_cold_set_wind != sFWHA01_t.system_parameter.cold_mode_set_air)
        {
            flash_unlock();

            if (flash_count % 2 != FALSE)
            {
				flash_sector_erase(A_SET_TEMP_ADDRESS);
				flash_halfword_program(A_SET_TEMP_ADDRESS, sFWHA01_t.system_parameter.set_temp);
				flash_halfword_program(A_SET_AIR_ADDRESS, sFWHA01_t.system_parameter.air_data);
				flash_halfword_program(A_CH1_SET_TEMP_ADDRESS, sFWHA01_t.system_parameter.ch1_set_temp);
				flash_halfword_program(A_CH1_SET_WIND_ADDRESS, sFWHA01_t.system_parameter.ch1_set_air);
				flash_halfword_program(A_CH2_SET_TEMP_ADDRESS, sFWHA01_t.system_parameter.ch2_set_temp);
				flash_halfword_program(A_CH2_SET_WIND_ADDRESS, sFWHA01_t.system_parameter.ch2_set_air);
				flash_halfword_program(A_CH3_SET_TEMP_ADDRESS, sFWHA01_t.system_parameter.ch3_set_temp);
				flash_halfword_program(A_CH3_SET_WIND_ADDRESS, sFWHA01_t.system_parameter.ch3_set_air);
            }
            else
            {
                flash_sector_erase(B_SET_TEMP_ADDRESS);
				flash_halfword_program(B_SET_TEMP_ADDRESS, sFWHA01_t.system_parameter.set_temp);
				flash_halfword_program(B_SET_AIR_ADDRESS, sFWHA01_t.system_parameter.air_data);
				flash_halfword_program(B_CH1_SET_TEMP_ADDRESS, sFWHA01_t.system_parameter.ch1_set_temp);
				flash_halfword_program(B_CH1_SET_WIND_ADDRESS, sFWHA01_t.system_parameter.ch1_set_air);
				flash_halfword_program(B_CH2_SET_TEMP_ADDRESS, sFWHA01_t.system_parameter.ch2_set_temp);
				flash_halfword_program(B_CH2_SET_WIND_ADDRESS, sFWHA01_t.system_parameter.ch2_set_air);
				flash_halfword_program(B_CH3_SET_TEMP_ADDRESS, sFWHA01_t.system_parameter.ch3_set_temp);
				flash_halfword_program(B_CH3_SET_WIND_ADDRESS, sFWHA01_t.system_parameter.ch3_set_air);
            }

            last_set_temp = sFWHA01_t.system_parameter.set_temp;
			last_set_wind = sFWHA01_t.system_parameter.air_data;
			last_ch1_set_temp = sFWHA01_t.system_parameter.ch1_set_temp;
			last_ch1_set_wind = sFWHA01_t.system_parameter.ch1_set_air;
			last_ch1_set_temp = sFWHA01_t.system_parameter.ch2_set_temp;
			last_ch1_set_wind = sFWHA01_t.system_parameter.ch2_set_air;
			last_ch1_set_temp = sFWHA01_t.system_parameter.ch3_set_temp;
			last_ch1_set_wind = sFWHA01_t.system_parameter.ch3_set_air;
            sflash.state ++;
            break;
        }

        break;

    case FLASH_GENERAL_DATA:
        if (flash_count % 2 != FALSE)
        {
            flash_halfword_program(A_WORK_MODE_ADDRESS, sFWHA01_t.work_mode);
			flash_halfword_program(A_TEMP_UNIT_ADDRESS, sFWHA01_t.temp_unit);
			flash_halfword_program(A_SPEAK_MODE_ADDRESS, sFWHA01_t.speak_state);
			flash_halfword_program(A_LANGUAGE_MODE_ADDRESS, sFWHA01_t.language_state);
			flash_halfword_program(A_CAL_DATA_ADDRESS, sFWHA01_t.system_parameter.cal_data);
			flash_halfword_program(A_DISPLAY_LOCK_MODE_ADDRESS, sFWHA01_t.display_lock_state);
			flash_halfword_program(A_SLEEP_MODE_ADDRESS, sFWHA01_t.sleep_state);
			flash_halfword_program(A_RUN_MODE_ADDRESS, sFWHA01_t.run_mode);
			flash_halfword_program(A_LONGKEY_MODE_ADDRESS, sFWHA01_t.long_key_mode);
			flash_halfword_program(A_SHORTKEY_MODE_ADDRESS, sFWHA01_t.short_key_mode);
			flash_halfword_program(A_ADJUSTKEY_MODE_ADDRESS, sFWHA01_t.adjust_key_mode);
			flash_halfword_program(A_COLD_WIND_DATA, sFWHA01_t.system_parameter.cold_mode_set_air);
//			flash_halfword_program(A_MODEL, sFWHA01_t.model);
        }
        else
        {
            flash_halfword_program(B_WORK_MODE_ADDRESS, sFWHA01_t.work_mode);
			flash_halfword_program(B_TEMP_UNIT_ADDRESS, sFWHA01_t.temp_unit);
			flash_halfword_program(B_SPEAK_MODE_ADDRESS, sFWHA01_t.speak_state);
			flash_halfword_program(B_LANGUAGE_MODE_ADDRESS, sFWHA01_t.language_state);
			flash_halfword_program(B_CAL_DATA_ADDRESS, sFWHA01_t.system_parameter.cal_data);
			flash_halfword_program(B_DISPLAY_LOCK_MODE_ADDRESS, sFWHA01_t.display_lock_state);
			flash_halfword_program(B_SLEEP_MODE_ADDRESS, sFWHA01_t.sleep_state);
			flash_halfword_program(B_RUN_MODE_ADDRESS, sFWHA01_t.run_mode);
			flash_halfword_program(B_LONGKEY_MODE_ADDRESS, sFWHA01_t.long_key_mode);
			flash_halfword_program(B_SHORTKEY_MODE_ADDRESS, sFWHA01_t.short_key_mode);
			flash_halfword_program(B_ADJUSTKEY_MODE_ADDRESS, sFWHA01_t.adjust_key_mode);
			flash_halfword_program(B_COLD_WIND_DATA, sFWHA01_t.system_parameter.cold_mode_set_air);
//			flash_halfword_program(B_MODEL, sFWHA01_t.model);
        }
		last_work_mode = sFWHA01_t.work_mode;
		last_temp_unit_mode = sFWHA01_t.temp_unit;
		last_speak_mode = sFWHA01_t.speak_state;
		last_language_mode = sFWHA01_t.language_state;
		last_cal_data = sFWHA01_t.system_parameter.cal_data;
		last_display_lock_mode = sFWHA01_t.display_lock_state;
		last_sleep_mode = sFWHA01_t.sleep_state;
		last_run_mode = sFWHA01_t.run_mode;
		last_longkey_mode = sFWHA01_t.long_key_mode;  
        last_shortkey_mode = sFWHA01_t.short_key_mode;  
        last_adjustkey_mode = sFWHA01_t.adjust_key_mode;
		last_cold_set_wind = sFWHA01_t.system_parameter.cold_mode_set_air;
//		last_model = sFWHA01_t.model;
		sflash.state ++;
		break;
    case FLASH_FINSH:
        if (flash_count % 2 != FALSE)
        {
             flash_halfword_program(A_FLASH_VERSION_ADDRESS, flash_version);
        }
        else
        {
             flash_halfword_program(B_FLASH_VERSION_ADDRESS, flash_version);
        }

        flash_lock();
        flash_version++;
        flash_count++;
        sflash.state = FLASH_HANDLE_DATA;
        break;
    }
}

void get_data_from_a(void)
{
	
	sFWHA01_t.system_parameter.set_temp = flash_wred_halfword(A_SET_TEMP_ADDRESS);
	sFWHA01_t.system_parameter.air_data= flash_wred_halfword(A_SET_AIR_ADDRESS);
	sFWHA01_t.system_parameter.ch1_set_temp = flash_wred_halfword(A_CH1_SET_TEMP_ADDRESS);
	sFWHA01_t.system_parameter.ch1_set_air = flash_wred_halfword(A_CH1_SET_WIND_ADDRESS);
	sFWHA01_t.system_parameter.ch2_set_temp = flash_wred_halfword(A_CH2_SET_TEMP_ADDRESS);
	sFWHA01_t.system_parameter.ch2_set_air = flash_wred_halfword(A_CH2_SET_WIND_ADDRESS);
	sFWHA01_t.system_parameter.ch3_set_temp = flash_wred_halfword(A_CH3_SET_TEMP_ADDRESS);
	sFWHA01_t.system_parameter.ch3_set_air = flash_wred_halfword(A_CH3_SET_WIND_ADDRESS);
	sFWHA01_t.work_mode = flash_wred_halfword(A_WORK_MODE_ADDRESS);
	sFWHA01_t.temp_unit = flash_wred_halfword(A_TEMP_UNIT_ADDRESS);
	sFWHA01_t.speak_state = flash_wred_halfword(A_SPEAK_MODE_ADDRESS);
	sFWHA01_t.language_state = flash_wred_halfword(A_LANGUAGE_MODE_ADDRESS);
	sFWHA01_t.system_parameter.cal_data = flash_wred_halfword(A_CAL_DATA_ADDRESS);
	sFWHA01_t.display_lock_state = flash_wred_halfword(A_DISPLAY_LOCK_MODE_ADDRESS);
	sFWHA01_t.sleep_state = flash_wred_halfword(A_SLEEP_MODE_ADDRESS);
	sFWHA01_t.run_mode = flash_wred_halfword(A_RUN_MODE_ADDRESS);
	sFWHA01_t.long_key_mode = flash_wred_halfword(A_LONGKEY_MODE_ADDRESS);  
    sFWHA01_t.short_key_mode = flash_wred_halfword(A_SHORTKEY_MODE_ADDRESS);  
	sFWHA01_t.adjust_key_mode = flash_wred_halfword(A_ADJUSTKEY_MODE_ADDRESS); 
	sFWHA01_t.system_parameter.cold_mode_set_air = flash_wred_halfword(A_COLD_WIND_DATA); 
//	sFWHA01_t.model = flash_wred_halfword(A_MODEL); 
}
void get_data_from_b(void)
{
	
	sFWHA01_t.system_parameter.set_temp = flash_wred_halfword(B_SET_TEMP_ADDRESS);
	sFWHA01_t.system_parameter.air_data= flash_wred_halfword(B_SET_AIR_ADDRESS);
	sFWHA01_t.system_parameter.ch1_set_temp = flash_wred_halfword(B_CH1_SET_TEMP_ADDRESS);
	sFWHA01_t.system_parameter.ch1_set_air = flash_wred_halfword(B_CH1_SET_WIND_ADDRESS);
	sFWHA01_t.system_parameter.ch2_set_temp = flash_wred_halfword(B_CH2_SET_TEMP_ADDRESS);
	sFWHA01_t.system_parameter.ch2_set_air = flash_wred_halfword(B_CH2_SET_WIND_ADDRESS);
	sFWHA01_t.system_parameter.ch3_set_temp = flash_wred_halfword(B_CH3_SET_TEMP_ADDRESS);
	sFWHA01_t.system_parameter.ch3_set_air = flash_wred_halfword(B_CH3_SET_WIND_ADDRESS);
	sFWHA01_t.work_mode = flash_wred_halfword(B_WORK_MODE_ADDRESS);
	sFWHA01_t.temp_unit = flash_wred_halfword(B_TEMP_UNIT_ADDRESS);
	sFWHA01_t.speak_state = flash_wred_halfword(B_SPEAK_MODE_ADDRESS);
	sFWHA01_t.language_state = flash_wred_halfword(B_LANGUAGE_MODE_ADDRESS);
	sFWHA01_t.system_parameter.cal_data = flash_wred_halfword(B_CAL_DATA_ADDRESS);
	sFWHA01_t.display_lock_state = flash_wred_halfword(B_DISPLAY_LOCK_MODE_ADDRESS);
	sFWHA01_t.sleep_state = flash_wred_halfword(B_SLEEP_MODE_ADDRESS);
	sFWHA01_t.run_mode = flash_wred_halfword(B_RUN_MODE_ADDRESS);
	sFWHA01_t.long_key_mode = flash_wred_halfword(B_LONGKEY_MODE_ADDRESS);  
    sFWHA01_t.short_key_mode = flash_wred_halfword(B_SHORTKEY_MODE_ADDRESS);  
	sFWHA01_t.adjust_key_mode = flash_wred_halfword(B_ADJUSTKEY_MODE_ADDRESS); 
	sFWHA01_t.system_parameter.cold_mode_set_air = flash_wred_halfword(B_COLD_WIND_DATA); 
//	sFWHA01_t.model = flash_wred_halfword(B_MODEL); 
}

void get_reset_data(void)
{
	sFWHA01_t.system_parameter.set_temp = 380;
	sFWHA01_t.system_parameter.air_data= 80;
	sFWHA01_t.system_parameter.cold_mode_set_air= 100;
	sFWHA01_t.system_parameter.ch1_set_temp = 320;
	sFWHA01_t.system_parameter.ch1_set_air = 50;
	sFWHA01_t.system_parameter.ch2_set_temp = 350;
	sFWHA01_t.system_parameter.ch2_set_air = 60;
	sFWHA01_t.system_parameter.ch3_set_temp = 380;
	sFWHA01_t.system_parameter.ch3_set_air = 80;
	sFWHA01_t.work_mode = WORK_NORMAL;
	sFWHA01_t.temp_unit = CELSIUS;
	sFWHA01_t.speak_state = SPEAKER_OPEN;
	sFWHA01_t.language_state = CHINESE;
	sFWHA01_t.system_parameter.cal_data = 0;
	sFWHA01_t.display_lock_state = LOCK;
	sFWHA01_t.sleep_state = SLEEP_OPEN;
	sFWHA01_t.run_mode = Standard_Mode;
	sFWHA01_t.long_key_mode = COLDWIND_MODE;
	sFWHA01_t.short_key_mode = CHANNEL_SWITCHING;
	sFWHA01_t.adjust_key_mode = SELECT_TEMP;
//	sFWHA01_t.model = HA04;
	
}


void check_data_all(void)
{
	if(sFWHA01_t.system_parameter.set_temp > MAX_SET_TEMP || sFWHA01_t.system_parameter.set_temp < MIN_SET_TEMP)
	{
		sFWHA01_t.system_parameter.set_temp = 380;
	}
	
	if(sFWHA01_t.system_parameter.air_data > MAX_SET_AIR || sFWHA01_t.system_parameter.air_data < MIN_SET_AIR)
	{
		sFWHA01_t.system_parameter.air_data = 80;
	}
	
	if(sFWHA01_t.system_parameter.cold_mode_set_air > MAX_SET_AIR || sFWHA01_t.system_parameter.cold_mode_set_air < MIN_SET_AIR)
	{
		sFWHA01_t.system_parameter.cold_mode_set_air = 100;
	}
	
	if(sFWHA01_t.system_parameter.ch1_set_temp > MAX_SET_TEMP || sFWHA01_t.system_parameter.ch1_set_temp < MIN_SET_TEMP)
	{
		sFWHA01_t.system_parameter.ch1_set_temp = 320;
	}
	if(sFWHA01_t.system_parameter.ch2_set_temp > MAX_SET_TEMP || sFWHA01_t.system_parameter.ch2_set_temp < MIN_SET_TEMP)
	{
		sFWHA01_t.system_parameter.ch2_set_temp = 350;
	}
	if(sFWHA01_t.system_parameter.ch3_set_temp > MAX_SET_TEMP || sFWHA01_t.system_parameter.ch3_set_temp < MIN_SET_TEMP)
	{
		sFWHA01_t.system_parameter.ch3_set_temp = 380;
	}
	
	if(sFWHA01_t.system_parameter.ch1_set_air > MAX_SET_AIR || sFWHA01_t.system_parameter.ch1_set_air < MIN_SET_AIR)
	{
		sFWHA01_t.system_parameter.ch1_set_air = 50;
	}
	if(sFWHA01_t.system_parameter.ch2_set_air > MAX_SET_AIR || sFWHA01_t.system_parameter.ch2_set_air < MIN_SET_AIR)
	{
		sFWHA01_t.system_parameter.ch2_set_air = 60;
	}
	if(sFWHA01_t.system_parameter.ch3_set_air > MAX_SET_AIR || sFWHA01_t.system_parameter.ch3_set_air < MIN_SET_AIR)
	{
		sFWHA01_t.system_parameter.ch3_set_air = 80;
	}
	
	if(sFWHA01_t.work_mode != WORK_CURVE && sFWHA01_t.work_mode != WORK_NORMAL)
	{
		sFWHA01_t.work_mode = WORK_NORMAL;
	}
	
	if(sFWHA01_t.temp_unit != CELSIUS && sFWHA01_t.temp_unit != FAHRENHEIT)
	{
		sFWHA01_t.temp_unit = CELSIUS;
	}
	
	if(sFWHA01_t.speak_state != SPEAKER_CLOSE && sFWHA01_t.speak_state != SPEAKER_OPEN)
	{
		sFWHA01_t.speak_state = SPEAKER_OPEN;
	}
	
	if(sFWHA01_t.language_state != CHINESE && sFWHA01_t.language_state != ENGLISH)
	{
		sFWHA01_t.language_state = CHINESE;
	}
	
	if(sFWHA01_t.system_parameter.cal_data > MAX_CAL_TEMP || sFWHA01_t.system_parameter.cal_data < MIN_CAL_TEMP)
	{
		sFWHA01_t.system_parameter.cal_data = 0;
	}
	
	if(sFWHA01_t.display_lock_state != LOCK && sFWHA01_t.display_lock_state != UNLOCK)
	{
		sFWHA01_t.display_lock_state = UNLOCK;
	}
	
	if(sFWHA01_t.sleep_state != SLEEP_OPEN)
	{
		sFWHA01_t.sleep_state = SLEEP_OPEN;
	}
	
	if(sFWHA01_t.run_mode != Standard_Mode && sFWHA01_t.run_mode != Power_Mode)
	{
		sFWHA01_t.run_mode = Standard_Mode;
	}
	
	if(sFWHA01_t.long_key_mode != COLDWIND_MODE && sFWHA01_t.long_key_mode != POWER_MODE)
	{
		sFWHA01_t.long_key_mode = COLDWIND_MODE;
	}
	
	if(sFWHA01_t.short_key_mode != COLDWIND_MODE && sFWHA01_t.short_key_mode != POWER_MODE && sFWHA01_t.short_key_mode != CHANNEL_SWITCHING)
	{
		sFWHA01_t.short_key_mode = CHANNEL_SWITCHING;
	}
	
	if(sFWHA01_t.adjust_key_mode != SELECT_TEMP && sFWHA01_t.adjust_key_mode != SELECT_WIND && sFWHA01_t.adjust_key_mode != SELECT_CH)
	{
		sFWHA01_t.adjust_key_mode = SELECT_TEMP;
	}
		
//	if(sFWHA01_t.model != HA04 && sFWHA01_t.model != HA05)
//	{
//		sFWHA01_t.model = HA04;
//	}	
//	
}