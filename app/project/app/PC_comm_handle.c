#include "PC_comm_handle.h"
#include "iap.h"
#include "iap_usart.h"
#include "HA01_HANDLE.h"
#include "lcd_handle.h"
#include "at32_spiflash.h"
#include "wk_system.h"

PC_DATA_t pc_data;
pc_event_e pc_event = PC_END_EVENT;

// 发送控制变量（与中断共用）
uint8_t *tx_buffer = NULL;
uint16_t tx_remain = 0;



// ========== SPI Flash 升级相关变量 ==========
#define SPI_FLASH_UPDATE_START_ADDR   0x00000000   // W25Q128起始地址
#define UPDATE_PACKET_SIZE            1036        // 完整数据包大小
#define UPDATE_DATA_OFFSET            7           // 有效数据偏移
#define UPDATE_DATA_LEN               1024        // 每包有效数据长度

static uint32_t current_flash_addr = SPI_FLASH_UPDATE_START_ADDR;

// 版本解析变量
static uint8_t sw_ver_major, sw_ver_minor, sw_ver_patch;
static uint8_t hw_ver_major, hw_ver_minor, hw_ver_patch;

extern uint8_t LOCAL_DEVICE_ID_1;
extern uint8_t LOCAL_DEVICE_ID_2;



static void RecvDataFromPC(PC_DATA_t *pc);
static void RecvFlashDataFromPC(PC_DATA_t *pc);

static void WriteDataToPC(PC_DATA_t * pc,
                          uint16_t cmd_1,  uint16_t cmd_2,
                          uint16_t id,     uint16_t data_len,
                          uint16_t data_1, uint16_t data_2,
                          uint16_t data_3, uint16_t data_4,
                          uint16_t data_5);
void pc_event_handle(void);
void send_heartbeat_data(PC_DATA_t * pc, HA01_Handle *this);
static int parse_version_string(const char *str, uint8_t *major, uint8_t *minor, uint8_t *patch);
void init_version_parse(void);
void flash_ask_ok(PC_DATA_t *pc);
void flash_ask_error(PC_DATA_t *pc);
uint32_t GetMcuInfo(void); 

extern bool show_direct_currtne_temp_flag;
extern bool show_direct_set_temp_flag;
extern bool show_direct_set_wind_flag;
extern float direct_handle_pid_out;


void pc_comm_handle(void)
{
    static uint16_t time_1 = 0;
    static uint16_t time_2 = 0;

    time_1++;
    time_2++;

    if (time_1 >= 5)
    {
        pc_event_handle();
        time_1 = 0;
    }

    if (time_2 >= 100)
    {
		if(!sFWHA01_t.flash_updata)
		{
			send_heartbeat_data(&pc_data, &sFWHA01_t);
		}
        time_2 = 0;
    }

    RecvDataFromPC(&pc_data);
	
	RecvFlashDataFromPC(&pc_data);
}

static void RecvDataFromPC(PC_DATA_t *pc)
{
    uint32_t crc_value;
    pc->read_size = usart_receiveData(PC_USART, pc->rx_buff);

    if (pc->read_size == PC_MAX_RECV_SIZE)
    {
        if (PC_CHECK_HEAD(pc->rx_buff[PC_HEAD1], pc->rx_buff[PC_HEAD2]))
        {
            /* crc check */
            memset(pc->check_crc_buff, 0, PC_CRC_BUFF_SIZE);
            convert_data(pc->rx_buff, pc->check_crc_buff, PC_CMD1, PC_DATA5_LEN_L);
            crc_value = crc_block_calculate(pc->check_crc_buff, 4);
            crc_data_reset();

            if (pc->rx_buff[PC_CRC32_1] == ((crc_value >> 24) & 0xff)  &&
                    pc->rx_buff[PC_CRC32_2] == ((crc_value >> 16) & 0xff)  &&
                    pc->rx_buff[PC_CRC32_3] == ((crc_value >> 8)  & 0xff)   &&
                    pc->rx_buff[PC_CRC32_4] == (crc_value & 0xff))
            {
                /* connect PC */
                if (pc->rx_buff[PC_CMD1] == 0x01 && pc->rx_buff[PC_CMD2] == 0x01 && pc->rx_buff[PC_ID_L] == 0x00)
                {
                    pc_event = CONNECT_PC_EVENT;
                }
                /* jump to bootloader */
                else if (pc->rx_buff[PC_CMD1] == PC_FIRMWARE_UPDATE && pc->rx_buff[PC_CMD2] == PC_FIRMWARE_UPDATE_CONNECT
                         && pc->rx_buff[PC_ID_L] == 0x00)
                {
                    pc_event = IAP_EVENT;
                }
				else if (pc->rx_buff[PC_CMD1] == PC_UI_UPDATE && pc->rx_buff[PC_CMD2] == PC_FIRMWARE_UPDATE_CONNECT
                         && pc->rx_buff[PC_ID_L] == 0x00)
                {
                    pc_event = FLASU_UPDATA_EVENT;
                }
                /* other cmd */
                else
                {
                    __NOP();
                }
            }
        }

        pc->read_size = 0;
    }
}
static void RecvFlashDataFromPC(PC_DATA_t *pc)
{
    if (sFWHA01_t.flash_updata)
    {
        pc->read_size = usart_receiveData(PC_USART, pc->rx_buff);

        if (pc->read_size != UPDATE_PACKET_SIZE)
        {
            return;
        }

        if (PC_CHECK_HEAD(pc->rx_buff[PC_HEAD1], pc->rx_buff[1035]))
        {
            memset(pc->check_crc_buff, 0, PC_CRC_BUFF_SIZE);
            convert_data(pc->rx_buff, pc->check_crc_buff, 1, 1030);
            uint32_t crc_value = crc_block_calculate(pc->check_crc_buff, 258);
            crc_data_reset();

            if (pc->rx_buff[1031] == ((crc_value >> 24) & 0xff)  &&
                    pc->rx_buff[1032] == ((crc_value >> 16) & 0xff)  &&
                    pc->rx_buff[1033] == ((crc_value >> 8)  & 0xff)   &&
                    pc->rx_buff[1034] == (crc_value & 0xff))
            {
                 spiflash_write(&pc_data.rx_buff[UPDATE_DATA_OFFSET],
                                current_flash_addr, UPDATE_DATA_LEN);
				
                current_flash_addr += 1024;
                flash_ask_ok(&pc_data);

                if (pc->rx_buff[2] == 0x04)
                {
					wk_delay_ms(20);
                    nvic_system_reset();
					
                }
            }
            else
            {
                flash_ask_error(&pc_data);
            }
        }
        else
        {
            flash_ask_error(&pc_data);
        }
        memset(pc->rx_buff, 0, PC_RX_BUFF_SIZE);
        pc->read_size = 0;
    }
}

static void WriteDataToPC(PC_DATA_t * pc,
                          uint16_t cmd_1,  uint16_t cmd_2,
                          uint16_t id,     uint16_t data_len,
                          uint16_t data_1, uint16_t data_2,
                          uint16_t data_3, uint16_t data_4,
                          uint16_t data_5)
{
    static uint32_t crc_value;
    pc->tx_buff[PC_HEAD1] = PC_HEAD_1;
    pc->tx_buff[PC_CMD1] = cmd_1;
    pc->tx_buff[PC_CMD2] = cmd_2;
    pc->tx_buff[PC_ID_H] = (uint8_t)((id >> 8) & 0xff);
    pc->tx_buff[PC_ID_L] = (uint8_t)((id & 0XFF));
    pc->tx_buff[PC_DATA_LEN_H] = (uint8_t)((data_len >> 8) & 0xff)	;
    pc->tx_buff[PC_DATA_LEN_L] = (uint8_t)((data_len & 0XFF));
    pc->tx_buff[PC_DATA1_LEN_H] = (uint8_t)((data_1 >> 8) & 0xff);
    pc->tx_buff[PC_DATA1_LEN_L] = (uint8_t)((data_1 & 0XFF));
    pc->tx_buff[PC_DATA2_LEN_H] = (uint8_t)((data_2 >> 8) & 0xff);
    pc->tx_buff[PC_DATA2_LEN_L] = (uint8_t)((data_2 & 0XFF));
    pc->tx_buff[PC_DATA3_LEN_H] = (uint8_t)((data_3 >> 8) & 0xff);
    pc->tx_buff[PC_DATA3_LEN_L] = (uint8_t)((data_3 & 0XFF));
    pc->tx_buff[PC_DATA4_LEN_H] = (uint8_t)((data_4 >> 8) & 0xff);
    pc->tx_buff[PC_DATA4_LEN_L] = (uint8_t)((data_4 & 0XFF));
    pc->tx_buff[PC_DATA5_LEN_H] = (uint8_t)((data_5 >> 8) & 0xff);
    pc->tx_buff[PC_DATA5_LEN_L] = (uint8_t)((data_5 & 0XFF));
    memset(pc->check_crc_buff, 0, PC_CRC_BUFF_SIZE);
    convert_data(pc->tx_buff, pc->check_crc_buff, PC_CMD1, PC_DATA5_LEN_L);
    crc_value = crc_block_calculate(pc->check_crc_buff, 4);
    crc_data_reset();
    pc->tx_buff[PC_CRC32_1] = ((crc_value >> 24) & 0xff);
    pc->tx_buff[PC_CRC32_2] = ((crc_value >> 16) & 0xff);
    pc->tx_buff[PC_CRC32_3] = ((crc_value >> 8) & 0xff);
    pc->tx_buff[PC_CRC32_4] = (crc_value & 0xff);
    pc->tx_buff[PC_HEAD2] = PC_HEAD_2;
    /* send data */
    usart_sendData(PC_USART, pc->tx_buff, PC_MAX_SEND_SIZE);
}

void pc_event_handle(void)
{
	static bool first_in = true;
	static uint32_t crc_value;
    static uint32_t uid_value;

    if (first_in)
    {
        init_version_parse();
		uid_value = GetMcuInfo();
        first_in = false;
    }
	
    switch (pc_event)
    {
    case CONNECT_PC_EVENT:
        pc_data.tx_buff[PC_HEAD1] = PC_HEAD_1;
	
        pc_data.tx_buff[PC_CMD1] = 0x01;
        pc_data.tx_buff[PC_CMD2] = 0x01;
	
        pc_data.tx_buff[PC_ID_H] = LOCAL_DEVECE_ID_2;
        pc_data.tx_buff[PC_ID_L] = LOCAL_DEVECE_ID_1;
	
        pc_data.tx_buff[PC_DATA_LEN_H] =  0x00;
        pc_data.tx_buff[PC_DATA_LEN_L] =  0x0A;
	
        pc_data.tx_buff[PC_DATA1_LEN_H] = sw_ver_major;
        pc_data.tx_buff[PC_DATA1_LEN_L] = sw_ver_minor;
        pc_data.tx_buff[PC_DATA2_LEN_H] = sw_ver_patch;
	
        pc_data.tx_buff[PC_DATA2_LEN_L] = hw_ver_major;
        pc_data.tx_buff[PC_DATA3_LEN_H] = hw_ver_minor;
        pc_data.tx_buff[PC_DATA3_LEN_L] = hw_ver_patch;
	
        pc_data.tx_buff[PC_DATA4_LEN_H] = ((uid_value >> 24) & 0xff);;
        pc_data.tx_buff[PC_DATA4_LEN_L] = ((uid_value >> 16) & 0xff);;
		
        pc_data.tx_buff[PC_DATA5_LEN_H] = ((uid_value >> 8) & 0xff);;
        pc_data.tx_buff[PC_DATA5_LEN_L] = (uid_value & 0xff);;
		
        memset(pc_data.check_crc_buff, 0, PC_CRC_BUFF_SIZE);
        convert_data(pc_data.tx_buff, pc_data.check_crc_buff, PC_CMD1, PC_DATA5_LEN_L);
        crc_value = crc_block_calculate(pc_data.check_crc_buff, 4);
        crc_data_reset();
		
        pc_data.tx_buff[PC_CRC32_1] = ((crc_value >> 24) & 0xff);
        pc_data.tx_buff[PC_CRC32_2] = ((crc_value >> 16) & 0xff);
        pc_data.tx_buff[PC_CRC32_3] = ((crc_value >> 8) & 0xff);
        pc_data.tx_buff[PC_CRC32_4] = (crc_value & 0xff);
        pc_data.tx_buff[PC_HEAD2] = PC_HEAD_2;
		/* send data */
        usart_sendData(PC_USART, pc_data.tx_buff, PC_MAX_SEND_SIZE);
 
        pc_event = PC_END_EVENT;
 
        break;

    case IAP_EVENT:
        pc_event = PC_END_EVENT;
        iap_flag = IAP_REV_FLAG_DONE;
        break;
	case FLASU_UPDATA_EVENT:
        sFWHA01_t.flash_updata = true;
	    memset(pc_data.rx_buff, 0, PC_RX_BUFF_SIZE);
        flash_ask_ok(&pc_data);
        pc_event = PC_END_EVENT;
        break;
    case PC_END_EVENT:
        break;
    }
}

void send_heartbeat_data(PC_DATA_t * pc, HA01_Handle *this)
{
    static uint32_t crc_value;
    static uint32_t convert_result;
	
    pc->tx_buff[0]        = PC_HEAD_1;
    pc->tx_buff[1]        = 0x01;
    pc->tx_buff[2]        = 0x02;
    pc->tx_buff[3]        = LOCAL_DEVECE_ID_2;
    pc->tx_buff[4]        = LOCAL_DEVECE_ID_1;
    pc->tx_buff[5]        = 0x00;
    pc->tx_buff[6]        = 0x06;
    pc->tx_buff[7]        = (uint8_t)((((uint32_t)temp_to_display) >> 8) & 0xff);
    pc->tx_buff[8]        = (uint8_t)((((uint32_t)temp_to_display) & 0XFF));
    pc->tx_buff[9]        = (uint8_t)wind_to_display;
    pc->tx_buff[10]       = (uint8_t)this->temp_unit;
    pc->tx_buff[11]       = 0x00;
    pc->tx_buff[12]       = 0x00;
    memset(pc->check_crc_buff, 0, PC_CRC_BUFF_SIZE);
    convert_data(pc->tx_buff, pc->check_crc_buff, 1, 12);
    crc_value = crc_block_calculate(pc->check_crc_buff,3);
    crc_data_reset();
    pc->tx_buff[13] = ((crc_value >> 24) & 0xff);
    pc->tx_buff[14] = ((crc_value >> 16) & 0xff);
    pc->tx_buff[15] = ((crc_value >> 8) & 0xff);
    pc->tx_buff[16] = (crc_value & 0xff);
    pc->tx_buff[17] = PC_HEAD_2;
	 /* send data */
    usart_sendData(PC_USART, pc->tx_buff, 18);
}
    


void flash_ask_ok(PC_DATA_t *pc)
{
    uint32_t crc_value = 0;
    pc->tx_buff[0] = PC_HEAD_1;
    pc->tx_buff[1] = 0x03;
    pc->tx_buff[2] = 0x02;
    pc->tx_buff[3] = LOCAL_DEVECE_ID_2;
    pc->tx_buff[4] = LOCAL_DEVECE_ID_1;
    pc->tx_buff[5] = 0x00;
    pc->tx_buff[6] = 0x0A;
    pc->tx_buff[7] = 0x1D;
    pc->tx_buff[8] = 0x00;
    pc->tx_buff[9] = 0x00;
    pc->tx_buff[10] = 0x00;
    pc->tx_buff[11] = 0x00;
    pc->tx_buff[12] = 0x00;
    pc->tx_buff[13] = 0x00;
    pc->tx_buff[14] = 0x00;
    pc->tx_buff[15] = 0x00;
    pc->tx_buff[16] = 0x00;
    memset(pc->check_crc_buff, 0, PC_CRC_BUFF_SIZE);
    convert_data(pc->tx_buff, pc->check_crc_buff, 1, 16);
    crc_value = crc_block_calculate(pc->check_crc_buff, 4);
    crc_data_reset();
    pc->tx_buff[17] = (crc_value >> 24) & 0xff;
    pc->tx_buff[18] = (crc_value >> 16) & 0xff;
    pc->tx_buff[19] = (crc_value >> 8)  & 0xff;
    pc->tx_buff[20] = crc_value & 0xff;
    pc->tx_buff[21] = PC_HEAD_2;
    usart_sendData(PC_USART, pc->tx_buff, 22);
}

void flash_ask_error(PC_DATA_t *pc)
{
    uint32_t crc_value = 0;
    pc->tx_buff[0] = PC_HEAD_1;
    pc->tx_buff[1] = 0x03;
    pc->tx_buff[2] = 0x03;
    pc->tx_buff[3] = LOCAL_DEVECE_ID_2;
    pc->tx_buff[4] = LOCAL_DEVECE_ID_1;
    pc->tx_buff[5] = 0x00;
    pc->tx_buff[6] = 0x0A;
    pc->tx_buff[7] = 0x18;
    pc->tx_buff[8] = 0x00;
    pc->tx_buff[9] = 0x00;
    pc->tx_buff[10] = 0x00;
    pc->tx_buff[11] = 0x00;
    pc->tx_buff[12] = 0x00;
    pc->tx_buff[13] = 0x00;
    pc->tx_buff[14] = 0x00;
    pc->tx_buff[15] = 0x00;
    pc->tx_buff[16] = 0x00;
    memset(pc->check_crc_buff, 0, PC_CRC_BUFF_SIZE);
    convert_data(pc->tx_buff, pc->check_crc_buff, 1, 16);
    crc_value = crc_block_calculate(pc->check_crc_buff, 4);
    crc_data_reset();
    pc->tx_buff[17] = (crc_value >> 24) & 0xff;
    pc->tx_buff[18] = (crc_value >> 16) & 0xff;
    pc->tx_buff[19] = (crc_value >> 8)  & 0xff;
    pc->tx_buff[20] = crc_value & 0xff;
    pc->tx_buff[21] = PC_HEAD_2;
    usart_sendData(PC_USART, pc->tx_buff, 22);
}
// ========== 版本解析 ==========
static int parse_version_string(const char *str, uint8_t *major, uint8_t *minor, uint8_t *patch)
{
    if (str == NULL || str[0] != 'V')
    {
        return -1;
    }

    const char *p = str + 1;
    *major = 0;

    while (*p >= '0' && *p <= '9')
    {
        *major = *major * 10 + (*p - '0');
        p++;
    }

    if (*p != '.')
    {
        return -1;
    }

    p++;
    *minor = 0;

    while (*p >= '0' && *p <= '9')
    {
        *minor = *minor * 10 + (*p - '0');
        p++;
    }

    if (*p != '.')
    {
        return -1;
    }

    p++;
    *patch = 0;

    while (*p >= '0' && *p <= '9')
    {
        *patch = *patch * 10 + (*p - '0');
        p++;
    }

    return 0;
}

void init_version_parse(void)
{
    parse_version_string(SOFTWARE_VERSTION, &sw_ver_major, &sw_ver_minor, &sw_ver_patch);
    parse_version_string(HARDWARE_VERSTION, &hw_ver_major, &hw_ver_minor, &hw_ver_patch);
}

#define   STORAGE_INFO       0x1FFFF7E0
#define   STORAGE_INFO_SRAM
#define   STORAGE_INFO_FLASH

#define   UNIQUE_ID          0x1FFFF7E8

#define   UNIQUE_ID_0_31     0x1FFFF7E8
#define   UNIQUE_ID_32_63    0x1FFFF7EC
#define   UNIQUE_ID_64_95    0x1FFFF7F0


struct McuInfo_t
{
    uint32_t Size;
    uint8_t UniqueId[12];
} McuInfo;

uint8_t UniqueId[12];

uint32_t GetMcuInfo(void)
{
    McuInfo.Size = *(uint32_t*)STORAGE_INFO;
    uint32_t uid = 0;

    for (uint8_t i = 0; i < 12; i++)
    {
        McuInfo.UniqueId[i] = *(uint8_t*)(UNIQUE_ID + i);
    }

    memset(pc_data.check_crc_buff, 0, PC_CRC_BUFF_SIZE);
    convert_data(McuInfo.UniqueId, pc_data.check_crc_buff, 0, 12);
    uid = crc_block_calculate(pc_data.check_crc_buff, 3);
    crc_data_reset();
    return uid;
}
















