/* add user code begin Header */
/**
 **************************************************************************
 * @file     main.c
 * @brief    main program
 **************************************************************************
 *                       Copyright notice & Disclaimer
 *
 * The software Board Support Package (BSP) that is made available to
 * download from Artery official website is the copyrighted work of Artery.
 * Artery authorizes customers to use, copy, and distribute the BSP
 * software and its related documentation for the purpose of design and
 * development in conjunction with Artery microcontrollers. Use of the
 * software is governed by this copyright notice and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
 * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
 * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
 * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
 * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 *
 **************************************************************************
 */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32f415_wk_config.h"
#include "wk_adc.h"
#include "wk_crc.h"
#include "wk_debug.h"
#include "wk_spi.h"
#include "wk_tmr.h"
#include "wk_usart.h"
#include "wk_wdt.h"
#include "wk_dma.h"
#include "wk_gpio.h"
#include "wk_system.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "tmt.h"
#include "lcd_handle.h"
#include "key_handle.h"
#include "beep_handle.h"
#include "output_handle.h"
#include "flash_handle.h"
#include "work_handle.h"
#include "at32_spiflash.h"
#include "PID_operation.h"
#include "HA01_handle.h"
#include "ec11_handle.h"
#include "lcd.h"
#include "adc_filter.h"
#include "PC_comm_handle.h"
#include "iap.h"
//#include "perf_counter.h"
//#include "EventRecorder.h"
/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */

/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */
#define FEED_DOG_TIME 1000
#define SPI1_RD_GPIO_PORT GPIOF
#define SPI1_RD_PIN GPIO_PINS_5
/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */
uint8_t beep_flag = 0;

extern uint16_t last_pid_out;
extern uint16_t pid_out;
/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */
void key_task(void);
void beep_task(void);
void lcd_task(void);
void output_task(void);
void debug_task(void);
void flash_task(void);
void work_task(void);
void ec11_task(void);
void feed_dog_task(void);
void check_flash_updata(void);
void pc_task(void);
void iap_task(void);
void check_and_show_modul(void);
void setting_modul(void);
/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* add user code begin 1 */
	nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x2000);
  /* add user code end 1 */

  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /* init debug function. */
  wk_debug_config();

  /* nvic config. */
  wk_nvic_config();

  /* timebase config. */
  wk_timebase_init();

  /* init gpio function. */
  wk_gpio_config();

  /* init dma1 channel1 */
  wk_dma1_channel1_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL1, 
                        DMA1_CHANNEL1_PERIPHERAL_BASE_ADDR, 
                        DMA1_CHANNEL1_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL1_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* init dma1 channel2 */
  wk_dma1_channel2_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL2, 
                        (uint32_t)&SPI1->dt, 
                        DMA1_CHANNEL2_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL2_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL2, FALSE);

  /* init dma1 channel3 */
  wk_dma1_channel3_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL3,  	
                        (uint32_t)&SPI1->dt, 
                        DMA1_CHANNEL3_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL3_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL3, FALSE);

  /* init usart1 function. */
  wk_usart1_init();

  /* init spi1 function. */
//  wk_spi1_init();

  /* init adc1 function. */
  wk_adc1_init();

  /* init tmr2 function. */
  wk_tmr2_init();

  /* init tmr5 function. */
  wk_tmr5_init();

  /* init tmr9 function. */
  wk_tmr9_init();

  /* init crc function. */
  wk_crc_init();

  /* init wdt function. */
  wk_wdt_init();

  /* add user code begin 2 */
  /* 优化方向：1.提高定时器分辨率 2.更换合适滤波算法 3.让小数点参与到运算中,减少输入误差 */
//  PID_Init(&handle_pid,500,4,45000, MAX_PWM_OUTPUT);  //300度
  PID_Init(&handle_pid,500,4,45000, MAX_PWM_OUTPUT);
	
  tmt_init();
    
  /* 读取W25Q128 ID */
  tmt.create(iap_task, IAP_HANDLE_TIME);
  tmt.create(feed_dog_task, FEED_DOG_TIME);
  tmt.create(lcd_task, LCD_HANDLE_TIME);
  tmt.create(output_task, OUTPUT_HANDLE_TIME);
  tmt.create(flash_task, FLASH_HANDLE_TIME);
  tmt.create(work_task, WORK_HANDLE_TIME);
  tmt.create(key_task, KEY_HANDLE_TIME);
  tmt.create(pc_task, PC_HANDLE_TIME);
  filter_init(&handle_temp, ADC_CHANNEL_10);
  FWHA01_Init(&sFWHA01_t);
  iap_init();
  BSP_UsartInit();
  spiflash_init();
  LCD_Init();
  LCD_Clear(BLACK);
  wk_delay_ms(200);
  gpio_bits_set(GPIOC, GPIO_PINS_4);
  check_flash_updata();
  check_and_show_modul();
  
  /* 等待系统上电稳定延时 */
  wk_delay_ms(500);
  /* add user code end 2 */

  while(1)
  {
    /* add user code begin 3 */
	tmt.run();
    /* add user code end 3 */
  }
}

  /* add user code begin 4 */

void iap_task(void)
{
    iap_command_handle();
}


void check_flash_updata(void)
{
    /* check flash updata */
    if (gpio_input_data_bit_read(SPI1_RD_GPIO_PORT, SPI1_RD_PIN))
    {
        LCD_ShowString(176, 152, (uint8_t *)"updata flash....", RED, BLACK, 16, 0);
        spi_enable(SPI1, FALSE);
    }

    while (gpio_input_data_bit_read(SPI1_RD_GPIO_PORT, SPI1_RD_PIN));

    spi_enable(SPI1, TRUE);
}

void key_task(void)
{
	if(sFWHA01_t.flash_updata)
	{
		return;
	}
    if (sFWHA01_t.init_flag && sFWHA01_t.handle_error_state == HANDLE_OK)
        key_handle();
}



void pc_task(void)
{
	pc_comm_handle();
}


void lcd_task(void)
{
	static bool is_first = false;
	
	if(sFWHA01_t.flash_updata)
	{
		if (is_first == false)
		{
			LCD_Clear(BLACK);
			LCD_ShowString(176, 152, (uint8_t *)"updata flash....", RED, BLACK, 16, 0);
			is_first = true;
		}
	}
	else
	{
		if (sFWHA01_t.init_flag)
			LcdProc();
	}

}

void output_task(void)
{
	if(sFWHA01_t.flash_updata)
	{
		return;
	}
	
    if (sFWHA01_t.init_flag)
		output_handle();
}

void debug_task(void)
{
    //printf("%d,%d,%d\n", pid_out, actual_temp, set_temp);
}

void flash_task(void)
{
	if(sFWHA01_t.flash_updata)
	{
		return;
	}
    FlashProc();
}

void work_task(void)
{
	#if 0
    WorkProc();
	#endif
}



void feed_dog_task(void)
{
  static bool first_in = false;
  if (first_in == false)
  {
    first_in = true;
    /* if enabled, please feed the dog through wdt_counter_reload() function */
  	 wdt_enable();
  }
  wdt_counter_reload();
}

void check_and_show_modul(void)
{
	
	sFWHA01_t.model = HA05;
    TranferPicturetoTFT_LCD(0, 0, 480, 320, LOGO_HA05);
	TranferPicturetoTFT_LCD(0, 0, 480, 320, LOGO_HA05);
}


/* add user code end 4 */
