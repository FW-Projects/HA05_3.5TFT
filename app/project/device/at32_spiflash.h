/**
  **************************************************************************
  * @file     at32_spiflash.h
  * @brief    SPI Flash (W25Q128/W25Q256) 驱动头文件 - 修复版
  * @note     修复W25Q256掉电后花屏问题：强制处理扩展地址寄存器(EAR)并进入4字节模式
  **************************************************************************
  */

#ifndef __AT32_SPIFLASH_H
#define __AT32_SPIFLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "at32f415.h"
#include <stdio.h>  // 添加标准IO用于调试输出

/* SPI硬件定义 */
#define SPIX                                   SPI1
#define SPI_PERIPH_CLK                         CRM_SPI1_PERIPH_CLOCK

/* SPI引脚定义 */
#define SPI_CS_PIN                             GPIO_PINS_4
#define SPI_CS_GPIO_GROUP                      GPIOA
#define SPI_CS_GPIO_GROUP_CLK                  CRM_GPIOA_PERIPH_CLOCK
#define SPI_CLK_PIN                            GPIO_PINS_5
#define SPI_CLK_GPIO_GROUP                     GPIOA
#define SPI_CLK_GPIO_GROUP_CLK                 CRM_GPIOA_PERIPH_CLOCK
#define SPI_MISO_PIN                           GPIO_PINS_6
#define SPI_MISO_GPIO_GROUP                    GPIOA
#define SPI_MISO_GPIO_GROUP_CLK                CRM_GPIOA_PERIPH_CLOCK
#define SPI_MOSI_PIN                           GPIO_PINS_7
#define SPI_MOSI_GPIO_GROUP                    GPIOA
#define SPI_MOSI_GPIO_GROUP_CLK                CRM_GPIOA_PERIPH_CLOCK

/* 使用DMA传输SPI数据 */
#define SPI_TRANS_DMA
#define DMA_PERIPH_CLK                         CRM_DMA1_PERIPH_CLOCK
#define DMA_TX_CH                              DMA1_CHANNEL3
#define DMA_RX_CH                              DMA1_CHANNEL2

/* CS控制宏 */
#define FLASH_CS_HIGH()                        gpio_bits_set(SPI_CS_GPIO_GROUP, SPI_CS_PIN)
#define FLASH_CS_LOW()                         gpio_bits_reset(SPI_CS_GPIO_GROUP, SPI_CS_PIN)

/* Flash器件ID */
#define W25Q128                                0xEF17
#define W25Q256                                0xEF18  // 实际读到的ID 0xEF18

/* Flash容量参数 */
#define SPIF_SECTOR_SIZE                       4096
#define SPIF_PAGE_SIZE                         256
#define FLASH_SPI_DUMMY_BYTE                   0xA5

/* --- 基本操作命令 (3字节地址模式) --- */
#define SPIF_WRITEENABLE                       0x06
#define SPIF_WRITEDISABLE                      0x04
#define SPIF_READSTATUSREG1                    0x05
#define SPIF_READSTATUSREG2                    0x35
#define SPIF_READSTATUSREG3                    0x15    // 用于检查4字节模式状态
#define SPIF_WRITESTATUSREG3                   0x11    // 写状态寄存器3
#define SPIF_READDATA                          0x03
#define SPIF_PAGEPROGRAM                       0x02
#define SPIF_SECTORERASE                       0x20
#define SPIF_MANUFACTDEVICEID                  0x90

/* --- 4字节地址模式命令 (W25Q256) --- */
#define SPIF_4B_READDATA                       0x13
#define SPIF_4B_PAGEPROGRAM                    0x12
#define SPIF_4B_SECTORERASE                    0x21
#define SPIF_ENTER_4BYTE_ADDR_MODE             0xB7
#define SPIF_EXIT_4BYTE_ADDR_MODE              0xE9
#define SPIF_ENABLE_RESET                      0x66
#define SPIF_RESET_DEVICE                      0x99

/* --- 扩展地址寄存器(EAR)操作 (关键) --- */
#define SPIF_READ_EXT_ADDR_REG                 0xC8    // 读EAR
#define SPIF_WRITE_EXT_ADDR_REG                0xC5    // 写EAR
#define SPIF_VOLATILE_SR_WRITE_ENABLE          0x50    // 易失性状态寄存器写使能

/* --- 状态寄存器位定义 --- */
#define SPIF_SR3_4BYTE_MODE_BIT                (1 << 5) // 状态寄存器3的位5：1=4字节模式
#define SPIF_SR1_BUSY_BIT                      (1 << 0) // 状态寄存器1的位0：1=忙碌
 
/* 调试宏定义 */
#ifdef SPIFLASH_DEBUG
    #define SPIF_DEBUG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
    #define SPIF_DEBUG(fmt, ...)
#endif

/* 全局变量声明 */
//extern uint8_t spiflash_sector_buf[SPIF_SECTOR_SIZE];
//extern uint8_t is_4byte_mode;
//extern uint16_t flash_id;

/* 函数声明 */
void spiflash_init(void);
uint16_t spiflash_read_id(void);
uint8_t spiflash_read_sr1(void);
uint8_t spiflash_read_sr2(void);
uint8_t spiflash_read_sr3(void);
uint8_t spiflash_read_ear(void);
void spiflash_write_ear(uint8_t ear_value);
void spiflash_write_enable(void);
void spiflash_wait_busy(void);
void spiflash_read(uint8_t *pbuffer, uint32_t read_addr, uint32_t length);
void spiflash_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length);
void spiflash_sector_erase(uint32_t sector_addr);
void spiflash_write_nocheck(uint8_t *pbuffer, uint32_t write_addr, uint32_t length);
void spiflash_page_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length);
void spiflash_force_4byte_mode(void);
uint8_t spiflash_is_4byte_mode(void);
void spiflash_reset(void);
void spi_bytes_write(uint8_t *pbuffer, uint32_t length);
void spi_bytes_read(uint8_t *pbuffer, uint32_t length);
uint8_t spi_byte_write(uint8_t data);
uint8_t spi_byte_read(void);
void spiflash_test_boundary(void);  // 新增：测试16MB边界函数
void spiflash_dump_status(void);    // 新增：打印状态信息函数

#ifdef __cplusplus
}
#endif

#endif /* __AT32_SPIFLASH_H */