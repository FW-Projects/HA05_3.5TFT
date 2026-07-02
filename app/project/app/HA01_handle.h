#ifndef _HA01_HANDLE_H
#define _HA01_HANDLE_H
#include "at32f415.h"
#include "stdbool.h"

#define SOFTWARE_VERSTION "V1.0.8"
#define HARDWARE_VERSTION "V1.0.1"


#define CURVE_REFRESH_TIME 0
#define CURVE_BUFF_SIZE 358

#define RESET_TIME 300

#define SAVE_CH_TIME 100

#define RESET_VALUE 255

#define MAX_SET_TEMP 500
#define MIN_SET_TEMP 100
#define MAX_SET_TEMP_F 932
#define MIN_SET_TEMP_F 212

#define MAX_CAL_TEMP 100
#define MIN_CAL_TEMP -100
#define MAX_CAL_TEMP_F 212
#define MIN_CAL_TEMP_F -212

#define PID_RANGE 100

#define MAX_SET_AIR 160
#define MIN_SET_AIR 5

#define MAX_ACTUAL_AIR 534
#define MIN_ACTUAL_AIR 80

#define MAX_ACTUAL_TEMP 600
#define MIN_ACTUAL_TEMP 70

#define ERROR_TIME 250

//#define MAX_PWM_OUTPUT (35000)
#define MAX_PWM_OUTPUT (48000-1)


#define ACTUAL_TEMP_REFRESH_TIME 20

#define SET_TEMP_SHOW_TIMES 1000

#define LOCK_RANGE 10

#define SLEEP_TIME 50

#define SLEEP_FAN_DATA 100

#define WAKEN_TIMES 3000

#define POWER_TEMP 50

#define TEMP 0
#define WIND 1
#define CH   2


#define LOGO_PICTURE 1
#define WORK_PICTURE 2
#define CURVE_PICTURE 3
#define SELECT_SET_WORK_PICTURE_CN 4
#define SELECT_SET_UNIT_PICTURE_CN 5
#define SELECT_SET_SPEAK_PICTURE_CN 6
#define SELECT_SET_LANGUAGE_PICTURE_CN 7
#define SELECT_SET_TEMP_CAL_PICTURE_CN 8
#define SELECT_SET_TEMP_LOCK_PICTURE_CN 9
#define SELECT_SET_SLEEP_PICTURE_CN 10
#define SELECT_SET_VERSION_PICTURE_CN 11
#define SELECT_SET_RESET_PICTURE_CN 12
#define SELECT_SET_SUPPORT_PICTURE_CN 13
#define SELECT_EXIT_MENU_PICTURE_CN 14

#define SET_WORK_PICTURE_CN 15
#define SET_UNIT_PICTURE_CN 16
#define SET_TEMP_CAL_PICTURE_CN 17
#define SET_SPEAK_PICTURE_CN 18
#define SET_TEMP_LOCK_PICTURE_CN 19
#define SET_SLEEP_PICTURE_CN 20
#define SET_LANGUAGE_PICTURE_CN 21
#define SET_LANGUAGE_PICTURE_ENG 22
#define SET_VERSION_PICTURE_CN 23
#define SET_RESET_PICTURE_CN 24
#define SET_RESET_DONE_PICTURE_CN 25
#define SET_SUPPORT_PICTURE_CN 26 


typedef enum
{
    HA04 = 0,
	HA05,
}
model_e;

typedef enum
{
    HANDLE_SLEEP = 0,
    HANDLE_WORKING,
    HANDLE_WAKEN,
} handle_state_e;

typedef enum
{
    HANDLE_OK = 0,
    HANDLE_LOW_TEMP_ERR,
    HANDLE_OVER_TEMP_ERR,
	HANDLE_FAN_ERROR,
} handle_error_state_e;

typedef enum
{
    NOT_IN_POSSITION = 0,
    IN_POSSITION,
} handle_position_e;

typedef enum
{
    WORK_NORMAL = 0,
    WORK_CURVE,
} work_mode_e;

typedef enum
{
    Standard_Mode = 0,
    Power_Mode,
	Cold_Mode,
} run_mode_e;


typedef enum
{
    CELSIUS = 0,
    FAHRENHEIT,
} temp_unit_e;

typedef enum
{
    SET_RETURN_CAL = 0,
	SET_SELECT_CAL,
    SET_CAL,
} set_cal_e;

typedef enum
{
    SPEAKER_CLOSE = 0,
    SPEAKER_OPEN,
    RETURN_SPEAK,
} speaker_state_e;

typedef enum
{
    UNLOCK = 0,
    LOCK,
} temp_lock_state_e;

typedef enum
{
    SLEEP_OPEN = 0,
	SLEEP_CLOSE,
} sleep_state_e;

typedef enum
{
    RESET_RETURN = 0,
    RESET_CONFIRN,
} set_reset_e1;

typedef enum
{
    RESET2_RETURN = 0,
    RESET2_CONFIRN,
} set_reset_e2;

typedef enum
{
    VISION_RETURN = 0,
    VISION_START,
} set_vision_e;

typedef enum 
{
	CHINESE = 0,
	ENGLISH,
} language_e;


typedef enum 
{
	LONGKEY_MODE = 1,
	SHORTKEY_MODE,
	ADJUSTKEY_MODE,
	RETURN_MENU,
} key_type_e;


typedef enum 
{
	CHANNEL_SWITCHING = 1,
	COLDWIND_MODE,
	POWER_MODE,
} key_mode_e;


typedef enum 
{
	SELECT_TEMP = 1,
	SELECT_WIND,
	SELECT_CH,
} adjust_key_mode_e;


typedef enum
{
    LOGO = 1,
    WORK_PAGE,
	CURVE_PAGE,
	MENU_BACKGROUND,
    SELECT_SET_WORK_PAGE_CN,
	SELECT_SET_WORK_PAGE_ENG,
    SELECT_SET_UNIT_PAGE_CN,
	SELECT_SET_UNIT_PAGE_ENG,
	SELECT_SET_SPEAK_PAGE_CN,
	SELECT_SET_SPEAK_PAGE_ENG, //10
	SELECT_SET_KEY_PAGE_CN,
	SELECT_SET_KEY_PAGE_ENG,
	SELECT_SET_LANGUAGE_PAGE_CN,
	SELECT_SET_LANGUAGE_PAGE_ENG,
    SELECT_SET_TEMP_CAL_PAGE_CN,
	SELECT_SET_TEMP_CAL_PAGE_ENG,
    SELECT_SET_TEMP_LOCK_PAGE_CN,
	SELECT_SET_TEMP_LOCK_PAGE_ENG,
    SELECT_SET_SLEEP_PAGE_CN,
	SELECT_SET_SLEEP_PAGE_ENG, //20
	SELECT_SET_VERSION_PAGE_CN,
	SELECT_SET_VERSION_PAGE_ENG,
    SELECT_SET_RESET_PAGE_CN,
	SELECT_SET_RESET_PAGE_ENG,
	SELECT_SET_SUPPORT_PAGE_CN,
	SELECT_SET_SUPPORT_PAGE_ENG,
    SELECT_EXIT_MENU_PAGE_CN,
	SELECT_EXIT_MENU_PAGE_ENG,
    SET_RUN_PAGE_CN,
	SET_RUN_PAGE_ENG,          //30
    SET_UNIT_PAGE_CN,
	SET_UNIT_PAGE_ENG,
	SET_SPEAK_PAGE_CN,
	SET_SPEAK_PAGE_ENG,
	SET_KEY_PAGE_CN,
	SET_KEY_PAGE_ENG,
	SET_LONG_KEY_PAGE_CN,
	SET_LONG_KEY_PAGE_ENG,
	SET_SHORT_KEY_PAGE_CN,
	SET_SHORT_KEY_PAGE_ENG,    //40
	SET_KEY_MODE_PAGE_CN,
	SET_KEY_MODE_PAGE_ENG,
	SET_LANGUAGE_PAGE_CN,
	SET_LANGUAGE_PAGE_ENG, 
	SET_TEMP_CAL_PAGE_CN,
	SET_TEMP_CAL_PAGE_ENG,
	SET_SELECT_TEMP_CAL_PAGE_CN,
	SET_SELECT_TEMP_CAL_PAGE_ENG,
    SET_TEMP_LOCK_PAGE_CN,
	SET_TEMP_LOCK_PAGE_ENG,     //50
    SET_SLEEP_PAGE_CN,
	SET_SLEEP_PAGE_ENG,
	SET_VERSION_PAGE_CN,
	SET_VERSION_PAGE_ENG,
    SET_RESET_PAGE_CN,
	SET_RESET_PAGE_ENG,
    SET_RESET_DONE_PAGE_CN,
	SET_RESET_DONE_PAGE_ENG,
	SET_SUPPORT_PAGE_CN,
	SET_SUPPORT_PAGE_ENG,       //60
	NAVIGATION_BAR,
	 /* some icons */
	SPEAK_ICON,           
    UN_SPEAK_ICON, 
	
	SLEEP_STATE_CN,    
	SLEEP_STATE_ENG,  
	LOW_TEMP_ICON_CN,  
	LOW_TEMP_ICON_ENG,	
	FAN_ABNORMAL_ICON_CN,
	FAN_ABNORMAL_ICON_ENG,
	OVER_TEMP_ICON_CN,       //70
	OVER_TEMP_ICON_ENG,
	
	CAL_CELSIUS_ICON,
    CAL_FAHRENHEIT_ICON,
	CURVE_CELSIUS_ICON,    
    CURVE_FAHRENHEIT_ICON, 
	WORK_CELSIUS_ICON,    
    WORK_FAHRENHEIT_ICON, 
	
	RETURN_ICON_CN,
	RETURN_ICON_ENG,
	
	UN_RETURN_ICON_ENG,       //80
	UN_RETURN_ICON_CN,    
	
	
	EXIT_ICON_CN,
	EXIT_ICON_ENG,
	UN_EXIT_ICON_CN,
	UN_EXIT_ICON_ENG,
	
	FAN_ICON_1,
	FAN_ICON_2,
	FAN_ICON_3,
	FAN_ICON_4,
	
	MENU_SELECT_ICON,       //90
	MENU_UNSELECT_ICON,
	
	ACTUAL_TEMP_ICON_CN,
	ACTUAL_TEMP_ICON_ENG,
	SET_TEMP_ICON_CN,
	SET_TEMP_ICON_ENG,
	UNSELECT_OUTPUT_FAN_ICON_CN,
	UNSELECT_OUTPUT_FAN_ICON_ENG,
	SELECT_OUTPUT_FAN_ICON_CN,
	SELECT_OUTPUT_FAN_ICON_ENG,
	
	UNSELECT_CH1,          //100
	UNSELECT_CH2, 
	UNSELECT_CH3, 
	SELECT_CH1, 
	SELECT_CH2, 
	SELECT_CH3, 
	TEMP_BAR_ICON,
	AIR_BAR_ICON,

	
	CONFIRM_ICON_CN,
	CONFIRM_ICON_ENG,
	UNCONFIRM_ICON_ENG,    //110
	UNCONFIRM_ICON_CN,
	
	POWER_ICON,
	
	SAVE_ICON_CN,
	SAVE_ICON_ENG,
	
	CURVE_ICON,
	CURVE_FAN_ICON,
	
	COLD_MODE_ICON_CN,
	COLD_MODE_ICON_ENG,
	
	MAIN_POWER_OUTPUT_ENG,
	MAIN_POWER_OUTPUT_CN,    //110
	
	SELECT_ARROW,
	NOSELECT_ARROW,
	
	REFRESH_SLEEP_ICON,
	REFRESH_COLDWIND_ICON,
	WORK_PAGE_ENG,
	REFRESH_SLEEP_ICON_ENG,
	
	NAVIGATION_BAR_HA05,
	LOGO_HA05,
	
	SET_SUPPORT_PAGE_CN_HA05,
	SET_SUPPORT_PAGE_ENG_HA05
	
	
} page_e;

typedef struct
{
    float actual_temp;
    float actual_temp_f_display;
    float last_actual_temp;
    float last_actual_temp_f_display;
	float last_curve_actual_temp;
	float last_curve_actual_temp_f_display;
    float set_temp;
    float last_set_temp;
    float set_temp_f_display;
    float last_set_temp_f_display;
    float cal_data;
    float cal_temp_c_display;
    float last_cal_temp_c_display;
    float cal_temp_f_display;
    float last_cal_temp_f_display;
	float current_data;
    float pwm_out;
	float last_pwm_out;
	float cpu_temp;
	float cpu_temp_f;
    float set_sleep_time;
    float last_set_sleep_time;
    float sleep_time_count;
    float waken_time_count;
    uint16_t error_time;
    bool run_disp_state;
    bool last_run_disp_state;
    float last_power_data;
	float ch1_set_temp;
	float ch2_set_temp;
	float ch3_set_temp;
	float ch1_set_temp_f_display;
	float ch2_set_temp_f_display;
	float ch3_set_temp_f_display;
	
	float ch1_set_air;
	float ch2_set_air;
	float ch3_set_air;
	float last_ch1_set_air;
	float last_ch2_set_air;
	float last_ch3_set_air;
	float air_data;
	float last_air_data;
	float last_air_data_actual;
	float last_curve_air_data;
	float last_curve_sleep_air_data;
	float last_curve_cold_mode_air_data;
	float actual_air;
	float sleep_air_data;
	float last_sleep_air_data;
	float cold_mode_set_air;
	float last_cold_mode_set_air;
	
	uint16_t sleep_time;
	int fan_icon_show_times;
	uint8_t fan_icon_show_step;
	float temp_linear_data;
	
	float last_ch1_set_temp;
	float last_ch2_set_temp;
	float last_ch3_set_temp;
	float last_ch1_set_temp_f_display;
	float last_ch2_set_temp_f_display;
	float last_ch3_set_temp_f_display;
    uint8_t temp_buff[CURVE_BUFF_SIZE];
    uint8_t air_buff[CURVE_BUFF_SIZE];
} handle_parameter_t;

typedef struct
{
	uint8_t ch;
	uint8_t last_ch;
	uint16_t set_temp_time;
	uint16_t set_wind_time;
	bool save_ch_flag;
} system_state_t;

typedef struct
{
	handle_parameter_t system_parameter;
	system_state_t general_parameter;
	handle_state_e Work_handle_state;
    handle_state_e last_Work_handle_state;
	handle_position_e handle_position;
	handle_position_e last_handle_position;
	handle_error_state_e handle_error_state;
    handle_error_state_e last_handle_error_state;
	
	work_mode_e work_mode;
    work_mode_e last_work_mode;
    work_mode_e set_work_mode;
	
	run_mode_e run_mode;
	run_mode_e last_run_mode;
	run_mode_e before_cold_run_mode;
	run_mode_e set_run_mode;
	
	temp_unit_e temp_unit;
    temp_unit_e last_temp_unit;
	temp_unit_e set_temp_unit;
	
    speaker_state_e speak_state;
    speaker_state_e last_speak_state;
	speaker_state_e set_speak_state;
	
    temp_lock_state_e display_lock_state;
    temp_lock_state_e last_display_lock_state;
	temp_lock_state_e set_display_lock_state;
	
	language_e language_state;
	language_e last_language_state;
	language_e set_language_state;
	
	sleep_state_e sleep_state;
	sleep_state_e last_sleep_state;
	sleep_state_e set_sleep_state;
	
	set_cal_e set_cal;
	
	set_reset_e1 set_reset1;
    set_reset_e2 set_reset2;
	
	key_type_e set_key_mode;
	
	key_mode_e long_key_mode;
	key_mode_e set_long_key_mode;
	
	key_mode_e short_key_mode;
	key_mode_e set_short_key_mode;
	
	adjust_key_mode_e adjust_key_mode;
	adjust_key_mode_e set_adjust_key_mode;
	
	page_e page;
    page_e last_page;
	
	
    bool reset_flag;
    bool init_flag;
	bool fan_run_flag;
	bool flash_updata;
    model_e model;


} HA01_Handle;


extern HA01_Handle sFWHA01_t;

void FWHA01_Init(HA01_Handle *FWHA01);

#endif