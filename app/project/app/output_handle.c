#include "output_handle.h"
#include "beep_handle.h"
#include "PID_operation.h"
#include "adc.h"
#include "adc_filter.h"
#include "lcd_handle.h"
//#include "perf_counter.h"
static void get_handle_position(HA01_Handle *this);
static void get_handle_work_state(HA01_Handle *this);
static void get_handle_error_state(HA01_Handle *this);
//static voidtrol(HA01_Handle *this);
static void relay_control(HA01_Handle *this);

int8_t off_set_buff[41] = {22, 21, 21, 21, 21, 21, 21, 21, 20, 20,
                           21, 21, 21, 20, 19, 19, 19, 19, 18, 17,
                           17, 16, 16, 15, 14, 13, 12, 11, (11), (10 - 2),
                           (9), (9), (8), 12, 15, 17, 21, 24, 26, 28, 33
                          };

int8_t wind_set_20_buff[41] = {22 - 10, 21 - 10, 21 - 10, 21 - 10, 21 - 10, 21 - 10, 21 - 10, 21 - 10, 20 - 10, 20 - 10,
                               21 - 10, 21 - 10, 21 - 10, 20 - 10, 19 - 10, 19 - 10, 19 - 10, 19 - 10, 18 - 10, 17 - 10,
                               17 - 10, 16 - 10, 16 - 10, 15 - 10, 14 - 10, 13 - 10, 12 - 10, 11 - 10, (11 - 30), (10 - 2 - 10),
                               (9 - 10), (9 - 10), (8 - 10), 12 - 10, 15 - 10, 17 - 10, 21 - 10, 24 - 10, 26 - 10, 28 - 10, 33 - 10
                              };

void output_handle(void)
{
	static uint8_t in_first = false;
	
    get_handle_position(&sFWHA01_t);
    get_handle_work_state(&sFWHA01_t);
	get_handle_error_state(&sFWHA01_t);
	fan_control(&sFWHA01_t);
 //    fan_control(&sFWHA01_t);
}

static void get_handle_position(HA01_Handle *this)
{
    if (gpio_input_data_bit_read(GPIOC, GPIO_PINS_13))
    {
        this->handle_position = NOT_IN_POSSITION;
    }
    else
    {
        this->handle_position = IN_POSSITION;
    }
}

static void get_handle_work_state(HA01_Handle *this)
{
	static uint8_t first_n = false;
	static float act_temp = 0x00;
    switch (this->Work_handle_state)
    {
    case HANDLE_SLEEP:
		if(this->sleep_state == SLEEP_OPEN)
		{
			if (this->handle_position == NOT_IN_POSSITION || this->run_mode == Cold_Mode)
			{
				this->Work_handle_state = HANDLE_WORKING;
				sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
				sFWHA01_t.system_parameter.last_curve_air_data = 0x00;
				sFWHA01_t.system_parameter.last_air_data = 0x00;
				sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
				sFWHA01_t.system_parameter.last_set_temp = 0x00;
				sFWHA01_t.system_parameter.last_set_temp_f_display = 0x00;
			}
			else if (this->handle_position == IN_POSSITION)
			{
				if(first_n == false)
				{
					first_n = true;
					this->system_parameter.last_air_data = 0x00;
					this->system_parameter.last_curve_air_data = 0x00;
				}
				this->Work_handle_state = HANDLE_SLEEP;
			}
		}
		else if(this->sleep_state == SLEEP_CLOSE)
		{
			this->Work_handle_state = HANDLE_WORKING;
			sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
			sFWHA01_t.system_parameter.last_set_temp = 0x00;
			sFWHA01_t.system_parameter.last_set_temp_f_display = 0x00;
		}
        this->system_parameter.error_time = 0;
        break;

    case HANDLE_WORKING:

        if (this->handle_position == IN_POSSITION &&
                this->sleep_state == SLEEP_OPEN &&
				this->run_mode != Cold_Mode)
        {
			lock_temp_flag = false;
			this->last_handle_position = IN_POSSITION;
			if(sFWHA01_t.run_mode == Power_Mode)
			{
				act_temp =this->system_parameter.actual_temp - POWER_TEMP;		
			}
			else
			{
				act_temp = this->system_parameter.actual_temp;
				
			}
            if (act_temp <= 70)
            {
                this->system_parameter.sleep_time++;

                if (this->system_parameter.sleep_time > SLEEP_TIME)
                {
                    this->Work_handle_state = HANDLE_SLEEP;
                    this->system_parameter.sleep_time = 0;
					sFWHA01_t.fan_run_flag = false;
					sFWHA01_t.system_parameter.last_air_data_actual = 0x00;
                    sbeep.cmd = BEEP_LONG;
                    break;
                }
            }
        }
		else if(this->sleep_state == SLEEP_CLOSE)
		{
			  this->Work_handle_state = HANDLE_WORKING;
		}
		else if (this->handle_position == NOT_IN_POSSITION)
		{
			if(this->last_handle_position == IN_POSSITION)
			{
				
				this->last_handle_position  = NOT_IN_POSSITION;
				sFWHA01_t.general_parameter.set_temp_time = SET_TEMP_SHOW_TIMES;
				sFWHA01_t.general_parameter.set_wind_time = SET_TEMP_SHOW_TIMES;
				sFWHA01_t.system_parameter.last_set_temp = RESET_VALUE;
				sFWHA01_t.system_parameter.last_air_data = RESET_VALUE;
			}
			
		}
        break;
	case HANDLE_WAKEN:
		if(sFWHA01_t.system_parameter.waken_time_count != 0)
		{
			if(this->handle_position == NOT_IN_POSSITION)
			{
				this->Work_handle_state = HANDLE_WORKING;
				sFWHA01_t.system_parameter.waken_time_count = 0;
			}
		}
		else
		{
			if(this->handle_position == NOT_IN_POSSITION)
				this->Work_handle_state = HANDLE_WORKING;
			else if(this->handle_position == IN_POSSITION)
				this->Work_handle_state = HANDLE_SLEEP;
		}
		break;
    default:
        break;
    }
}

static void get_handle_error_state(HA01_Handle *this)
{
    static uint8_t get_step = 0;

    switch (get_step)
    {
    case 0:
		if(sFWHA01_t.run_mode == Cold_Mode)
		{
			if(this->system_parameter.actual_air < MIN_ACTUAL_AIR)
			{
				this->system_parameter.error_time++;
				
				if(this->system_parameter.error_time > ERROR_TIME * 2)
				{
					this->handle_error_state = HANDLE_FAN_ERROR;
					this->system_parameter.error_time = 0x00;
					get_step = 1;
					break;
				}
			}
		}
		else
		{
			if (this->handle_position == NOT_IN_POSSITION ||
					(this->handle_position == IN_POSSITION && this->sleep_state== SLEEP_CLOSE))
			{
				if (this->system_parameter.actual_temp >= MAX_ACTUAL_TEMP ||
						this->system_parameter.actual_temp <= MIN_ACTUAL_TEMP ||
						this->system_parameter.actual_air < MIN_ACTUAL_AIR)
				{
					this->system_parameter.error_time++;

					/* 5 * 200 / 2 = 500ms */
					if (this->system_parameter.error_time > ERROR_TIME / 2)
					{
						if (this->system_parameter.actual_temp >= MAX_ACTUAL_TEMP)
						{
							this->handle_error_state = HANDLE_OVER_TEMP_ERR;
							this->system_parameter.error_time = 0;
							get_step = 1;
							break;
						}
					}

					/* 5 * 200  = 1000ms */
					if (this->system_parameter.error_time > ERROR_TIME * 2)
					{
						if (this->system_parameter.actual_air < MIN_ACTUAL_AIR)
						{
							this->handle_error_state = HANDLE_FAN_ERROR;
							this->system_parameter.error_time = 0;
							get_step = 1;
							break;
						}
						else if (this->system_parameter.actual_temp <= MIN_ACTUAL_TEMP)
						{
							this->handle_error_state = HANDLE_LOW_TEMP_ERR;
							this->system_parameter.error_time = 0;
							get_step = 1;
							break;
						}
					}
				}
				else
				{
					this->system_parameter.error_time = 0;
				}
			}
			else
			{
				this->system_parameter.error_time = 0;
			}
		}
        break;
    case 1:
        if (this->handle_position == IN_POSSITION && this->sleep_state == SLEEP_OPEN)
        {
            this->handle_error_state = HANDLE_OK;
            this->system_parameter.error_time = 0;
            get_step = 0;
        }

        break;

    default:
        break;
    }
}

 void fan_control(HA01_Handle *this)
{
    static uint8_t control_step = 0;
    static bool fan_run_flag = false;

    switch (control_step)
    {
    case 0:
        if (this->handle_error_state != HANDLE_OK)
        {
            control_step = 1;
            break;
        }
		if(sFWHA01_t.run_mode == Cold_Mode)
		{
			fan_run_flag = true;
            tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, this->system_parameter.cold_mode_set_air * 0.56 + 30);
		}
		else
		{
			if (this->Work_handle_state == HANDLE_SLEEP)
			{
				/* keep fan output until the temp below 60 */
				if (this->system_parameter.actual_temp >= 40)
				{
					if (fan_run_flag == false)
					{
						tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, 50);
					}
				}
				else
				{
					if (fan_run_flag == false)
					{
						fan_run_flag = true;
						/* close fan output */
						tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, 0);
					}
				}
			}
			else if (this->Work_handle_state == HANDLE_WORKING &&
					 this->handle_position == IN_POSSITION &&
					 this->sleep_state== SLEEP_OPEN)
			{
				/* wait for next time open */
				if (fan_run_flag)
				{
					fan_run_flag = false;
				}
				if (this->system_parameter.actual_temp >= 250)
				{
					this->system_parameter.sleep_air_data = SLEEP_FAN_DATA;

					if (fan_run_flag == false)
					{
						/* open fan output with a half of max set val*/
						tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, this->system_parameter.sleep_air_data * 0.56 + 30);
					}
				}
				else if (this->system_parameter.actual_temp >= 70 && this->system_parameter.actual_temp < 250)
				{
					if (fan_run_flag == false)
					{
						/* open fan output with actual temp change*/
						this->system_parameter.sleep_air_data = this->system_parameter.actual_temp * 0.4;
//						tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, this->system_parameter.sleep_air_data * 1.13 + 30);
						tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, this->system_parameter.sleep_air_data);
					}
				}
				else
				{
					/* close fan output */
					tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, 0);
				}
			}
			else if ((this->Work_handle_state == HANDLE_WORKING &&
					  this->handle_position == NOT_IN_POSSITION) ||
					 this->sleep_state== SLEEP_CLOSE)
			{
				/* wait for next time open */
				if (fan_run_flag == false)
				{
					fan_run_flag = true;

				}
				
				/* open fan output with user set val */
				tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, this->system_parameter.air_data * 0.56 + 30);
			}
		}
        break;

    case 1:
        /* open fan output with max set val */
        tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, MAX_SET_AIR * 0.56 + 30);

        if (this->handle_error_state == HANDLE_OK)
        {
            control_step = 0;
            break;
        }

        break;

    default:
        break;
    }
}


int8_t linear_correction(uint16_t user_set_temp)
{
    volatile uint8_t n = 0;
    volatile uint16_t check_start = 0;
    volatile uint8_t check_value = 0;

    if (sFWHA01_t.system_parameter.air_data > 20)
    {
        if (user_set_temp < 300)
        {
            check_value = 0;
            check_start = 100;

            for (n = 0; n < 20; n++)
            {
                if (user_set_temp <= (check_start + check_value))
                {
                    return (off_set_buff[n]);
                }
                else
                {
                    check_start += 10;
                }
            }
        }
        else
        {
            check_value = 0;
            check_start = 300;

            for (n = 20; n < 40; n++)
            {
                if (user_set_temp <= (check_start + check_value))
                {
                    return (off_set_buff[n]);
                }
                else
                {
                    check_start += 10;
                }
            }
        }

        return (off_set_buff[n]);
    }
    else if (sFWHA01_t.system_parameter.air_data <= 20 && sFWHA01_t.system_parameter.air_data > 0)
    {
        if (user_set_temp < 300)
        {
            check_value = 0;
            check_start = 100;

            for (n = 0; n < 20; n++)
            {
                if (user_set_temp <= (check_start + check_value))
                {
                    return (wind_set_20_buff[n]);
                }
                else
                {
                    check_start += 10;
                }
            }
        }
        else
        {
            check_value = 0;
            check_start = 300;

            for (n = 20; n < 40; n++)
            {
                if (user_set_temp <= (check_start + check_value))
                {
                    return (wind_set_20_buff[n]);
                }
                else
                {
                    check_start += 10;
                }
            }

            return (wind_set_20_buff[n]);
        }
    }

    return 0;
}

void pwm_control(HA01_Handle *this)
{
    static float temp = 0;
    static float set_temp = 0;
    static float last_set_temp = 0;
    static uint16_t delay_time = 0;
    static bool change_flag = false;

	static uint32_t hight_kd = 16000;
    static uint32_t low_kd = 10000;
	static float low_ki = 3;
	static float hight_ki = 4;
    if (change_flag)
    {
		if(handle_pid.Kd<= low_kd)
			handle_pid.Kd = low_kd;
    }
    else
    {
        handle_pid.Kd = hight_kd;
		handle_pid.Ki = hight_ki;
    }

	if(sFWHA01_t.run_mode == Power_Mode)
	{
		set_temp = this->system_parameter.set_temp + POWER_TEMP;
	}
	else
	{
		set_temp = this->system_parameter.set_temp;
	}
		
    if (last_set_temp != set_temp)
    {
//		PID_Clear_I(&handle_pid);
        handle_pid.SumError = handle_pid.SumError / 2;
        this->system_parameter.temp_linear_data = linear_correction(set_temp);
        last_set_temp  = set_temp;
    }
	
//	this->system_parameter.actual_temp = get_adcval_average(ADC_CHANNEL_10,16) >> 2;

//	this->system_parameter.actual_temp  = get_adcval_filtered(ADC_CHANNEL_10) >> 2;
	
	this->system_parameter.actual_temp = move_average_filter(&handle_temp) >> 2;
	
	
//    temp = this->system_parameter.actual_temp + this->system_parameter.temp_linear_data - this->system_parameter.cal_data;
	temp = this->system_parameter.actual_temp- this->system_parameter.cal_data;
	
	if(sFWHA01_t.run_mode == Cold_Mode)
	{
		change_flag = false;
		delay_time = 0;
		this->system_parameter.pwm_out = 0;
		PID_Clear(&handle_pid);
	}
	else
	{	
		if ((this->handle_position == NOT_IN_POSSITION || this->sleep_state == SLEEP_CLOSE))
		{
			if (temp <= (set_temp + PID_RANGE) && temp >= (set_temp - PID_RANGE))
			{
				if (temp <= (set_temp + 5) && temp >= (set_temp - 5))
				{
					delay_time++;

					if (delay_time >= 60)
					{
						if(handle_pid.Kd-2000<=low_kd)
						{
							handle_pid.Kd = low_kd;
							
							if (handle_pid.Ki - 0.1 <= low_ki)
							{
								handle_pid.Ki = low_ki;
							}
							else
							{
								handle_pid.Ki -= 0.1;
							}
						}
						else
						{
							handle_pid.Kd -= 2000;
							
						}
						delay_time = 0;
						change_flag = true;
					}
				}
				else
				{
					change_flag = false;                                                                                                                                                                                                                                                                                                                                                                         
					delay_time = 0;
				}
				this->system_parameter.pwm_out = PID_Position_Calc(&handle_pid, set_temp, temp);
			}
			else if (temp > (set_temp + PID_RANGE))
			{
				this->system_parameter.pwm_out = 0;
			}
			else if (temp < (set_temp - PID_RANGE))
			{
				this->system_parameter.pwm_out = MAX_PWM_OUTPUT;
			}
		}
		else if ((this->handle_position == IN_POSSITION && this->sleep_state == SLEEP_OPEN))
		{
			change_flag = false;
			delay_time = 0;
			this->system_parameter.pwm_out = 0;
			PID_Clear(&handle_pid);
		}
	}

    if (this->system_parameter.pwm_out <= 0)
    {
        this->system_parameter.pwm_out = 0;
    }

    if (this->handle_error_state!= HANDLE_OK)
    {
		change_flag = false;
			delay_time = 0;
        this->system_parameter.pwm_out = 0;
        PID_Clear(&handle_pid);
    }
}
