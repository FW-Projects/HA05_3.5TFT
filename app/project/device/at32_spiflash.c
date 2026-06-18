/**
  **************************************************************************
  * @file     at32_spiflash.c
  * @brief    SPI Flash (W25Q128/W25Q256) 驱动源文件 - 修复版
  * @note     修复W25Q256掉电后花屏问题：强制处理扩展地址寄存器(EAR)并进入4字节模式
  **************************************************************************
  */

#include "at32_spiflash.h"
#include "wk_system.h" /* 包含用户自定义延时函数的头文件 */

/* 全局变量 */
uint8_t spiflash_sector_buf[SPIF_SECTOR_SIZE];
static uint8_t is_4byte_mode = 0;
static uint16_t flash_id = 0;

/****************************** 底层SPI收发 ********************************/

uint8_t spi_byte_write(uint8_t data)
{
    uint8_t brxbuff;
#if defined(SPI_TRANS_DMA)
    spi_i2s_dma_transmitter_enable(SPIX, FALSE);
    spi_i2s_dma_receiver_enable(SPIX, FALSE);
#endif
    spi_i2s_data_transmit(SPIX, data);
    while(spi_i2s_flag_get(SPIX, SPI_I2S_RDBF_FLAG) == RESET);
    brxbuff = spi_i2s_data_receive(SPIX);
    while(spi_i2s_flag_get(SPIX, SPI_I2S_BF_FLAG) != RESET);
    return brxbuff;
}

uint8_t spi_byte_read(void)
{
    return (spi_byte_write(FLASH_SPI_DUMMY_BYTE));
}

/****************************** SPI批量读写 ********************************/

void spi_bytes_write(uint8_t *pbuffer, uint32_t length)
{
    volatile uint8_t dummy_data;
#if defined(SPI_TRANS_DMA)
    dma_init_type dma_init_struct;
    dma_reset(DMA_RX_CH);
    dma_reset(DMA_TX_CH);
    dma_default_para_init(&dma_init_struct);
    dma_init_struct.buffer_size = length;
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_base_addr = (uint32_t)&dummy_data;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_init_struct.memory_inc_enable = FALSE;
    dma_init_struct.peripheral_base_addr = (uint32_t)(&SPIX->dt);
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.priority = DMA_PRIORITY_VERY_HIGH;
    dma_init_struct.loop_mode_enable = FALSE;
    dma_init(DMA_RX_CH, &dma_init_struct);

    dma_init_struct.buffer_size = length;
    dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_base_addr = (uint32_t)pbuffer;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_base_addr = (uint32_t)(&SPIX->dt);
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.priority = DMA_PRIORITY_VERY_HIGH;
    dma_init_struct.loop_mode_enable = FALSE;
    dma_init(DMA_TX_CH, &dma_init_struct);

    spi_i2s_dma_transmitter_enable(SPIX, TRUE);
    spi_i2s_dma_receiver_enable(SPIX, TRUE);

    dma_channel_enable(DMA_RX_CH, TRUE);
    dma_channel_enable(DMA_TX_CH, TRUE);

    while(dma_flag_get(DMA1_FDT2_FLAG) == RESET);
    dma_flag_clear(DMA1_FDT2_FLAG);

    dma_channel_enable(DMA_RX_CH, FALSE);
    dma_channel_enable(DMA_TX_CH, FALSE);

    spi_i2s_dma_transmitter_enable(SPIX, FALSE);
    spi_i2s_dma_receiver_enable(SPIX, FALSE);
#else
    while(length--)
    {
        while(spi_i2s_flag_get(SPIX, SPI_I2S_TDBE_FLAG) == RESET);
        spi_i2s_data_transmit(SPIX, *pbuffer);
        while(spi_i2s_flag_get(SPIX, SPI_I2S_RDBF_FLAG) == RESET);
        dummy_data = spi_i2s_data_receive(SPIX);
        pbuffer++;
    }
#endif
}

void spi_bytes_read(uint8_t *pbuffer, uint32_t length)
{
    uint8_t write_value = FLASH_SPI_DUMMY_BYTE;
#if defined(SPI_TRANS_DMA)
    dma_init_type dma_init_struct;
    dma_reset(DMA_RX_CH);
    dma_reset(DMA_TX_CH);
    dma_default_para_init(&dma_init_struct);
    dma_init_struct.buffer_size = length;
    dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_base_addr = (uint32_t)&write_value;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_init_struct.memory_inc_enable = FALSE;
    dma_init_struct.peripheral_base_addr = (uint32_t)(&SPIX->dt);
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.priority = DMA_PRIORITY_VERY_HIGH;
    dma_init_struct.loop_mode_enable = FALSE;
    dma_init(DMA_TX_CH, &dma_init_struct);

    dma_init_struct.buffer_size = length;
    dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_base_addr = (uint32_t)pbuffer;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_base_addr = (uint32_t)(&SPIX->dt);
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.priority = DMA_PRIORITY_VERY_HIGH;
    dma_init_struct.loop_mode_enable = FALSE;
    dma_init(DMA_RX_CH, &dma_init_struct);

    spi_i2s_dma_transmitter_enable(SPIX, TRUE);
    spi_i2s_dma_receiver_enable(SPIX, TRUE);
    dma_channel_enable(DMA_RX_CH, TRUE);
    dma_channel_enable(DMA_TX_CH, TRUE);

    while(dma_flag_get(DMA1_FDT2_FLAG) == RESET);
    dma_flag_clear(DMA1_FDT2_FLAG);

    dma_channel_enable(DMA_RX_CH, FALSE);
    dma_channel_enable(DMA_TX_CH, FALSE);

    spi_i2s_dma_transmitter_enable(SPIX, FALSE);
    spi_i2s_dma_receiver_enable(SPIX, FALSE);
#else
    while(length--)
    {
        while(spi_i2s_flag_get(SPIX, SPI_I2S_TDBE_FLAG) == RESET);
        spi_i2s_data_transmit(SPIX, write_value);
        while(spi_i2s_flag_get(SPIX, SPI_I2S_RDBF_FLAG) == RESET);
        *pbuffer = spi_i2s_data_receive(SPIX);
        pbuffer++;
    }
#endif
}

/****************************** 基础指令与状态 ********************************/

uint16_t spiflash_read_id(void)
{
    uint16_t wreceivedata = 0;
    FLASH_CS_LOW();
    spi_byte_write(SPIF_MANUFACTDEVICEID);
    spi_byte_write(0x00);
    spi_byte_write(0x00);
    spi_byte_write(0x00);
    wreceivedata |= spi_byte_read() << 8;
    wreceivedata |= spi_byte_read();
    FLASH_CS_HIGH();
    return wreceivedata;
}

uint8_t spiflash_read_sr1(void)
{
    uint8_t breadbyte = 0;
    FLASH_CS_LOW();
    spi_byte_write(SPIF_READSTATUSREG1);
    breadbyte = (uint8_t)spi_byte_read();
    FLASH_CS_HIGH();
    return (breadbyte);
}

uint8_t spiflash_read_sr2(void)
{
    uint8_t sr2_value = 0;
    FLASH_CS_LOW();
    spi_byte_write(SPIF_READSTATUSREG2);
    sr2_value = (uint8_t)spi_byte_read();
    FLASH_CS_HIGH();
    return sr2_value;
}

uint8_t spiflash_read_sr3(void)
{
    uint8_t sr3_value = 0;
    FLASH_CS_LOW();
    spi_byte_write(SPIF_READSTATUSREG3);
    sr3_value = (uint8_t)spi_byte_read();
    FLASH_CS_HIGH();
    return sr3_value;
}

void spiflash_wait_busy(void)
{
    while((spiflash_read_sr1() & SPIF_SR1_BUSY_BIT) == SPIF_SR1_BUSY_BIT);
}

void spiflash_write_enable(void)
{
    FLASH_CS_LOW();
    spi_byte_write(SPIF_WRITEENABLE);
    FLASH_CS_HIGH();
}

/****************************** 扩展地址寄存器(EAR)操作 - 关键 ********************************/
/* 掉电后，EAR可能为1，导致地址错乱，必须先强制清零 */

uint8_t spiflash_read_ear(void)
{
    uint8_t ear_value = 0;
    FLASH_CS_LOW();
    spi_byte_write(SPIF_READ_EXT_ADDR_REG); // 0xC8
    ear_value = spi_byte_read();
    FLASH_CS_HIGH();
    return (ear_value & 0x01); // EAR只有bit0有效
}

void spiflash_write_ear(uint8_t ear_value)
{
    /* 必须先发送易失性状态寄存器写使能指令 */
    FLASH_CS_LOW();
    spi_byte_write(SPIF_VOLATILE_SR_WRITE_ENABLE); // 0x50
    FLASH_CS_HIGH();
     //wk_delay_ms(1); // 使用您的延时函数

    /* 然后执行标准写使能 */
    spiflash_write_enable();
     //wk_delay_ms(1);

    /* 最后写入EAR值 */
    FLASH_CS_LOW();
    spi_byte_write(SPIF_WRITE_EXT_ADDR_REG); // 0xC5
    spi_byte_write(ear_value & 0x01);
    FLASH_CS_HIGH();
    spiflash_wait_busy();
}

/****************************** 4字节地址模式管理 ********************************/

uint8_t spiflash_is_4byte_mode(void)
{
    return ((spiflash_read_sr3() & SPIF_SR3_4BYTE_MODE_BIT) != 0);
}

void spiflash_force_4byte_mode(void)
{
    uint8_t sr3 = spiflash_read_sr3();
    
    /* 如果已经是4字节模式，直接返回 */
    if((sr3 & SPIF_SR3_4BYTE_MODE_BIT) != 0)
    {
        is_4byte_mode = 1;
        SPIF_DEBUG("Already in 4-byte mode\n");
        return;
    }
    
    /* 写使能 */
    spiflash_write_enable();
    //wk_delay_ms(2);
    
    /* 首先确保SR3的4字节模式位被设置 */
    FLASH_CS_LOW();
    spi_byte_write(SPIF_WRITESTATUSREG3); // 0x11
    spi_byte_write(sr3 | SPIF_SR3_4BYTE_MODE_BIT); // 设置4字节模式位
    FLASH_CS_HIGH();
    spiflash_wait_busy();
    //wk_delay_ms(5);
    
    /* 发送进入4字节模式命令 */
    FLASH_CS_LOW();
    spi_byte_write(SPIF_ENTER_4BYTE_ADDR_MODE); // 0xB7
    FLASH_CS_HIGH();
    spiflash_wait_busy();
    //wk_delay_ms(5);
    
    /* 验证是否成功 */
    sr3 = spiflash_read_sr3();
    if((sr3 & SPIF_SR3_4BYTE_MODE_BIT) != 0)
    {
        is_4byte_mode = 1;
        SPIF_DEBUG("Successfully entered 4-byte mode\n");
    }
    else
    {
        SPIF_DEBUG("Failed to enter 4-byte mode, SR3=0x%02X\n", sr3);
    }
}

void spiflash_reset(void)
{
    /* 使能复位 */
    FLASH_CS_LOW();
    spi_byte_write(SPIF_ENABLE_RESET); // 0x66
    FLASH_CS_HIGH();
    //wk_delay_ms(1);
    
    /* 执行复位 */
    FLASH_CS_LOW();
    spi_byte_write(SPIF_RESET_DEVICE); // 0x99
    FLASH_CS_HIGH();
    //wk_delay_ms(50); // 增加延时，确保完全复位
}

/****************************** 初始化函数 - 核心修复 ********************************/

void spiflash_init(void)
{
    gpio_init_type gpio_initstructure;
    spi_init_type spi_init_struct;

    /* 硬件初始化 */
    crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
#if defined(SPI_TRANS_DMA)
    crm_periph_clock_enable(DMA_PERIPH_CLK, TRUE);
#endif

    /* 初始化SCK、MISO、MOSI引脚为复用模式 */
    gpio_initstructure.gpio_pins = SPI_CLK_PIN;
    gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init(SPI_CLK_GPIO_GROUP, &gpio_initstructure);

    gpio_initstructure.gpio_pins = SPI_MISO_PIN;
    gpio_init(SPI_MISO_GPIO_GROUP, &gpio_initstructure);

    gpio_initstructure.gpio_pins = SPI_MOSI_PIN;
    gpio_init(SPI_MOSI_GPIO_GROUP, &gpio_initstructure);

    /* 初始化CS引脚为输出模式 */
    gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_initstructure.gpio_pins = SPI_CS_PIN;
    gpio_init(SPI_CS_GPIO_GROUP, &gpio_initstructure);

    /* 初始化SPI外设 */
    crm_periph_clock_enable(SPI_PERIPH_CLK, TRUE);
    spi_default_para_init(&spi_init_struct);
    spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
    spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
    spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_2;
    spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
    spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
    spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
    spi_init(SPIX, &spi_init_struct);
    spi_enable(SPIX, TRUE);

    /* 设置CS为高电平（未选中状态） */
    FLASH_CS_HIGH();
    
    /* 等待Flash芯片完全上电稳定 */
    wk_delay_ms(50);

    /* ========== 核心修复流程开始 ========== */
    flash_id = spiflash_read_id();
    SPIF_DEBUG("Flash ID: 0x%04X\n", flash_id);

    if(flash_id == W25Q256)
    {
        SPIF_DEBUG("W25Q256 detected, initializing...\n");
        
        /* 步骤1：强制进行硬件复位，回到已知状态 */
        spiflash_reset();
        wk_delay_ms(20);
        
        /* 步骤2：读取当前所有状态寄存器 */
        uint8_t sr1 = spiflash_read_sr1();
        uint8_t sr3 = spiflash_read_sr3();
        uint8_t ear = spiflash_read_ear();
        SPIF_DEBUG("Initial status: SR1=0x%02X, SR3=0x%02X, EAR=0x%02X\n", sr1, sr3, ear);
        
        /* 步骤3：如果有必要，先退出4字节模式（回到3字节模式） */
        if((sr3 & SPIF_SR3_4BYTE_MODE_BIT) != 0)
        {
            SPIF_DEBUG("Exiting 4-byte mode first...\n");
            spiflash_write_enable();
            wk_delay_ms(2);
            FLASH_CS_LOW();
            spi_byte_write(SPIF_EXIT_4BYTE_ADDR_MODE); // 0xE9
            FLASH_CS_HIGH();
            spiflash_wait_busy();
            wk_delay_ms(5);
        }
        
        /* 步骤4：强制扩展地址寄存器(EAR)清零 */
        if(ear != 0)
        {
            SPIF_DEBUG("EAR not zero (0x%02X), forcing to 0...\n", ear);
            spiflash_write_ear(0);
            wk_delay_ms(5);
            ear = spiflash_read_ear();
            SPIF_DEBUG("EAR after reset: 0x%02X\n", ear);
        }
        
        /* 步骤5：检查状态寄存器3，确保4字节模式位已设置 */
        sr3 = spiflash_read_sr3();
        if((sr3 & SPIF_SR3_4BYTE_MODE_BIT) == 0)
        {
            SPIF_DEBUG("Setting SR3 4-byte mode bit...\n");
            spiflash_write_enable();
            wk_delay_ms(2);
            FLASH_CS_LOW();
            spi_byte_write(SPIF_WRITESTATUSREG3); // 0x11
            spi_byte_write(sr3 | SPIF_SR3_4BYTE_MODE_BIT); // 设置4字节模式位
            FLASH_CS_HIGH();
            spiflash_wait_busy();
            wk_delay_ms(10);
        }
        
        /* 步骤6：安全进入4字节地址模式 */
        SPIF_DEBUG("Entering 4-byte address mode...\n");
        spiflash_force_4byte_mode();
        wk_delay_ms(5);
        
        /* 步骤7：验证模式是否正确设置 */
        is_4byte_mode = spiflash_is_4byte_mode();
        ear = spiflash_read_ear();
        SPIF_DEBUG("Final status: 4-byte mode=%d, EAR=0x%02X\n", is_4byte_mode, ear);
        
        /* 步骤8：读取前几个字节验证通信正常 */
        uint8_t test_buf[8];
        spiflash_read(test_buf, 0x00000000, 8);
        SPIF_DEBUG("First 8 bytes: ");
        for(int i = 0; i < 8; i++)
        {
            SPIF_DEBUG("%02X ", test_buf[i]);
        }
        SPIF_DEBUG("\n");
        
        SPIF_DEBUG("W25Q256 initialization completed successfully\n");
    }
    else if(flash_id == W25Q128)
    {
        is_4byte_mode = 0;
        SPIF_DEBUG("W25Q128 detected, 3-byte address mode\n");
    }
    else
    {
        SPIF_DEBUG("Unknown Flash ID: 0x%04X\n", flash_id);
    }
    /* ========== 核心修复流程结束 ========== */
}

/****************************** 主要读写擦除函数 ********************************/

void spiflash_read(uint8_t *pbuffer, uint32_t read_addr, uint32_t length)
{
    /* 对于W25Q256，确保处于正确的模式 */
    if(flash_id == W25Q256)
    {
        /* 如果尚未进入4字节模式，强制进入 */
        if(!is_4byte_mode)
        {
            spiflash_force_4byte_mode();
            //wk_delay_ms(1);
        }
        
        /* 如果读取16MB以下的地址，确保EAR为0 */
        if(read_addr < 0x1000000)
        {
            uint8_t current_ear = spiflash_read_ear();
            if(current_ear != 0)
            {
                SPIF_DEBUG("Warning: EAR not zero (0x%02X) when reading low address, fixing...\n", current_ear);
                spiflash_write_ear(0);
                //wk_delay_ms(1);
            }
        }
    }

    FLASH_CS_LOW();
    
    if((flash_id == W25Q256) && is_4byte_mode)
    {
        /* 4字节地址模式读取 */
        spi_byte_write(SPIF_4B_READDATA); // 0x13
        spi_byte_write((uint8_t)(read_addr >> 24));
        spi_byte_write((uint8_t)(read_addr >> 16));
        spi_byte_write((uint8_t)(read_addr >> 8));
        spi_byte_write((uint8_t)read_addr);
    }
    else
    {
        /* 3字节地址模式读取 */
        spi_byte_write(SPIF_READDATA); // 0x03
        
        /* 对于W25Q256在3字节模式下，地址需要截断到24位 */
        uint32_t eff_addr = read_addr;
        if(flash_id == W25Q256)
        {
            eff_addr = read_addr & 0xFFFFFF;
        }
        
        spi_byte_write((uint8_t)(eff_addr >> 16));
        spi_byte_write((uint8_t)(eff_addr >> 8));
        spi_byte_write((uint8_t)eff_addr);
    }
    
    spi_bytes_read(pbuffer, length);
    FLASH_CS_HIGH();
}

void spiflash_page_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
    if((0 < length) && (length <= SPIF_PAGE_SIZE))
    {
        spiflash_write_enable();
        //wk_delay_ms(1);

        FLASH_CS_LOW();
        
        if((flash_id == W25Q256) && is_4byte_mode)
        {
            /* 4字节地址模式页编程 */
            spi_byte_write(SPIF_4B_PAGEPROGRAM); // 0x12
            spi_byte_write((uint8_t)(write_addr >> 24));
            spi_byte_write((uint8_t)(write_addr >> 16));
            spi_byte_write((uint8_t)(write_addr >> 8));
            spi_byte_write((uint8_t)write_addr);
        }
        else
        {
            /* 3字节地址模式页编程 */
            spi_byte_write(SPIF_PAGEPROGRAM); // 0x02
            
            uint32_t eff_addr = write_addr;
            if(flash_id == W25Q256)
            {
                eff_addr = write_addr & 0xFFFFFF;
            }
            
            spi_byte_write((uint8_t)(eff_addr >> 16));
            spi_byte_write((uint8_t)(eff_addr >> 8));
            spi_byte_write((uint8_t)eff_addr);
        }
        
        spi_bytes_write(pbuffer, length);
        FLASH_CS_HIGH();
        spiflash_wait_busy();
    }
}

void spiflash_sector_erase(uint32_t erase_addr)
{
    uint32_t byte_addr = erase_addr * SPIF_SECTOR_SIZE;
    spiflash_write_enable();
    spiflash_wait_busy();
    
    FLASH_CS_LOW();
    
    if((flash_id == W25Q256) && is_4byte_mode)
    {
        /* 4字节地址模式扇区擦除 */
        spi_byte_write(SPIF_4B_SECTORERASE); // 0x21
        spi_byte_write((uint8_t)(byte_addr >> 24));
        spi_byte_write((uint8_t)(byte_addr >> 16));
        spi_byte_write((uint8_t)(byte_addr >> 8));
        spi_byte_write((uint8_t)byte_addr);
    }
    else
    {
        /* 3字节地址模式扇区擦除 */
        spi_byte_write(SPIF_SECTORERASE); // 0x20
        
        uint32_t eff_addr = byte_addr;
        if(flash_id == W25Q256)
        {
            eff_addr = byte_addr & 0xFFFFFF;
        }
        
        spi_byte_write((uint8_t)(eff_addr >> 16));
        spi_byte_write((uint8_t)(eff_addr >> 8));
        spi_byte_write((uint8_t)eff_addr);
    }
    
    FLASH_CS_HIGH();
    spiflash_wait_busy();
}

/****************************** 辅助写入函数 ********************************/

void spiflash_write_nocheck(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
    uint16_t page_remain;
    page_remain = SPIF_PAGE_SIZE - write_addr % SPIF_PAGE_SIZE;
    if(length <= page_remain) page_remain = length;
    
    while(1)
    {
        spiflash_page_write(pbuffer, write_addr, page_remain);
        if(length == page_remain) break;
        else
        {
            pbuffer += page_remain;
            write_addr += page_remain;
            length -= page_remain;
            page_remain = (length > SPIF_PAGE_SIZE) ? SPIF_PAGE_SIZE : length;
        }
    }
}

void spiflash_write(uint8_t *pbuffer, uint32_t write_addr, uint32_t length)
{
    uint32_t sector_pos;
    uint16_t sector_offset;
    uint16_t sector_remain;
    uint16_t i;
    uint8_t *spiflash_buf = spiflash_sector_buf;

    sector_pos = write_addr / SPIF_SECTOR_SIZE;
    sector_offset = write_addr % SPIF_SECTOR_SIZE;
    sector_remain = SPIF_SECTOR_SIZE - sector_offset;
    if(length <= sector_remain) sector_remain = length;

    while(1)
    {
        spiflash_read(spiflash_buf, sector_pos * SPIF_SECTOR_SIZE, SPIF_SECTOR_SIZE);
        for(i = 0; i < sector_remain; i++)
        {
            if(spiflash_buf[sector_offset + i] != 0xFF) break;
        }
        if(i < sector_remain)
        {
            spiflash_sector_erase(sector_pos);
            for(i = 0; i < sector_remain; i++)
            {
                spiflash_buf[i + sector_offset] = pbuffer[i];
            }
            spiflash_write_nocheck(spiflash_buf, sector_pos * SPIF_SECTOR_SIZE, SPIF_SECTOR_SIZE);
        }
        else
        {
            spiflash_write_nocheck(pbuffer, write_addr, sector_remain);
        }
        if(length == sector_remain) break;
        else
        {
            sector_pos++;
            sector_offset = 0;
            pbuffer += sector_remain;
            write_addr += sector_remain;
            length -= sector_remain;
            sector_remain = (length > SPIF_SECTOR_SIZE) ? SPIF_SECTOR_SIZE : length;
        }
    }
}

/****************************** 调试和测试函数 ********************************/

void spiflash_dump_status(void)
{
    uint16_t id = spiflash_read_id();
    SPIF_DEBUG("Flash ID: 0x%04X\n", id);
    
    if(id == W25Q256)
    {
        uint8_t sr1 = spiflash_read_sr1();
        uint8_t sr2 = spiflash_read_sr2();
        uint8_t sr3 = spiflash_read_sr3();
        uint8_t ear = spiflash_read_ear();
        
        SPIF_DEBUG("Status Registers:\n");
        SPIF_DEBUG("  SR1: 0x%02X (BUSY=%d)\n", sr1, (sr1 & SPIF_SR1_BUSY_BIT) ? 1 : 0);
        SPIF_DEBUG("  SR2: 0x%02X\n", sr2);
        SPIF_DEBUG("  SR3: 0x%02X (4-byte mode=%d)\n", 
                  sr3, (sr3 & SPIF_SR3_4BYTE_MODE_BIT) ? 1 : 0);
        SPIF_DEBUG("  EAR: 0x%02X\n", ear);
        SPIF_DEBUG("  4-byte mode flag: %d\n", is_4byte_mode);
    }
}

void spiflash_test_boundary(void)
{
    if(flash_id != W25Q256)
    {
        SPIF_DEBUG("Test only for W25Q256\n");
        return;
    }
    
    SPIF_DEBUG("Testing 16MB boundary...\n");
    
    uint8_t test_buf[32];
    
    /* 测试16MB边界前（0x00FFFFF0-0x00FFFFFF） */
    SPIF_DEBUG("Reading 0x00FFFFF0-0x00FFFFFF (last 16 bytes of first 16MB):\n");
    spiflash_read(test_buf, 0x00FFFFF0, 16);
    for(int i = 0; i < 16; i++)
    {
        SPIF_DEBUG("%02X ", test_buf[i]);
    }
    SPIF_DEBUG("\n");
    
    /* 测试16MB边界后（0x01000000-0x0100000F） */
    SPIF_DEBUG("Reading 0x01000000-0x0100000F (first 16 bytes of second 16MB):\n");
    spiflash_read(test_buf, 0x01000000, 16);
    for(int i = 0; i < 16; i++)
    {
        SPIF_DEBUG("%02X ", test_buf[i]);
    }
    SPIF_DEBUG("\n");
    
    /* 测试EAR值 */
    uint8_t ear = spiflash_read_ear();
    SPIF_DEBUG("Current EAR: 0x%02X\n", ear);
    SPIF_DEBUG("Expected: EAR=0x00 for addresses < 0x1000000, EAR=0x01 for >= 0x1000000\n");
}