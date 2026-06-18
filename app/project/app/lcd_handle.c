#include "lcd_handle.h"
#include "work_handle.h"
#include "HA01_handle.h"
#include "wk_system.h"

void page_switch(void);
void show_navigation_bar(void);
static void show_channel_state(void);
static void show_channel_value(void);
static void show_temp(void);
static void show_output(HA01_Handle *this);
void show_curve(HA01_Handle *this);
void show_cal_temp(void);
void show_icon(void);
static void number_change(void);
static void show_fan(HA01_Handle *this);
static void show_work_state_icon(HA01_Handle *this);
uint8_t actual_temp_refesh_time = 0;
uint8_t actual_wind_refesh_time = 0;
static float display_air = 0;
static float display_temp = 0;
float wind_to_display = 0;
static uint8_t last_sleep_state = 0;
int temp_to_display = 0x00;
int16_t last_output_value = -1;  // 用 -1 确保第一次刷新
uint8_t last_coldwind_state = 0;
uint8_t first_draw = false;
uint8_t lock_temp_flag = false;

static number temp_actual =
	{
		.x = 85,
		.y = 20,
		.sizey = 128,
		.num = &sFWHA01_t.system_parameter.actual_temp,
		.fc = WHITE,
		.bc = BLACK,
		.len = 3,
		.mode = 0,
};

static number temp_set =
	{
		.x = 85,
		.y = 20,
		.sizey = 128,
		.num = &sFWHA01_t.system_parameter.set_temp,
		.fc = WHITE,
		.bc = BLACK,
		.len = 3,
		.mode = 0,

};

static number wind_set =
	{
		.x = 110,
		.y = 180,
		.sizey = 64,
		.num = &sFWHA01_t.system_parameter.air_data,
		.fc = WHITE,
		.bc = BLACK,
		.len = 3,
		.mode = 0,

};

static number vision_2 =
	{
		.x = 415,
		.y = 8,
		//	.num = 'v:1.10',
		.len = 7,
		.fc = WHITE,
		.bc = 0x3186,
		.sizey = 16,
		.mode = 0,
};

static number vision_1 =
	{
		.x = 236,
		.y = 131,
		//	.num = 'v:1.10',
		.len = 7,
		.fc = WHITE,
		.bc = 0x3166,
		.sizey = 32,
		.mode = 0,
};

static icon_t beep =
	{
		.x1 = 398,
		.y1 = 6,
		.length = 21,
		.winth = 21,

};
static icon_t ch1_frame =
	{
		.x1 = 90,
		.y1 = 258,
		.length = 93,
		.winth = 48,
};
static icon_t ch2_frame =
	{
		.x1 = 193,
		.y1 = 258,
		.length = 93,
		.winth = 48,
};
static icon_t ch3_frame =
	{
		.x1 = 296,
		.y1 = 258,
		.length = 93,
		.winth = 48,
};

static icon_t ch1_frame_select =
	{
		.x1 = 90,
		.y1 = 258,
		.length = 93,
		.winth = 48,
};
static icon_t ch2_frame_select =
	{
		.x1 = 193,
		.y1 = 258,
		.length = 93,
		.winth = 48,
};
static icon_t ch3_frame_select =
	{
		.x1 = 296,
		.y1 = 258,
		.length = 93,
		.winth = 48,
};

static number ch1_set_temp_1 =
	{
		.x = 96,
		.y = 280,
		.num = &sFWHA01_t.system_parameter.ch1_set_temp,
		.len = 3,
		.fc = WHITE,
		.bc = 0x3186,
		.sizey = 24,
		.mode = 0,
};

static number ch2_set_temp_1 =
	{
		.x = 199,
		.y = 280,
		.num = &sFWHA01_t.system_parameter.ch2_set_temp,
		.len = 3,
		.fc = WHITE,
		.bc = 0x3186,
		.sizey = 24,
		.mode = 0,
};

static number ch3_set_temp_1 =
	{
		.x = 302,
		.y = 280,
		.num = &sFWHA01_t.system_parameter.ch3_set_temp,
		.len = 3,
		.fc = WHITE,
		.bc = 0x3186,
		.sizey = 24,
		.mode = 0,
};
static number ch1_set_air_1 =
	{
		.x = 141,
		.y = 280,
		.num = &sFWHA01_t.system_parameter.ch1_set_air,
		.len = 3,
		.fc = WHITE,
		.bc = 0x3186,
		.sizey = 24,
		.mode = 0,
};

static number ch2_set_air_1 =
	{
		.x = 244,
		.y = 280,
		.num = &sFWHA01_t.system_parameter.ch2_set_air,
		.len = 3,
		.fc = WHITE,
		.bc = 0x3186,
		.sizey = 24,
		.mode = 0,
};
static number ch3_set_air_1 =
	{
		.x = 347,
		.y = 280,
		.num = &sFWHA01_t.system_parameter.ch3_set_air,
		.len = 3,
		.fc = WHITE,
		.bc = 0x3186,
		.sizey = 24,
		.mode = 0,
};

static icon_t set_temp_icon =
	{
		.x1 = 90,
		.y1 = 210,
		.length = 123,
		.winth = 28,
};
static icon_t actual_temp_icon =
	{
		.x1 = 90,
		.y1 = 210,
		.length = 123,
		.winth = 28,
};
static icon_t set_air_icon =
	{
		.x1 = 270,
		.y1 = 212,
		.length = 123,
		.winth = 25,
};
static icon_t actual_air_icon =
	{
		.x1 = 270,
		.y1 = 212,
		.length = 123,
		.winth = 25,
};
static number temp_main =
	{
		.x = 80,
		.y = 120,
		.len = 3,
		.fc = WHITE,
		.bc = 0x18a3,
		.sizey = 80,
		.mode = 0,
};

static number air_main =
	{
		.x = 270,
		.y = 120,
		.len = 3,
		.fc = WHITE,
		.bc = 0x18a3,
		.sizey = 80,
		.mode = 0,
};
static number temp_main_curve =
	{
		.x = 393,
		.y = 80,
		.len = 3,
		.fc = WHITE,
		.bc = 0x3186,
		.sizey = 32,
		.mode = 0,
};

static number air_main_curve =
	{
		.x = 404,
		.y = 205,
		.len = 3,
		.fc = WHITE,
		.bc = 0x3186,
		.sizey = 32,
		.mode = 0,
};

static icon_t return_icon =
	{
		.x1 = 196,
		.y1 = 250,
		.length = 88,
		.winth = 45,
};


	
static icon_t exit_icon =
	{
		.x1 = 90,
		.y1 = 210,
		.length = 119,
		.winth = 43,
};

static icon_t confirm_icon =
{
	.x1 = 205,
	.y1 = 190,
	.length = 70,
	.winth = 32,

};
	
static icon_t sleep_icon = 
{
	.x1 = 165,
	.y1 = 40,
	.length = 150,
	.winth = 75,
};

static icon_t over_hot_icon = 
{
	.x1 = 205,
	.y1 = 190,
	.length = 235,
	.winth = 103,
};

static icon_t low_hot_icon = 
{
	.x1 = 205,
	.y1 = 190,
	.length = 235,
	.winth = 103,
};

static icon_t error_fan_icon = 
{
	.x1 = 205,
	.y1 = 190,
	.length = 235,
	.winth = 103,
};

static icon_t power_icon = 
{
	.x1 = 355,
	.y1 = 3,
	.length = 42,
	.winth = 24,
	
};

void LcdProc(void)
{
	static uint8_t run_step = 0;
	
	switch(run_step)
	{
		case 0:
			show_fan(&sFWHA01_t);
			page_switch();
			show_temp();
			show_curve(&sFWHA01_t);
			show_channel_state();
			show_channel_value();
			run_step++;
		break;
		case 1:
			show_navigation_bar();
			show_work_state_icon(&sFWHA01_t);
			show_output(&sFWHA01_t);
			show_cal_temp();
			number_change();
			show_icon();
			run_step = 0;
		break;
		default:break;
		
	}
}

void show_icon(void)
{
	 static uint8_t show_step = 0;
	static uint8_t display_number = 0;
	static int display_times = 50;
	
	
	switch (show_step)
    {
		case 0:
		/*  ℃ ℉ */
		if (sFWHA01_t.last_temp_unit != sFWHA01_t.temp_unit)
		{
			/* main uint */
			if (sFWHA01_t.page == WORK_PAGE)
			{
				if (sFWHA01_t.temp_unit == CELSIUS)
					TranferPicturetoTFT_LCD(200, 170, 20, 21, WORK_CELSIUS_ICON);
				else if (sFWHA01_t.temp_unit == FAHRENHEIT)
					TranferPicturetoTFT_LCD(200, 170, 20, 21, WORK_FAHRENHEIT_ICON);
			}
			else if (sFWHA01_t.page == CURVE_PAGE)
			{
				if (sFWHA01_t.temp_unit == CELSIUS)
					TranferPicturetoTFT_LCD(440, 84, 26, 24, CURVE_CELSIUS_ICON);
				else if (sFWHA01_t.temp_unit == FAHRENHEIT)
					TranferPicturetoTFT_LCD(440, 84, 26, 24, CURVE_FAHRENHEIT_ICON);
			}
			else if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN || sFWHA01_t.page == SET_TEMP_CAL_PAGE_CN)
			{
				if (sFWHA01_t.temp_unit == CELSIUS)
					TranferPicturetoTFT_LCD(320, 153, 28, 27, CAL_CELSIUS_ICON);
				else if (sFWHA01_t.temp_unit == FAHRENHEIT)
					TranferPicturetoTFT_LCD(320, 153, 28, 27, CAL_FAHRENHEIT_ICON);
			}
			sFWHA01_t.last_temp_unit = sFWHA01_t.temp_unit;
		}
		 show_step++;
        break;
		case 1:
			/* select icon */
			if (sFWHA01_t.page == SET_RUN_PAGE_CN ||
				sFWHA01_t.page == SET_UNIT_PAGE_CN ||
				sFWHA01_t.page == SET_SPEAK_PAGE_CN ||
				sFWHA01_t.page == SET_TEMP_LOCK_PAGE_CN ||
				sFWHA01_t.page == SET_SLEEP_PAGE_CN ||
				sFWHA01_t.page == SET_LONG_KEY_PAGE_CN ||
				sFWHA01_t.page == SET_SHORT_KEY_PAGE_CN ||
				sFWHA01_t.page == SET_KEY_MODE_PAGE_CN)
			{
				if (sFWHA01_t.page == SET_RUN_PAGE_CN)
				{
					if (sFWHA01_t.set_run_mode == Standard_Mode)
					{
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_UNSELECT_ICON);
					}
					else if (sFWHA01_t.set_run_mode == Power_Mode)
					{
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_UNSELECT_ICON);
					}
				}

				if (sFWHA01_t.page == SET_UNIT_PAGE_CN)
				{
					if (sFWHA01_t.set_temp_unit == FAHRENHEIT)
					{
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_UNSELECT_ICON);
					}
					else if (sFWHA01_t.set_temp_unit == CELSIUS)
					{
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_UNSELECT_ICON);
					}
				}

				if (sFWHA01_t.page == SET_SPEAK_PAGE_CN)
				{
					if (sFWHA01_t.set_speak_state == SPEAKER_CLOSE)
					{
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_UNSELECT_ICON);
					}
					else if (sFWHA01_t.set_speak_state == SPEAKER_OPEN)
					{
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_UNSELECT_ICON);
					}
				}

				if (sFWHA01_t.page == SET_TEMP_LOCK_PAGE_CN)
				{
					if (sFWHA01_t.set_display_lock_state == LOCK)
					{
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_UNSELECT_ICON);
					}
					else if (sFWHA01_t.set_display_lock_state == UNLOCK)
					{
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_UNSELECT_ICON);
					}
				}

				if (sFWHA01_t.page == SET_SLEEP_PAGE_CN)
				{
					if (sFWHA01_t.set_sleep_state == SLEEP_CLOSE)
					{
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_UNSELECT_ICON);
					}
					else if (sFWHA01_t.set_sleep_state == SLEEP_OPEN)
					{
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_UNSELECT_ICON);
					}
				}
				
				if(sFWHA01_t.page == SET_LONG_KEY_PAGE_CN)
				{
					if(sFWHA01_t.set_long_key_mode == COLDWIND_MODE)
					{
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_UNSELECT_ICON);
					}
					else if(sFWHA01_t.set_long_key_mode == POWER_MODE)
					{
						TranferPicturetoTFT_LCD(340, 177, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 137, 37, 30, MENU_UNSELECT_ICON);
					}
					
				}
				if(sFWHA01_t.page == SET_SHORT_KEY_PAGE_CN)
				{
					if(sFWHA01_t.set_short_key_mode == CHANNEL_SWITCHING)
					{
						TranferPicturetoTFT_LCD(340, 117, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 158, 37, 30, MENU_UNSELECT_ICON);
						TranferPicturetoTFT_LCD(340, 199, 37, 30, MENU_UNSELECT_ICON);
						
					}
					else if(sFWHA01_t.set_short_key_mode == COLDWIND_MODE)
					{
						TranferPicturetoTFT_LCD(340, 117, 37, 30, MENU_UNSELECT_ICON);
						TranferPicturetoTFT_LCD(340, 158, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 199, 37, 30, MENU_UNSELECT_ICON);
					}
					else if(sFWHA01_t.set_short_key_mode == POWER_MODE)
					{
						TranferPicturetoTFT_LCD(340, 117, 37, 30, MENU_UNSELECT_ICON);
						TranferPicturetoTFT_LCD(340, 158, 37, 30, MENU_UNSELECT_ICON);
						TranferPicturetoTFT_LCD(340, 199, 37, 30, MENU_SELECT_ICON);
					}
				}
				if(sFWHA01_t.page == SET_KEY_MODE_PAGE_CN)
				{
					if(sFWHA01_t.set_adjust_key_mode == SELECT_TEMP)
					{
						TranferPicturetoTFT_LCD(340, 117, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 158, 37, 30, MENU_UNSELECT_ICON);
						TranferPicturetoTFT_LCD(340, 199, 37, 30, MENU_UNSELECT_ICON);
					}
					else if(sFWHA01_t.set_adjust_key_mode == SELECT_WIND)
					{
						TranferPicturetoTFT_LCD(340, 117, 37, 30, MENU_UNSELECT_ICON);
						TranferPicturetoTFT_LCD(340, 158, 37, 30, MENU_SELECT_ICON);
						TranferPicturetoTFT_LCD(340, 199, 37, 30, MENU_UNSELECT_ICON);
					}
					else if(sFWHA01_t.set_adjust_key_mode == SELECT_CH)
					{
						TranferPicturetoTFT_LCD(340, 117, 37, 30, MENU_UNSELECT_ICON);
						TranferPicturetoTFT_LCD(340, 158, 37, 30, MENU_UNSELECT_ICON);
						TranferPicturetoTFT_LCD(340, 199, 37, 30, MENU_SELECT_ICON);
					}
				}
			}
			 show_step++;
        break;
		case 2:
			/* arrow icon */
			if(sFWHA01_t.page == SET_KEY_PAGE_CN)
			{
				if(sFWHA01_t.set_key_mode == LONGKEY_MODE)
				{
					TranferPicturetoTFT_LCD(340, 120, 20, 20, SELECT_ARROW);
					TranferPicturetoTFT_LCD(340, 161, 20, 20, NOSELECT_ARROW);
					TranferPicturetoTFT_LCD(340, 202, 20, 20, NOSELECT_ARROW);
				}
				else if(sFWHA01_t.set_key_mode == SHORTKEY_MODE)
				{
					TranferPicturetoTFT_LCD(340, 120, 20, 20, NOSELECT_ARROW);
					TranferPicturetoTFT_LCD(340, 161, 20, 20, SELECT_ARROW);
					TranferPicturetoTFT_LCD(340, 202, 20, 20, NOSELECT_ARROW);
				}
				else if(sFWHA01_t.set_key_mode == ADJUSTKEY_MODE)
				{
					TranferPicturetoTFT_LCD(340, 120, 20, 20, NOSELECT_ARROW);
					TranferPicturetoTFT_LCD(340, 161, 20, 20, NOSELECT_ARROW);
					TranferPicturetoTFT_LCD(340, 202, 20, 20, SELECT_ARROW);
				}
				else if(sFWHA01_t.set_key_mode == RETURN_MENU)
				{
					TranferPicturetoTFT_LCD(340, 120, 20, 20, NOSELECT_ARROW);
					TranferPicturetoTFT_LCD(340, 161, 20, 20, NOSELECT_ARROW);
					TranferPicturetoTFT_LCD(340, 202, 20, 20, NOSELECT_ARROW);
				}
			}
		 show_step++;
        break;
		case 3:
			/* return icon */
			if (sFWHA01_t.page == SET_TEMP_CAL_PAGE_CN ||
				sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN ||
				sFWHA01_t.page == SET_VERSION_PAGE_CN ||
				sFWHA01_t.page == SET_RESET_PAGE_CN ||
				sFWHA01_t.page == SET_RESET_DONE_PAGE_CN ||
				sFWHA01_t.page == SET_KEY_PAGE_CN )
			{
				/* RETU																																																																																																																																																																																											RN_ICON */
				if (sFWHA01_t.page == SET_TEMP_CAL_PAGE_CN ||
					sFWHA01_t.page == SET_VERSION_PAGE_CN ||
					(sFWHA01_t.page == SET_RESET_PAGE_CN && sFWHA01_t.set_reset1 == RESET_RETURN) ||
					sFWHA01_t.page == SET_RESET_DONE_PAGE_CN)
				{
					TranferPicturetoTFT_LCD(return_icon.x1, return_icon.y1, return_icon.length, return_icon.winth, RETURN_ICON_CN);
				}

				/* UN_RETURN_ICON */
				if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN ||
					(sFWHA01_t.page == SET_RESET_PAGE_CN && sFWHA01_t.set_reset1 == RESET_CONFIRN))
				{
					TranferPicturetoTFT_LCD(return_icon.x1, return_icon.y1, return_icon.length, return_icon.winth, UN_RETURN_ICON_CN);
				}
				
				if(sFWHA01_t.page == SET_KEY_PAGE_CN)
				{
					if(sFWHA01_t.set_key_mode == RETURN_MENU)
					{
						TranferPicturetoTFT_LCD(return_icon.x1, return_icon.y1 + 5, return_icon.length, return_icon.winth, RETURN_ICON_CN);
					}
					else
						TranferPicturetoTFT_LCD(return_icon.x1, return_icon.y1 + 5, return_icon.length, return_icon.winth, UN_RETURN_ICON_CN);
				}
			}
			 show_step++;
        break;
		case 4:
			/*confirm icon */
			if (sFWHA01_t.page == SET_RESET_PAGE_CN)
			{
				if (sFWHA01_t.set_reset1 == RESET_CONFIRN)
					TranferPicturetoTFT_LCD(confirm_icon.x1, confirm_icon.y1, confirm_icon.length, confirm_icon.winth, CONFIRM_ICON_CN);
				else if (sFWHA01_t.set_reset1 == RESET_RETURN)
					TranferPicturetoTFT_LCD(confirm_icon.x1, confirm_icon.y1, confirm_icon.length, confirm_icon.winth, UNCONFIRM_ICON_CN);
			}

			/* vision */
			if (sFWHA01_t.page == SET_VERSION_PAGE_CN)
			{
				/* software vision*/
				LCD_VISION(vision_1.x, vision_1.y, vision_1.len, vision_1.fc, vision_1.bc, vision_1.sizey);

				/* hardware vision*/
				LCD_VISION_1(vision_1.x, vision_1.y + 50, vision_1.len, vision_1.fc, vision_1.bc, vision_1.sizey);
			}
		 show_step++;
        break;
		case 5:
			/* sleep icon */
			if(last_sleep_state != sFWHA01_t.Work_handle_state && sFWHA01_t.run_mode != Cold_Mode)
			{
				last_sleep_state = sFWHA01_t.Work_handle_state;
				if(sFWHA01_t.Work_handle_state == HANDLE_SLEEP && sFWHA01_t.page == WORK_PAGE && sFWHA01_t.run_mode != Cold_Mode)
				{
					TranferPicturetoTFT_LCD(sleep_icon.x1,sleep_icon.y1,sleep_icon.length,sleep_icon.winth,SLEEP_STATE_CN);
				}		
				else if(sFWHA01_t.Work_handle_state == HANDLE_WORKING && sFWHA01_t.page == WORK_PAGE)
				{
					TranferPicturetoTFT_LCD(sleep_icon.x1,sleep_icon.y1,sleep_icon.length,sleep_icon.winth,REFRESH_SLEEP_ICON);
					last_output_value = -1;
					//刷新功率条
				}
			}
		 show_step++;
        break;
		case 6:
			/* coldwind icon */
			if(last_coldwind_state == Cold_Mode && sFWHA01_t.run_mode != Cold_Mode)
			{
				last_coldwind_state = sFWHA01_t.run_mode;
				if(sFWHA01_t.page == WORK_PAGE)
				{
					TranferPicturetoTFT_LCD(145,80,190,35,REFRESH_COLDWIND_ICON);
				}
			}
			else if(last_coldwind_state != Cold_Mode && sFWHA01_t.run_mode == Cold_Mode)
			{
				
				last_coldwind_state = sFWHA01_t.run_mode;
				if(sFWHA01_t.page == WORK_PAGE && sFWHA01_t.run_mode == Cold_Mode)
				{
					if(sFWHA01_t.language_state == CHINESE)
						TranferPicturetoTFT_LCD(sleep_icon.x1,sleep_icon.y1,sleep_icon.length,sleep_icon.winth,REFRESH_SLEEP_ICON);
					else if(sFWHA01_t.language_state == ENGLISH)
						TranferPicturetoTFT_LCD(sleep_icon.x1,sleep_icon.y1,sleep_icon.length,sleep_icon.winth,REFRESH_SLEEP_ICON_ENG);
					TranferPicturetoTFT_LCD(145,80,190,35,COLD_MODE_ICON_CN);

				}
			}
			
			
//			if(sFWHA01_t.general_parameter.save_ch_flag == true)
//			{
//				TranferPicturetoTFT_LCD(165,135,150,68,SAVE_ICON_CN);
//				display_times--;
//				if(display_times <= 0)	
//				{
//					if(sFWHA01_t.page == CURVE_PAGE)
//					{
//						display_times = 25;
//					}
//					else
//					{
//						display_times = SAVE_CH_TIME;
//					}
//					
//					sFWHA01_t.general_parameter.save_ch_flag = false;
//					
//					sFWHA01_t.system_parameter.last_air_data_actual = RESET_VALUE;
//					sFWHA01_t.system_parameter.last_sleep_air_data = RESET_VALUE;
//					sFWHA01_t.system_parameter.last_actual_temp = RESET_VALUE; 
//					sFWHA01_t.system_parameter.last_actual_temp_f_display = RESET_VALUE;
//					actual_temp_refesh_time = 0x00;
//					sFWHA01_t.last_page = RESET_VALUE;
//				}
//			}
			 show_step = 0;
        break;
			default:
        break;
		}
}

static void show_work_state_icon(HA01_Handle *this)
{
	static uint8_t show_step = 0;
	static page_e last_p = RESET_VALUE;
	
//	if(this->handle_error_state != HANDLE_OK)
//	{
//		if(this->page != WORK_PAGE || this->page != CURVE_PAGE)
//		{
//			this->last_handle_error_state = RESET_VALUE;
//		}
//	}
	
	if (this->page == WORK_PAGE || this->page == CURVE_PAGE)
	{
		if (this->last_handle_error_state != this->handle_error_state)
		{
			if (this->handle_error_state == HANDLE_OK)
			{
				/* refresh page clean error page */
				this->last_page = LOGO;
			}
			else if (this->handle_error_state == HANDLE_OVER_TEMP_ERR)
			{
				TranferPicturetoTFT_LCD(123,123,235,103,OVER_TEMP_ICON_CN);

			}
			else if (this->handle_error_state == HANDLE_LOW_TEMP_ERR)
			{
				TranferPicturetoTFT_LCD(123,123,235,103,LOW_TEMP_ICON_CN);
			}
			else if (this->handle_error_state == HANDLE_FAN_ERROR)
			{
				TranferPicturetoTFT_LCD(123,123,235,103,FAN_ABNORMAL_ICON_CN);
			}

			this->last_handle_error_state = this->handle_error_state;
		}
	}
}

void show_cal_temp(void)
{
    // 如果当前不在校准页面，直接更新显示值为当前设定温度
    if (sFWHA01_t.page != SET_SELECT_TEMP_CAL_PAGE_CN && sFWHA01_t.page != SET_TEMP_CAL_PAGE_CN)
    {
        sFWHA01_t.system_parameter.cal_temp_c_display = sFWHA01_t.system_parameter.set_temp;
        sFWHA01_t.system_parameter.cal_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display;
    }

    // 检查是否需要刷新显示
    if (sFWHA01_t.system_parameter.last_cal_temp_c_display == sFWHA01_t.system_parameter.cal_temp_c_display &&
        sFWHA01_t.system_parameter.last_cal_temp_f_display == sFWHA01_t.system_parameter.cal_temp_f_display)
    {
        return; // 无需刷新
    }

    // 更新上次显示值
    sFWHA01_t.system_parameter.last_cal_temp_c_display = sFWHA01_t.system_parameter.cal_temp_c_display;
    sFWHA01_t.system_parameter.last_cal_temp_f_display = sFWHA01_t.system_parameter.cal_temp_f_display;

    // 确定显示的温度值
    uint16_t display_temp = (sFWHA01_t.temp_unit == CELSIUS)
                            ? sFWHA01_t.system_parameter.cal_temp_c_display
                            : sFWHA01_t.system_parameter.cal_temp_f_display;

    // 确定显示颜色
    uint16_t color = WHITE;
    if (sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN && sFWHA01_t.set_cal == SET_CAL)
    {
        color = GREEN;
    }

    // 显示温度
    if (sFWHA01_t.page == SET_TEMP_CAL_PAGE_CN ||
        sFWHA01_t.page == SET_SELECT_TEMP_CAL_PAGE_CN)
    {
        LCD_ShowIntNum(270, 150, display_temp, 3, color, 0x3166, 32, 0);
    }
}
void show_curve(HA01_Handle *this)
{
    static bool first_in = false;
    static uint8_t bar_value = 0;
    static uint8_t air_value = 0;
	static uint16_t display_air = 0;
    if (this->page == CURVE_PAGE)
    {
        if (!first_in)
        {
            // 清空曲线区域
            LCD_Fill(18, 41, 376, 250, BLACK);
            first_in = true;
        }

        int16_t display_temp = (sFWHA01_t.run_mode == Power_Mode) 
                               ? sFWHA01_t.system_parameter.actual_temp - POWER_TEMP
                               : sFWHA01_t.system_parameter.actual_temp;
		if(display_temp < sFWHA01_t.system_parameter.cpu_temp)
			display_temp = sFWHA01_t.system_parameter.cpu_temp;
		
		if(this->handle_position == IN_POSSITION)
		{
			if(this->sleep_state == SLEEP_CLOSE)
			{
				display_air = this->system_parameter.air_data;
			}
			else
			{
				display_air = this->system_parameter.sleep_air_data;
			}
		}
		else
		{
			display_air = this->system_parameter.air_data;
		}
		
		if(this->Work_handle_state == HANDLE_SLEEP || this->system_parameter.actual_air < 30)
		{
			display_air = 0;
		}
		else if(sFWHA01_t.run_mode == Cold_Mode)
		{
			display_air = this->system_parameter.cold_mode_set_air;
		}
//		if(this->Work_handle_state == HANDLE_SLEEP || this->system_parameter.actual_air < 30)
//		{
//			display_air = this->system_parameter.actual_air;
//		}
//		else
//		{
//			if(sFWHA01_t.run_mode == Cold_Mode)
//				display_air = this->system_parameter.cold_mode_set_air;
//			else
//				display_air = this->system_parameter.air_data;
//			
//		}

        LCD_Show_Curve(18, 40, 376, 250, display_temp, display_air,
                       this->system_parameter.temp_buff, this->system_parameter.air_buff);
        LCD_Show_Grid(18, 40, 376, 250, 0x840f, this->temp_unit);

        return;
    }
    else if (this->page == WORK_PAGE)
    {
        // 显示温度进度条
        if ((sFWHA01_t.system_parameter.last_curve_actual_temp != sFWHA01_t.system_parameter.actual_temp) ||
            (sFWHA01_t.system_parameter.last_curve_actual_temp_f_display != sFWHA01_t.system_parameter.actual_temp_f_display ||
			sFWHA01_t.general_parameter.set_temp_time != 0x00))
        {
            sFWHA01_t.system_parameter.last_curve_actual_temp = sFWHA01_t.system_parameter.actual_temp;
            sFWHA01_t.system_parameter.last_curve_actual_temp_f_display = sFWHA01_t.system_parameter.actual_temp_f_display;

			if(sFWHA01_t.general_parameter.set_temp_time != 0x00)
			{
				if (this->temp_unit == CELSIUS)
					bar_value = sFWHA01_t.system_parameter.set_temp * 0.2;
				else
					bar_value = (sFWHA01_t.system_parameter.set_temp_f_display- 122) * 0.108;
			}
			else
			{
				if (this->temp_unit == CELSIUS)
					bar_value = (sFWHA01_t.run_mode == Power_Mode)
								? (sFWHA01_t.system_parameter.actual_temp - POWER_TEMP) * 0.2
								: sFWHA01_t.system_parameter.actual_temp * 0.2;
				else // FAHRENHEIT
					bar_value = (sFWHA01_t.run_mode == Power_Mode)
								? (sFWHA01_t.system_parameter.actual_temp_f_display - 122) * 0.108
								: sFWHA01_t.system_parameter.actual_temp_f_display * 0.108;
			}

            DrawProgressBar(26, 90, 39, 256, 15, 15, bar_value, 0xfca0, 0x18a3, TEMP_BAR_ICON);
        }

        // 显示风量进度条
        if ((sFWHA01_t.system_parameter.last_curve_air_data != sFWHA01_t.system_parameter.air_data) ||
            (sFWHA01_t.system_parameter.last_curve_sleep_air_data != sFWHA01_t.system_parameter.sleep_air_data) ||
            (sFWHA01_t.system_parameter.last_curve_cold_mode_air_data != sFWHA01_t.system_parameter.cold_mode_set_air))
        {
            sFWHA01_t.system_parameter.last_curve_air_data = sFWHA01_t.system_parameter.air_data;
            sFWHA01_t.system_parameter.last_curve_sleep_air_data = sFWHA01_t.system_parameter.sleep_air_data;
            sFWHA01_t.system_parameter.last_curve_cold_mode_air_data = sFWHA01_t.system_parameter.cold_mode_set_air;

            if (this->Work_handle_state == HANDLE_WORKING &&
                this->handle_position == IN_POSSITION &&
                this->sleep_state == SLEEP_OPEN && sFWHA01_t.general_parameter.set_wind_time == 0x00)
            {
                air_value = sFWHA01_t.system_parameter.sleep_air_data;
            }
            else
            {
                air_value = (sFWHA01_t.run_mode == Cold_Mode) 
                            ? sFWHA01_t.system_parameter.cold_mode_set_air
                            : sFWHA01_t.system_parameter.air_data;
            }

            DrawProgressBar(441, 90, 454, 256, 15, 15, air_value * 0.625, 0x045f, 0x18a3, AIR_BAR_ICON);  //   0x4c38
        }

        return;
    }

    // 非曲线页或工作页
    first_in = false;
}

static void show_fan(HA01_Handle *this)
{
    static uint8_t fan_count = 0;
    static uint8_t fan_times = 0;
    static bool first_in = true;
    static bool show_fan_flag = true;
	static uint8_t fan_num = 0x00;

    if (!first_in) {
        first_in = true;
        show_fan_flag = true;
    }

    // 仅在工作页处理
    if (this->page != WORK_PAGE) {
        show_fan_flag = true;
        return;
    }

    // 风量足够且设备未休眠
    if (this->system_parameter.actual_air >= MIN_ACTUAL_AIR) 
    {
        fan_times++;
        if (fan_times > 20) {
            fan_times = 0;
            fan_count = (fan_count + 1) % 4;  // 循环 fan_count 0~3
        }

        show_fan_flag = true;

        // 显示风扇图标
        switch (fan_count) {
            case 0: TranferPicturetoTFT_LCD(430, 40, 36, 34, FAN_ICON_1); break;
            case 1: TranferPicturetoTFT_LCD(430, 40, 36, 34, FAN_ICON_2); break;
            case 2: TranferPicturetoTFT_LCD(430, 40, 36, 34, FAN_ICON_3); break;
            case 3: TranferPicturetoTFT_LCD(430, 40, 36, 34, FAN_ICON_4); break;
        }
    }
    // 风量低或设备休眠，只显示静止风扇图标
    else if (this->system_parameter.actual_air < MIN_ACTUAL_AIR &&
             this->Work_handle_state == HANDLE_SLEEP &&
             show_fan_flag == true)
    {
		fan_num++;
		if(fan_num > 2)
		{
			fan_num = 0x00;
			show_fan_flag = false;
		}
        TranferPicturetoTFT_LCD(430, 40, 36, 34, FAN_ICON_1);
    }
}

static void show_output(HA01_Handle *this)
{
    static uint8_t temp_state = 0;
    static uint8_t last_temp_state = 0;
    static int16_t output_value = 0;
    static int16_t delay_time = 50;         // 初始延时
    int16_t display_actual_data = 0;

    // 只处理工作页
    if (this->page != WORK_PAGE)
	{
		last_output_value = -1;
        return;
	}

    // 休眠或唤醒状态直接清零输出
    if (this->Work_handle_state == HANDLE_SLEEP || this->Work_handle_state == HANDLE_WAKEN)
    {
        LCD_ShowIntNum(325, 52, 0x00, 3, WHITE, BLACK, 24, 0);
        output_value = 0;
    }
    else if (this->Work_handle_state == HANDLE_WORKING)
    {
        // 判断手柄在位且睡眠开启，直接输出0
        if ((this->handle_position == IN_POSSITION && this->sleep_state == SLEEP_OPEN) || sFWHA01_t.run_mode == Cold_Mode)
        {
            output_value = 0;
        }
        else
        {
            // 根据模式计算实际温度
            display_actual_data = this->system_parameter.actual_temp;
            if (sFWHA01_t.run_mode == Power_Mode)
                display_actual_data -= POWER_TEMP;

            if (display_actual_data <= (this->system_parameter.set_temp + 100) && display_actual_data >= (this->system_parameter.set_temp - 100))
            {
                if (last_temp_state != PID_TEMP)
                {
                    output_value = 100;
                    temp_state = PID_TEMP;
                }
                else
                {
                    delay_time--;
                    if (delay_time <= 0)
                    {
                        delay_time = 50; // 重置延时
						int16_t temp_diff = this->system_parameter.set_temp - display_actual_data;
                        output_value = temp_diff >= 0 ? temp_diff : -temp_diff;
                        if (output_value <= 1)
                            output_value = 1;
                    }
                }
            }
            else if (display_actual_data < (this->system_parameter.set_temp - 100))  // 需要加温
            {
                output_value = 100;
                temp_state = ADD_TEMP;
            }
            else if (display_actual_data > (this->system_parameter.set_temp + 100)) // temp_diff > 100, 需要降温
            {
                output_value = 0;
                temp_state = REDUCE_TEMP;
            }

            last_temp_state = temp_state;
        }
    }

    // 只有输出值变化时刷新显示
    if (last_output_value != output_value)
    {
        DrawProgressBar_2(207, 63, 270, 67, output_value, 0x4750, 0x18a3);
        LCD_ShowIntNum(325, 52, output_value, 3, WHITE, BLACK, 24, 0);
        last_output_value = output_value;
    }
}
static void show_temp(void)
{
    static uint8_t first_in = 0;
    static int disp_actual = 0;
	static uint8_t show_step = 0;
	
	switch(show_step)
	{
		case 0:
			// 更新 set_temp_time 和 actual_refresh_time
			if (sFWHA01_t.general_parameter.set_temp_time != 0)
			{
				if (sFWHA01_t.page == CURVE_PAGE)
					sFWHA01_t.general_parameter.set_temp_time = (sFWHA01_t.general_parameter.set_temp_time > 25) ?
																sFWHA01_t.general_parameter.set_temp_time - 25 : 0;
				else if (sFWHA01_t.page == WORK_PAGE)
					sFWHA01_t.general_parameter.set_temp_time = (sFWHA01_t.general_parameter.set_temp_time > 5) ?
																sFWHA01_t.general_parameter.set_temp_time - 5 : 0;
				// 重置缓存
				sFWHA01_t.system_parameter.last_actual_temp = 0;
				sFWHA01_t.system_parameter.last_actual_temp_f_display = 0;
				
				// 更新设定温度显示
				if (sFWHA01_t.system_parameter.last_set_temp != sFWHA01_t.system_parameter.set_temp ||
					sFWHA01_t.system_parameter.last_set_temp_f_display != sFWHA01_t.system_parameter.set_temp_f_display)
				{
					sFWHA01_t.system_parameter.last_set_temp = sFWHA01_t.system_parameter.set_temp;
					sFWHA01_t.system_parameter.last_set_temp_f_display = sFWHA01_t.system_parameter.set_temp_f_display;

					int temp_to_display = (sFWHA01_t.temp_unit == CELSIUS) ? 
										   sFWHA01_t.system_parameter.set_temp : 
										   sFWHA01_t.system_parameter.set_temp_f_display;

					if (sFWHA01_t.page == CURVE_PAGE)
						LCD_ShowIntNum(temp_main_curve.x, temp_main_curve.y, temp_to_display,
									   temp_main_curve.len, 0xfc61, temp_main_curve.bc, temp_main_curve.sizey, 0);
					else if (sFWHA01_t.page == WORK_PAGE)
					{
						TranferPicturetoTFT_LCD(set_temp_icon.x1, set_temp_icon.y1, set_temp_icon.length, set_temp_icon.winth, SET_TEMP_ICON_CN);
						LCD_ShowIntNum(temp_main.x, temp_main.y, temp_to_display,
									   temp_main.len, 0xfc61, temp_main.bc, temp_main.sizey, 0);
					}
				}
				
			}
			else 
			{
				if (actual_temp_refesh_time != 0)
				{
					actual_temp_refesh_time--;
				}
				else
				{
					if(sFWHA01_t.handle_error_state == HANDLE_OK)
					{
						int cal = (sFWHA01_t.temp_unit == FAHRENHEIT && sFWHA01_t.system_parameter.cal_data != 0) ?
						  9 * sFWHA01_t.system_parameter.cal_data / 5 + 32 :
						  -sFWHA01_t.system_parameter.cal_data;
						if(sFWHA01_t.run_mode == Power_Mode)
						{
							if(sFWHA01_t.temp_unit == FAHRENHEIT)
							{
								sFWHA01_t.system_parameter.actual_temp_f_display = 9 * sFWHA01_t.system_parameter.actual_temp / 5 + 32;
								disp_actual = sFWHA01_t.system_parameter.actual_temp_f_display  - 122  + cal;
								if(disp_actual < sFWHA01_t.system_parameter.cpu_temp_f)
									disp_actual = sFWHA01_t.system_parameter.cpu_temp_f;
							}
							else
							{
								disp_actual = sFWHA01_t.system_parameter.actual_temp  - POWER_TEMP + cal;
								if(disp_actual < sFWHA01_t.system_parameter.cpu_temp)
									disp_actual = sFWHA01_t.system_parameter.cpu_temp;
							}
							
						}
						else
						{
							if(sFWHA01_t.temp_unit == FAHRENHEIT)
							{
								sFWHA01_t.system_parameter.actual_temp_f_display = 9 * sFWHA01_t.system_parameter.actual_temp / 5 + 32;
								disp_actual = sFWHA01_t.system_parameter.actual_temp_f_display + cal;
								if(disp_actual < sFWHA01_t.system_parameter.cpu_temp_f)
									disp_actual = sFWHA01_t.system_parameter.cpu_temp_f;
							}
							else
							{
								disp_actual = sFWHA01_t.system_parameter.actual_temp + cal;
								if(disp_actual < sFWHA01_t.system_parameter.cpu_temp)
									disp_actual = sFWHA01_t.system_parameter.cpu_temp;
							}
						}
						actual_temp_refesh_time = (sFWHA01_t.page == CURVE_PAGE || sFWHA01_t.page == LOGO) ? 20 :
											 (sFWHA01_t.Work_handle_state == HANDLE_SLEEP) ? 0 : ACTUAL_TEMP_REFRESH_TIME;

						// 更新实际温度显示

						if (sFWHA01_t.system_parameter.last_actual_temp != sFWHA01_t.system_parameter.actual_temp ||
							sFWHA01_t.system_parameter.last_actual_temp_f_display != sFWHA01_t.system_parameter.actual_temp_f_display)
						{
							sFWHA01_t.system_parameter.last_actual_temp = sFWHA01_t.system_parameter.actual_temp;
							sFWHA01_t.system_parameter.last_actual_temp_f_display = sFWHA01_t.system_parameter.actual_temp_f_display;

							if (sFWHA01_t.display_lock_state == LOCK && sFWHA01_t.run_mode != Cold_Mode)
							{
								if(lock_temp_flag == false)
								{
									temp_to_display = disp_actual;
									if (disp_actual >= (sFWHA01_t.system_parameter.set_temp - LOCK_RANGE) &&
										disp_actual <= (sFWHA01_t.system_parameter.set_temp + LOCK_RANGE))
									{
										lock_temp_flag = true;
									}
								}
								else
								{
									temp_to_display = (sFWHA01_t.temp_unit == FAHRENHEIT) ? sFWHA01_t.system_parameter.set_temp_f_display : sFWHA01_t.system_parameter.set_temp;
								}
							}
							else
							{
								temp_to_display = disp_actual;
							}

							if (sFWHA01_t.page == WORK_PAGE)
							{
								TranferPicturetoTFT_LCD(actual_temp_icon.x1, actual_temp_icon.y1, actual_temp_icon.length, actual_temp_icon.winth, ACTUAL_TEMP_ICON_CN);
								LCD_ShowIntNum(temp_main.x, temp_main.y, temp_to_display, temp_main.len, temp_main.fc, temp_main.bc, temp_main.sizey, 0);
							}
							else if (sFWHA01_t.page == CURVE_PAGE)
							{
								LCD_ShowIntNum(temp_main_curve.x, temp_main_curve.y, temp_to_display, temp_main_curve.len, temp_main_curve.fc, temp_main_curve.bc, temp_main_curve.sizey, 0);
							}
						}
					}
				}
			}
			show_step++;
		break;
		case 1:
			if(sFWHA01_t.general_parameter.set_wind_time != 0x00)
			{
				 if (sFWHA01_t.page == CURVE_PAGE)
					sFWHA01_t.general_parameter.set_wind_time = (sFWHA01_t.general_parameter.set_wind_time > 25) ?
																sFWHA01_t.general_parameter.set_wind_time - 25 : 0;
				else if (sFWHA01_t.page == WORK_PAGE)
					sFWHA01_t.general_parameter.set_wind_time = (sFWHA01_t.general_parameter.set_wind_time > 5) ?
																sFWHA01_t.general_parameter.set_wind_time - 5 : 0;
				
				if(sFWHA01_t.run_mode != Cold_Mode)
				{					
					sFWHA01_t.system_parameter.last_air_data_actual = RESET_VALUE;
					sFWHA01_t.system_parameter.last_sleep_air_data = RESET_VALUE;
				}
				
				// 更新风速显示
				if (sFWHA01_t.system_parameter.last_air_data != sFWHA01_t.system_parameter.air_data ||
					sFWHA01_t.system_parameter.last_cold_mode_set_air != sFWHA01_t.system_parameter.cold_mode_set_air)
				{
					sFWHA01_t.system_parameter.last_air_data = sFWHA01_t.system_parameter.air_data;
					sFWHA01_t.system_parameter.last_cold_mode_set_air = sFWHA01_t.system_parameter.cold_mode_set_air;

					int display_air = (sFWHA01_t.run_mode == Cold_Mode) ? 
									  sFWHA01_t.system_parameter.cold_mode_set_air : 
									  sFWHA01_t.system_parameter.air_data;

					if (sFWHA01_t.page == CURVE_PAGE)
						LCD_ShowIntNum(air_main_curve.x, air_main_curve.y, display_air,
									   air_main_curve.len, 0x3c7f, air_main_curve.bc, air_main_curve.sizey, 0);
					else if (sFWHA01_t.page == WORK_PAGE)
					{
						TranferPicturetoTFT_LCD(set_air_icon.x1, set_air_icon.y1, set_air_icon.length, set_air_icon.winth, SELECT_OUTPUT_FAN_ICON_CN);
						LCD_ShowIntNum(air_main.x, air_main.y, display_air,
									   air_main.len, 0x3c7f, air_main.bc, air_main.sizey, 0);
					}
				}
			}
			else 
			{
				if (actual_wind_refesh_time != 0)
				{
					actual_wind_refesh_time--;
				}
				else
				{
					if ((sFWHA01_t.system_parameter.last_air_data_actual != sFWHA01_t.system_parameter.air_data || sFWHA01_t.system_parameter.last_sleep_air_data != sFWHA01_t.system_parameter.sleep_air_data))
					{
						first_in = true;
						sFWHA01_t.system_parameter.last_air_data_actual = sFWHA01_t.system_parameter.air_data;
						sFWHA01_t.system_parameter.last_sleep_air_data = sFWHA01_t.system_parameter.sleep_air_data; 
						if (sFWHA01_t.handle_position == IN_POSSITION && sFWHA01_t.sleep_state== SLEEP_OPEN && sFWHA01_t.Work_handle_state != HANDLE_SLEEP) 
						{ 
							wind_to_display = sFWHA01_t.system_parameter.sleep_air_data; 
						} 
						else 
							wind_to_display = sFWHA01_t.system_parameter.air_data; 
						if (sFWHA01_t.page == CURVE_PAGE) 
						{ 
							LCD_ShowIntNum(air_main_curve.x, air_main_curve.y, wind_to_display, air_main_curve.len, air_main.fc, air_main_curve.bc, air_main_curve.sizey, 0);
						} 
						else if (sFWHA01_t.page == WORK_PAGE) 
						{ 
							TranferPicturetoTFT_LCD(set_air_icon.x1, set_air_icon.y1, set_air_icon.length, set_air_icon.winth, UNSELECT_OUTPUT_FAN_ICON_CN); 
							LCD_ShowIntNum(air_main.x, air_main.y, wind_to_display, air_main.len, air_main.fc, air_main.bc, air_main.sizey, 0); 
						}
					}
				}
			}	
			show_step = 0;
		break;
		default:break;
	}
}

static void show_channel_state(void)
{
    static uint8_t clean_state = false;

    if (sFWHA01_t.page != WORK_PAGE && sFWHA01_t.page != CURVE_PAGE)
        return;

    // 通道框信息
    typedef struct {
    int x, y, length, winth;
    uint8_t select_img;
    uint8_t unselect_img;
	} CH_Frame;

    CH_Frame ch_frames[CH_COUNT] = {
    {ch1_frame.x1, ch3_frame.y1, ch1_frame.length, ch1_frame.winth, SELECT_CH1, UNSELECT_CH1},
    {ch2_frame.x1, ch3_frame.y1, ch2_frame.length, ch2_frame.winth, SELECT_CH2, UNSELECT_CH2},
    {ch3_frame.x1, ch3_frame.y1, ch3_frame.length, ch3_frame.winth, SELECT_CH3, UNSELECT_CH3}
	};

    // 如果通道改变，更新显示
    if (sFWHA01_t.general_parameter.last_ch != sFWHA01_t.general_parameter.ch)
    {
        for (uint8_t i = 0; i < CH_COUNT; i++)
        {
            uint8_t ch_num = i + 1;
            // 取消上次选择的通道
            if (sFWHA01_t.general_parameter.last_ch == ch_num)
                TranferPicturetoTFT_LCD(ch_frames[i].x, ch_frames[i].y, ch_frames[i].length, ch_frames[i].winth, ch_frames[i].unselect_img);
        }

        // 如果 last_ch 不在 1~3 范围，则取消所有通道
        if (sFWHA01_t.general_parameter.last_ch < 1 || sFWHA01_t.general_parameter.last_ch > 3)
        {
            for (uint8_t i = 0; i < CH_COUNT; i++)
                TranferPicturetoTFT_LCD(ch_frames[i].x, ch_frames[i].y, ch_frames[i].length, ch_frames[i].winth, ch_frames[i].unselect_img);
        }

        // 选择当前通道
        if (sFWHA01_t.general_parameter.ch >= 1 && sFWHA01_t.general_parameter.ch <= 3)
        {
            uint8_t idx = sFWHA01_t.general_parameter.ch - 1;
            TranferPicturetoTFT_LCD(ch_frames[idx].x, ch_frames[idx].y, ch_frames[idx].length, ch_frames[idx].winth, ch_frames[idx].select_img);
        }

        clean_state = false;

        // 重置 last_set_temp
        sFWHA01_t.system_parameter.last_ch1_set_temp = RESET_VALUE;
        sFWHA01_t.system_parameter.last_ch1_set_temp_f_display = RESET_VALUE;
		sFWHA01_t.system_parameter.last_ch2_set_temp = RESET_VALUE;
        sFWHA01_t.system_parameter.last_ch2_set_temp_f_display = RESET_VALUE;
		sFWHA01_t.system_parameter.last_ch3_set_temp = RESET_VALUE;
        sFWHA01_t.system_parameter.last_ch3_set_temp_f_display = RESET_VALUE;

        sFWHA01_t.general_parameter.last_ch = sFWHA01_t.general_parameter.ch;
    }

    // 清理 last_set_temp 当 set_temp 与各通道不匹配
    if (sFWHA01_t.general_parameter.ch != 0 && !clean_state)
    {
        uint16_t set_temp = sFWHA01_t.system_parameter.set_temp;
        uint16_t set_temp_f = sFWHA01_t.system_parameter.set_temp_f_display;
        uint16_t ch_temps[CH_COUNT] = {
            sFWHA01_t.system_parameter.ch1_set_temp,
            sFWHA01_t.system_parameter.ch2_set_temp,
            sFWHA01_t.system_parameter.ch3_set_temp
        };
        uint16_t ch_temps_f[CH_COUNT] = {
            sFWHA01_t.system_parameter.ch1_set_temp_f_display,
            sFWHA01_t.system_parameter.ch2_set_temp_f_display,
            sFWHA01_t.system_parameter.ch3_set_temp_f_display
        };

        bool mismatch = true;
        for (uint8_t i = 0; i < CH_COUNT; i++)
        {
            if (set_temp == ch_temps[i] || set_temp_f == ch_temps_f[i])
            {
                mismatch = false;
                break;
            }
        }

        if (mismatch)
        {
			sFWHA01_t.system_parameter.last_ch1_set_temp = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch1_set_temp_f_display = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch2_set_temp = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch2_set_temp_f_display = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch3_set_temp = RESET_VALUE;
			sFWHA01_t.system_parameter.last_ch3_set_temp_f_display = RESET_VALUE;
            clean_state = true;
        }
    }
}

// 摄氏度转华氏度
static inline uint16_t celsius_to_fahrenheit(uint16_t c)
{
    return 9 * c / 5 + 32;
}


static void number_change(void)
{
    static bool first_in = false;

    // 1?? 初始化，只执行一次
    if (!first_in)
    {
        sFWHA01_t.system_parameter.actual_temp_f_display = celsius_to_fahrenheit(sFWHA01_t.system_parameter.actual_temp);
        sFWHA01_t.system_parameter.set_temp_f_display = celsius_to_fahrenheit(sFWHA01_t.system_parameter.set_temp);

        sFWHA01_t.system_parameter.ch1_set_temp_f_display = celsius_to_fahrenheit(sFWHA01_t.system_parameter.ch1_set_temp);
        sFWHA01_t.system_parameter.ch2_set_temp_f_display = celsius_to_fahrenheit(sFWHA01_t.system_parameter.ch2_set_temp);
        sFWHA01_t.system_parameter.ch3_set_temp_f_display = celsius_to_fahrenheit(sFWHA01_t.system_parameter.ch3_set_temp);

        first_in = true;
    }

    if (sFWHA01_t.last_temp_unit != sFWHA01_t.temp_unit)
    {
        sFWHA01_t.system_parameter.actual_temp_f_display = celsius_to_fahrenheit(sFWHA01_t.system_parameter.actual_temp);
        sFWHA01_t.system_parameter.set_temp_f_display = celsius_to_fahrenheit(sFWHA01_t.system_parameter.set_temp);
        sFWHA01_t.system_parameter.ch1_set_temp_f_display = celsius_to_fahrenheit(sFWHA01_t.system_parameter.ch1_set_temp);
        sFWHA01_t.system_parameter.ch2_set_temp_f_display = celsius_to_fahrenheit(sFWHA01_t.system_parameter.ch2_set_temp);
        sFWHA01_t.system_parameter.ch3_set_temp_f_display = celsius_to_fahrenheit(sFWHA01_t.system_parameter.ch3_set_temp);

        if (sFWHA01_t.temp_unit == CELSIUS)
        {
            ch1_set_temp_1.num = &sFWHA01_t.system_parameter.ch1_set_temp;
            ch2_set_temp_1.num = &sFWHA01_t.system_parameter.ch2_set_temp;
            ch3_set_temp_1.num = &sFWHA01_t.system_parameter.ch3_set_temp;
        }
        else // FAHRENHEIT
        {
            ch1_set_temp_1.num = &sFWHA01_t.system_parameter.ch1_set_temp_f_display;
            ch2_set_temp_1.num = &sFWHA01_t.system_parameter.ch2_set_temp_f_display;
            ch3_set_temp_1.num = &sFWHA01_t.system_parameter.ch3_set_temp_f_display;
        }

        // 更新 last_temp_unit
//        sFWHA01_t.last_temp_unit = sFWHA01_t.temp_unit;
    }
}


static void show_channel_value(void)
{
    if (!(sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE))
        return;

    // ----------------- 通道 1 -----------------
    if (sFWHA01_t.system_parameter.last_ch1_set_temp != sFWHA01_t.system_parameter.ch1_set_temp ||
        sFWHA01_t.system_parameter.last_ch1_set_temp_f_display != sFWHA01_t.system_parameter.ch1_set_temp_f_display)
    {
        if (sFWHA01_t.temp_unit == FAHRENHEIT)
        {
            LCD_ShowIntNum(ch1_set_temp_1.x, ch3_set_temp_1.y, sFWHA01_t.system_parameter.ch1_set_temp_f_display,
                           ch1_set_temp_1.len, ch1_set_temp_1.fc, ch1_set_temp_1.bc, ch1_set_temp_1.sizey, 0);
        }
        else
        {
            LCD_ShowIntNum(ch1_set_temp_1.x, ch3_set_temp_1.y, sFWHA01_t.system_parameter.ch1_set_temp,
                           ch1_set_temp_1.len, ch1_set_temp_1.fc, ch1_set_temp_1.bc, ch1_set_temp_1.sizey, 0);
        }
        sFWHA01_t.system_parameter.last_ch1_set_temp = sFWHA01_t.system_parameter.ch1_set_temp;
        sFWHA01_t.system_parameter.last_ch1_set_temp_f_display = sFWHA01_t.system_parameter.ch1_set_temp_f_display;
    }

    // ----------------- 通道 2 -----------------
    if (sFWHA01_t.system_parameter.last_ch2_set_temp != sFWHA01_t.system_parameter.ch2_set_temp ||
        sFWHA01_t.system_parameter.last_ch2_set_temp_f_display != sFWHA01_t.system_parameter.ch2_set_temp_f_display)
    {
        if (sFWHA01_t.temp_unit == FAHRENHEIT)
        {
            LCD_ShowIntNum(ch2_set_temp_1.x, ch3_set_temp_1.y, sFWHA01_t.system_parameter.ch2_set_temp_f_display,
                           ch2_set_temp_1.len, ch2_set_temp_1.fc, ch2_set_temp_1.bc, ch2_set_temp_1.sizey, 0);
        }
        else
        {
            LCD_ShowIntNum(ch2_set_temp_1.x, ch3_set_temp_1.y, sFWHA01_t.system_parameter.ch2_set_temp,
                           ch2_set_temp_1.len, ch2_set_temp_1.fc, ch2_set_temp_1.bc, ch2_set_temp_1.sizey, 0);
        }
        sFWHA01_t.system_parameter.last_ch2_set_temp = sFWHA01_t.system_parameter.ch2_set_temp;
        sFWHA01_t.system_parameter.last_ch2_set_temp_f_display = sFWHA01_t.system_parameter.ch2_set_temp_f_display;
    }

    // ----------------- 通道 3 -----------------
    if (sFWHA01_t.system_parameter.last_ch3_set_temp != sFWHA01_t.system_parameter.ch3_set_temp ||
        sFWHA01_t.system_parameter.last_ch3_set_temp_f_display != sFWHA01_t.system_parameter.ch3_set_temp_f_display)
    {
        if (sFWHA01_t.temp_unit == FAHRENHEIT)
        {
            LCD_ShowIntNum(ch3_set_temp_1.x, ch3_set_temp_1.y, sFWHA01_t.system_parameter.ch3_set_temp_f_display,
                           ch3_set_temp_1.len, ch3_set_temp_1.fc, ch3_set_temp_1.bc, ch3_set_temp_1.sizey, 0);
        }
        else
        {
            LCD_ShowIntNum(ch3_set_temp_1.x, ch3_set_temp_1.y, sFWHA01_t.system_parameter.ch3_set_temp,
                           ch3_set_temp_1.len, ch3_set_temp_1.fc, ch3_set_temp_1.bc, ch3_set_temp_1.sizey, 0);
        }
        sFWHA01_t.system_parameter.last_ch3_set_temp = sFWHA01_t.system_parameter.ch3_set_temp;
        sFWHA01_t.system_parameter.last_ch3_set_temp_f_display = sFWHA01_t.system_parameter.ch3_set_temp_f_display;
    }

    // ----------------- 风量显示 -----------------
    if (sFWHA01_t.system_parameter.last_ch1_set_air != sFWHA01_t.system_parameter.ch1_set_air)
    {
        LCD_ShowIntNum(ch1_set_air_1.x, ch1_set_air_1.y, sFWHA01_t.system_parameter.ch1_set_air,
                       ch1_set_air_1.len, ch1_set_air_1.fc, ch1_set_air_1.bc, ch1_set_air_1.sizey, 0);
        sFWHA01_t.system_parameter.last_ch1_set_air = sFWHA01_t.system_parameter.ch1_set_air;
    }

    if (sFWHA01_t.system_parameter.last_ch2_set_air != sFWHA01_t.system_parameter.ch2_set_air)
    {
        LCD_ShowIntNum(ch2_set_air_1.x, ch2_set_air_1.y, sFWHA01_t.system_parameter.ch2_set_air,
                       ch2_set_air_1.len, ch2_set_air_1.fc, ch2_set_air_1.bc, ch2_set_air_1.sizey, 0);
        sFWHA01_t.system_parameter.last_ch2_set_air = sFWHA01_t.system_parameter.ch2_set_air;
    }

    if (sFWHA01_t.system_parameter.last_ch3_set_air != sFWHA01_t.system_parameter.ch3_set_air)
    {
        LCD_ShowIntNum(ch3_set_air_1.x, ch3_set_air_1.y, sFWHA01_t.system_parameter.ch3_set_air,
                       ch3_set_air_1.len, ch3_set_air_1.fc, ch3_set_air_1.bc, ch3_set_air_1.sizey, 0);
        sFWHA01_t.system_parameter.last_ch3_set_air = sFWHA01_t.system_parameter.ch3_set_air;
    }
}

void page_switch(void)
{
    static bool first_in = false;
    static bool first_s = false;
    static int reset_time = RESET_TIME;

    // ====================== 复位倒计时逻辑 ======================
    if (sFWHA01_t.reset_flag == true)
    {
        if (--reset_time < 0)
        {
            reset_time = RESET_TIME;
            sFWHA01_t.reset_flag = false;
			sFWHA01_t.page = SELECT_SET_RESET_PAGE_CN;

//            // 工作模式 <-> 页面自动翻转
//            if (sFWHA01_t.work_mode == WORK_CURVE)
//                sFWHA01_t.page = WORK_PAGE;
//            else if (sFWHA01_t.work_mode == WORK_NORMAL)
//                sFWHA01_t.page = CURVE_PAGE;
        }
    }

    // ====================== 首次进入初始化 ======================
    if (first_in == false)
    {
        first_in = true;

        if (sFWHA01_t.work_mode == WORK_NORMAL)
        {
            if (sFWHA01_t.handle_position == IN_POSSITION ||
                sFWHA01_t.handle_position == NOT_IN_POSSITION)
            {
                sFWHA01_t.page = WORK_PAGE;
            }
        }
        else if (sFWHA01_t.work_mode == WORK_CURVE)
        {
            sFWHA01_t.page = CURVE_PAGE;
        }
    }

    // ====================== 正常模式页面锁定 ======================
    if (sFWHA01_t.work_mode == WORK_NORMAL)
    {
        if ((sFWHA01_t.page == WORK_PAGE || sFWHA01_t.page == CURVE_PAGE) &&
            (sFWHA01_t.handle_position == IN_POSSITION ||
             sFWHA01_t.handle_position == NOT_IN_POSSITION))
        {
            sFWHA01_t.page = WORK_PAGE;
        }
    }

//    // ====================== 手柄状态变化处理 ======================
//    if ((sFWHA01_t.last_Work_handle_state != sFWHA01_t.Work_handle_state) &&
//        (sFWHA01_t.page == WORK_PAGE))
//    {
//        if (sFWHA01_t.last_Work_handle_state == HANDLE_SLEEP &&
//            sFWHA01_t.Work_handle_state == HANDLE_WORKING)
//        {
//			sFWHA01_t.last_page = RESET_VALUE;
//        }
//        else if (sFWHA01_t.last_Work_handle_state == HANDLE_WORKING &&
//                 sFWHA01_t.Work_handle_state == HANDLE_SLEEP)
//        {
//            // 清空缓存数据
//            sFWHA01_t.system_parameter.last_air_data = 0x00;
//            sFWHA01_t.system_parameter.last_sleep_air_data = 0x00;
//            sFWHA01_t.system_parameter.last_curve_air_data = 0x00;
//            sFWHA01_t.system_parameter.last_curve_sleep_air_data = 0x00;
//			sFWHA01_t.system_parameter.last_curve_cold_mode_air_data = 0x00;
//        }

//        sFWHA01_t.last_Work_handle_state = sFWHA01_t.Work_handle_state;
//        actual_temp_refesh_time = 0;
//    }

    // ====================== 页面变化 → 只执行一次刷屏（核心提速） ======================
    if (sFWHA01_t.last_page != sFWHA01_t.page)
    {
        sFWHA01_t.last_page = sFWHA01_t.page;

        // ====================== 一次性重置所有显示缓存 ======================
		last_output_value = -1;
        sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
        sFWHA01_t.system_parameter.last_actual_temp = RESET_VALUE;
        sFWHA01_t.system_parameter.last_set_sleep_time = RESET_VALUE;
        display_air = sFWHA01_t.system_parameter.air_data;

        sFWHA01_t.system_parameter.last_ch1_set_temp = RESET_VALUE;
        sFWHA01_t.system_parameter.last_ch1_set_temp_f_display = RESET_VALUE;
        sFWHA01_t.system_parameter.last_ch2_set_temp = RESET_VALUE;
        sFWHA01_t.system_parameter.last_ch2_set_temp_f_display = RESET_VALUE;
        sFWHA01_t.system_parameter.last_ch3_set_temp = RESET_VALUE;
        sFWHA01_t.system_parameter.last_ch3_set_temp_f_display = RESET_VALUE;

        sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
        sFWHA01_t.system_parameter.last_pwm_out = RESET_VALUE;

        sFWHA01_t.system_parameter.last_curve_actual_temp = 1000;
        sFWHA01_t.system_parameter.last_curve_actual_temp_f_display = 1000;
        sFWHA01_t.system_parameter.last_curve_air_data = RESET_VALUE;

        sFWHA01_t.general_parameter.last_ch = RESET_VALUE;
        sFWHA01_t.system_parameter.last_cal_temp_c_display = 1000;
        sFWHA01_t.system_parameter.last_cal_temp_f_display = 1000;

        sFWHA01_t.last_temp_unit = RESET_VALUE;
        sFWHA01_t.last_work_mode = RESET_VALUE;
        sFWHA01_t.last_display_lock_state = RESET_VALUE;
        last_sleep_state = RESET_VALUE;

        sFWHA01_t.system_parameter.last_ch1_set_air = RESET_VALUE;
        sFWHA01_t.system_parameter.last_ch2_set_air = RESET_VALUE;
        sFWHA01_t.system_parameter.last_ch3_set_air = RESET_VALUE;

        // ====================== 高效刷屏（只刷一次） ======================
        if ((sFWHA01_t.page >= SET_RUN_PAGE_CN && sFWHA01_t.page <= SET_SUPPORT_PAGE_ENG) || sFWHA01_t.page == SET_SUPPORT_PAGE_CN_HA05 || sFWHA01_t.page == SET_SUPPORT_PAGE_ENG_HA05)
        {
            // 设置页：只第一次刷背景，后续只刷小窗口
            if (first_s == false)
            {
                TranferPicturetoTFT_LCD(LCD_FULL_SCREEN_X1, LCD_FULL_SCREEN_Y1,
                                       LCD_FULL_SCREEN_WIDTH, LCD_FULL_SCREEN_HEIGHT,
                                       MENU_BACKGROUND);
                first_s = true;
            }

            // 按页面类型刷对应小区域
            if ((sFWHA01_t.page >= SET_RUN_PAGE_CN   && sFWHA01_t.page <= SET_SPEAK_PAGE_ENG) ||
                (sFWHA01_t.page >= SET_TEMP_CAL_PAGE_CN && sFWHA01_t.page <= SET_RESET_DONE_PAGE_ENG))
            {
                TranferPicturetoTFT_LCD(79, 70, 322, 165, sFWHA01_t.page);
            }
            else if (sFWHA01_t.page >= SET_KEY_PAGE_CN && sFWHA01_t.page <= SET_KEY_MODE_PAGE_ENG)
            {
                TranferPicturetoTFT_LCD(80, 59, 319, 187, sFWHA01_t.page);
            }
            else if (sFWHA01_t.page == SET_LANGUAGE_PAGE_CN || sFWHA01_t.page == SET_LANGUAGE_PAGE_ENG)
            {
                TranferPicturetoTFT_LCD(72, 88, 336, 130, sFWHA01_t.page);
            }
            else if (sFWHA01_t.page == SET_SUPPORT_PAGE_CN || sFWHA01_t.page == SET_SUPPORT_PAGE_ENG || sFWHA01_t.page == SET_SUPPORT_PAGE_CN_HA05 || sFWHA01_t.page == SET_SUPPORT_PAGE_ENG_HA05)
            {
                TranferPicturetoTFT_LCD(9, 87, 462, 132, sFWHA01_t.page);
            }
        }
        else
        {
            // 普通页面：全屏刷一次
            first_s = false;
            TranferPicturetoTFT_LCD(LCD_FULL_SCREEN_X1, LCD_FULL_SCREEN_Y1,
                                   LCD_FULL_SCREEN_WIDTH, LCD_FULL_SCREEN_HEIGHT,
                                   sFWHA01_t.page);
        }
    }
}

void show_navigation_bar(void)
{
    // 首次绘制导航栏
    if (!first_draw)
    {
		TranferPicturetoTFT_LCD(0, 0, 480, 30, NAVIGATION_BAR_HA05);
        LCD_VISION(vision_2.x, vision_2.y, vision_2.len, vision_2.fc, vision_2.bc, vision_2.sizey);
        sFWHA01_t.last_speak_state = RESET_VALUE;
        first_draw = true;
    }

    // 显示蜂鸣器图标
//    if (sFWHA01_t.page != SET_SPEAK_PAGE_CN && sFWHA01_t.last_speak_state != sFWHA01_t.speak_state)
	if (sFWHA01_t.last_speak_state != sFWHA01_t.speak_state)
    {
        if (sFWHA01_t.speak_state == SPEAKER_OPEN) 
		{ 
			TranferPicturetoTFT_LCD(beep.x1, beep.y1, beep.length, beep.winth, SPEAK_ICON); 
		} 
		else 
		{ 
			TranferPicturetoTFT_LCD(beep.x1, beep.y1, beep.length, beep.winth, UN_SPEAK_ICON); 
		}

        sFWHA01_t.last_speak_state = sFWHA01_t.speak_state;
    }

    // 显示电源模式图标
    if (sFWHA01_t.last_run_mode != sFWHA01_t.run_mode)
    {
        if (sFWHA01_t.run_mode == Power_Mode || sFWHA01_t.before_cold_run_mode == Power_Mode)
        {
            TranferPicturetoTFT_LCD(power_icon.x1, power_icon.y1, power_icon.length, power_icon.winth, POWER_ICON);
        }
        else
        {
            first_draw = false; // 非电源模式下重置首次绘制
        }

        sFWHA01_t.last_run_mode = sFWHA01_t.run_mode;
    }
}