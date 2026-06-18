#include "HA01_handle.h"
#include <string.h>

HA01_Handle sFWHA01_t;

void FWHA01_Init(HA01_Handle *FWHA01)
{

	/* start the Direct handle init of  */
		FWHA01->system_parameter.actual_temp = 0,
	FWHA01->system_parameter.set_temp = 380,
	FWHA01->system_parameter.set_temp_f_display = 380;

	FWHA01->system_parameter.error_time = 0,
	FWHA01->system_parameter.cal_data = 0,
	FWHA01->system_parameter.cal_temp_c_display = 0,
	FWHA01->system_parameter.cal_temp_f_display = 0,
	FWHA01->handle_position = NOT_IN_POSSITION,
	FWHA01->handle_error_state = HANDLE_OK,
	FWHA01->Work_handle_state = HANDLE_WORKING;

	memset(FWHA01->system_parameter.temp_buff, 250, CURVE_BUFF_SIZE);
    memset(FWHA01->system_parameter.air_buff, 250, CURVE_BUFF_SIZE);

	/* end the  Direct handle init of*/

	/* start the general init  of  */
	FWHA01->work_mode = WORK_NORMAL,
	FWHA01->set_work_mode = WORK_NORMAL,
	FWHA01->temp_unit = CELSIUS,
	FWHA01->set_temp_unit = CELSIUS,
	FWHA01->speak_state = SPEAKER_OPEN,
	FWHA01->set_speak_state = SPEAKER_OPEN,
	FWHA01->display_lock_state = UNLOCK,
	FWHA01->set_display_lock_state = UNLOCK,
	FWHA01->language_state = CHINESE,
	FWHA01->set_language_state = CHINESE,
	FWHA01->sleep_state = SLEEP_OPEN,
	FWHA01->set_sleep_state = SLEEP_OPEN,
	
	FWHA01->set_key_mode = LONGKEY_MODE;
	FWHA01->long_key_mode = COLDWIND_MODE;
	FWHA01->set_long_key_mode = COLDWIND_MODE;
	FWHA01->short_key_mode = CHANNEL_SWITCHING;
	FWHA01->set_short_key_mode = CHANNEL_SWITCHING;
	FWHA01->adjust_key_mode = SELECT_TEMP;
	FWHA01->set_adjust_key_mode = SELECT_TEMP;
	
	FWHA01->general_parameter.ch = 0,
	FWHA01->system_parameter.last_pwm_out = 1000;
	/* ch set */
	FWHA01->system_parameter.ch1_set_temp = 320,
	FWHA01->system_parameter.ch2_set_temp = 350,
	FWHA01->system_parameter.ch3_set_temp = 380,
	FWHA01->system_parameter.last_sleep_air_data = 0,
	FWHA01->system_parameter.sleep_air_data = 0,
	FWHA01->system_parameter.ch1_set_air = 40,
	FWHA01->system_parameter.ch2_set_air = 50,
	FWHA01->system_parameter.ch3_set_air = 60,
	FWHA01->system_parameter.cold_mode_set_air = 100,
	FWHA01->system_parameter.air_data = 50,
	/* en working set */
		/* end the general init of*/

		FWHA01->reset_flag = 0;
	FWHA01->work_mode = WORK_NORMAL;
	//	FWHA01->work_mode = WORK_CURVE;
	FWHA01->page = LOGO;
	FWHA01->last_page = LOGO;
	FWHA01->system_parameter.pwm_out = 0x00;
	FWHA01->system_parameter.cpu_temp = 0x00;
	
	FWHA01->system_parameter.air_data = 0x00;
	FWHA01->system_parameter.last_air_data = 0x00;
	FWHA01->system_parameter.last_air_data_actual = 0x00;
	FWHA01->system_parameter.actual_air = 0x00;
	FWHA01->system_parameter.last_curve_sleep_air_data = 0x00;
	FWHA01->system_parameter.last_curve_air_data = 0x00;
//	FWHA01->general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
	FWHA01->general_parameter.set_temp_time = 0x00;
	FWHA01->general_parameter.save_ch_flag = false;
	FWHA01->fan_run_flag = true;
	FWHA01->init_flag = false;
}
