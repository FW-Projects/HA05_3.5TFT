#include "work_handle.h"
#include "lcd_handle.h"
#include "PID_operation.h"
#include "HA01_handle.h"
extern uint8_t beep_flag;
work_handle_t swork =
{
    .state = INIT,
    .sleep_time = 0,
    .error_time = 0,
    .beep_open = 0,
};
volatile static work_state_t last_work_state;
void WorkProc(void)
{
    switch (sFWHA01_t.Work_handle_state)
    {
    case INIT:
        if (!gpio_input_data_bit_read(GPIOC, GPIO_PINS_13))
        {
            swork.state = STOP;
            break;
        }
        else
        {
            swork.state = WORKING;
            break;
        }
        last_work_state = INIT;
    
	case STOP:
        if (actual_temp >= 560)
        {
            swork.error_time++;
            if (swork.error_time >= ERROR_TIME_OUT/2)
            {       
                swork.state = OVER_TEMP_ERR;
				last_work_state = STOP; 
                break;
            }
        }
        else
        { 
            swork.error_time = 0;
        }

        if (!gpio_input_data_bit_read(GPIOA, GPIO_PINS_8))
        {
            if (actual_temp <= 70)
            {
                swork.sleep_time ++;
                if (swork.sleep_time >= 5)
                {
                    swork.sleep_time = 0;
                    beep_flag = BEEP_LONG;
                    swork.state = SLEEP;
					last_work_state = STOP;
                    break;
                }
            }
            else
            {
                swork.sleep_time = 0;
            }
            swork.state = STOP;
            break;
        }
        else
        {                                                                                            
            swork.state = WORKING;                                                                                                                                                
			last_work_state = STOP;
            break;
        } 
		
    case WORKING:
        if (actual_temp <= 50 || actual_temp >= 560)
        {
            swork.error_time++;

			if (swork.error_time >= ERROR_TIME_OUT/2)
			{
			    if (actual_temp >= 560)
                {
                    swork.state = OVER_TEMP_ERR;
                    break; 
                }   
			} 
            if (swork.error_time >= ERROR_TIME_OUT)
            {
                if (actual_temp <= 50)
                {
                    swork.state = LOW_TEMP_ERR;
					last_work_state = WORKING;
                    break;
                }
            }
        }
        else
        {
            swork.error_time = 0; 
        }

        if (!gpio_input_data_bit_read(GPIOA, GPIO_PINS_8))
        {
            swork.state = STOP;
			last_work_state = WORKING;
            break;
        }
        else
        {
            swork.state = WORKING;
            break;
        }

    case SLEEP:

        if (gpio_input_data_bit_read(GPIOA, GPIO_PINS_8))
        {
            swork.state = WORKING;
			last_work_state = SLEEP;
        }

        break;

    case OVER_TEMP_ERR:
        break;
    case LOW_TEMP_ERR:
        break;
    }
}
