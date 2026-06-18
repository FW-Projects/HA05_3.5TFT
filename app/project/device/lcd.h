#ifndef __LCD_H
#define __LCD_H
#include "at32f415_wk_config.h"
#include "stdint.h"
#include "stdlib.h"

typedef struct
{
	u16 width;
	u16 height;
	u16 id;
	u8 dir;
	u16 wramcmd;
	u16 setxcmd;
	u16 setycmd;
} _lcd_dev;

extern _lcd_dev lcddev;

extern u16 POINT_COLOR;
extern u16 BACK_COLOR;

// PB0~15
#define DATAOUT(x) GPIOB->odt = x;
#define DATAIN GPIOB->idt;

// #define	LCD_CS_SET  gpio_bits_set(GPIOA,GPIO_PINS_15)
// #define	LCD_RS_SET	gpio_bits_set(GPIOC,GPIO_PINS_12)
// #define	LCD_WR_SET	gpio_bits_set(GPIOC,GPIO_PINS_10)
// #define	LCD_RD_SET	gpio_bits_set(GPIOC,GPIO_PINS_11)
// #define	LCD_RES_SET	gpio_bits_set(GPIOD,GPIO_PINS_2)

// #define	LCD_CS_CLR  gpio_bits_reset(GPIOA,GPIO_PINS_15)
// #define	LCD_RS_CLR	gpio_bits_reset(GPIOC,GPIO_PINS_12)
// #define	LCD_WR_CLR	gpio_bits_reset(GPIOC,GPIO_PINS_10)
// #define	LCD_RD_CLR	gpio_bits_reset(GPIOC,GPIO_PINS_11)
// #define	LCD_RES_CLR	gpio_bits_reset(GPIOD,GPIO_PINS_2)

#define LCD_CS_CLR GPIOA->clr = 0x8000	// CS  PA15
#define LCD_RS_CLR GPIOC->clr = 0x1000	// RS  PC12
#define LCD_WR_CLR GPIOC->clr = 0x0400	// WR  PC10
#define LCD_RD_CLR GPIOC->clr = 0x0800	// RD  PC11
#define LCD_RES_CLR GPIOD->clr = 0x0004 // RES PD12

#define LCD_CS_SET GPIOA->scr = 0x8000
#define LCD_RS_SET GPIOC->scr = 0x1000
#define LCD_WR_SET GPIOC->scr = 0x0400
#define LCD_RD_SET GPIOC->scr = 0x0800
#define LCD_RES_SET GPIOD->scr = 0x0004

#define L2R_U2D 0
#define L2R_D2U 1
#define R2L_U2D 2
#define R2L_D2U 3

#define U2D_L2R 4
#define U2D_R2L 5
#define D2U_L2R 6
#define D2U_R2L 7

#define DFT_SCAN_DIR R2L_D2U

#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define YELLOW_1 0x1F
#define BROWN 0XBC40
#define BRRED 0XFC07
#define GRAY 0X8430
#define DARKBLUE 0X01CF
#define LIGHTBLUE 0X7D7C
#define GRAYBLUE 0X5458
#define LIGHTGREEN 0X841F
#define LGRAY 0XC618
#define LGRAYBLUE 0XA651
#define LBBLUE 0X2B12

#define BLOCK_SIZE 1024

#define PIC_ADDRESS_1   0x00000000
#define PIC_ADDRESS_2   0x0004B000
#define PIC_ADDRESS_3   0x0008EF80
#define PIC_ADDRESS_4   0x000D2F00
#define PIC_ADDRESS_5   0x00116E80
#define PIC_ADDRESS_6   0x0015AE00
#define PIC_ADDRESS_7   0x0019ED80
#define PIC_ADDRESS_8   0x001E2D00
#define PIC_ADDRESS_9   0x00226C80
#define PIC_ADDRESS_10  0x0026AC00
#define PIC_ADDRESS_11  0x002AEB80
#define PIC_ADDRESS_12  0x002F2B00
#define PIC_ADDRESS_13  0x00336A80
#define PIC_ADDRESS_14  0x0037AA00
#define PIC_ADDRESS_15  0x003BE980
#define PIC_ADDRESS_16  0x00402900
#define PIC_ADDRESS_17  0x00446880
#define PIC_ADDRESS_18  0x0048A800
#define PIC_ADDRESS_19  0x004CE780
#define PIC_ADDRESS_20  0x00512700
#define PIC_ADDRESS_21  0x00556680
#define PIC_ADDRESS_22  0x0059A600
#define PIC_ADDRESS_23  0x005DE580
#define PIC_ADDRESS_24  0x00622500
#define PIC_ADDRESS_25  0x00666480
#define PIC_ADDRESS_26  0x006AA400
#define PIC_ADDRESS_27  0x006EE380
#define PIC_ADDRESS_28  0x00732300
#define PIC_ADDRESS_29  0x00776280
#define PIC_ADDRESS_30  0x00790194
#define PIC_ADDRESS_31  0x007AA0A8
#define PIC_ADDRESS_32  0x007C3FBC
#define PIC_ADDRESS_33  0x007DDED0
#define PIC_ADDRESS_34  0x007F7DE4
#define PIC_ADDRESS_35  0x00811CF8
#define PIC_ADDRESS_36  0x0082EF02
#define PIC_ADDRESS_37  0x0084C10C
#define PIC_ADDRESS_38  0x00869316
#define PIC_ADDRESS_39  0x00886520
#define PIC_ADDRESS_40  0x008A372A
#define PIC_ADDRESS_41  0x008C0934
#define PIC_ADDRESS_42  0x008DDB3E
#define PIC_ADDRESS_43  0x008FAD48
#define PIC_ADDRESS_44  0x00910288
#define PIC_ADDRESS_45  0x009257C8
#define PIC_ADDRESS_46  0x0093F6DC
#define PIC_ADDRESS_47  0x009595F0
#define PIC_ADDRESS_48  0x00973504
#define PIC_ADDRESS_49  0x0098D418
#define PIC_ADDRESS_50  0x009A732C
#define PIC_ADDRESS_51  0x009C1240
#define PIC_ADDRESS_52  0x009DB154
#define PIC_ADDRESS_53  0x009F5068
#define PIC_ADDRESS_54  0x00A0EF7C
#define PIC_ADDRESS_55  0x00A28E90
#define PIC_ADDRESS_56  0x00A42DA4
#define PIC_ADDRESS_57  0x00A5CCB8
#define PIC_ADDRESS_58  0x00A76BCC
#define PIC_ADDRESS_59  0x00A90AE0
#define PIC_ADDRESS_60  0x00AAE750
#define PIC_ADDRESS_61  0x00ACC3C0
#define PIC_ADDRESS_62  0x00AD3440
#define PIC_ADDRESS_63  0x00AD37B2
#define PIC_ADDRESS_64  0x00AD3B24
#define PIC_ADDRESS_65  0x00AD9308
#define PIC_ADDRESS_66  0x00ADEAEC
#define PIC_ADDRESS_67  0x00AEA806
#define PIC_ADDRESS_68  0x00AF6520
#define PIC_ADDRESS_69  0x00B0223A
#define PIC_ADDRESS_70  0x00B0DF54
#define PIC_ADDRESS_71  0x00B19C6E
#define PIC_ADDRESS_72  0x00B25988
#define PIC_ADDRESS_73  0x00B25F70
#define PIC_ADDRESS_74  0x00B26558
#define PIC_ADDRESS_75  0x00B26A38
#define PIC_ADDRESS_76  0x00B26F18
#define PIC_ADDRESS_77  0x00B27260
#define PIC_ADDRESS_78  0x00B275A8
#define PIC_ADDRESS_79  0x00B29498
#define PIC_ADDRESS_80  0x00B2B388
#define PIC_ADDRESS_81  0x00B2D278
#define PIC_ADDRESS_82  0x00B2F168
#define PIC_ADDRESS_83  0x00B31962
#define PIC_ADDRESS_84  0x00B3415C
#define PIC_ADDRESS_85  0x00B36956
#define PIC_ADDRESS_86  0x00B39150
#define PIC_ADDRESS_87  0x00B39AE0
#define PIC_ADDRESS_88  0x00B3A470
#define PIC_ADDRESS_89  0x00B3AE00
#define PIC_ADDRESS_90  0x00B3B790
#define PIC_ADDRESS_91  0x00B3C03C
#define PIC_ADDRESS_92  0x00B3C8E8
#define PIC_ADDRESS_93  0x00B3E3D0
#define PIC_ADDRESS_94  0x00B3FEB8
#define PIC_ADDRESS_95  0x00B419A0
#define PIC_ADDRESS_96  0x00B43488
#define PIC_ADDRESS_97  0x00B44C8E
#define PIC_ADDRESS_98  0x00B46494
#define PIC_ADDRESS_99  0x00B47C9A
#define PIC_ADDRESS_100 0x00B494A0
#define PIC_ADDRESS_101 0x00B4B780
#define PIC_ADDRESS_102 0x00B4DA60
#define PIC_ADDRESS_103 0x00B4FD40
#define PIC_ADDRESS_104 0x00B52020
#define PIC_ADDRESS_105 0x00B54300
#define PIC_ADDRESS_106 0x00B565E0
#define PIC_ADDRESS_107 0x00B567A2
#define PIC_ADDRESS_108 0x00B56964
#define PIC_ADDRESS_109 0x00B57AE4
#define PIC_ADDRESS_110 0x00B58C64
#define PIC_ADDRESS_111 0x00B59DE4
#define PIC_ADDRESS_112 0x00B5AF64
#define PIC_ADDRESS_113 0x00B5B744
#define PIC_ADDRESS_114 0x00B606F4
#define PIC_ADDRESS_115 0x00B656A4
#define PIC_ADDRESS_116 0x00B65FD2
#define PIC_ADDRESS_117 0x00B663F4
#define PIC_ADDRESS_118 0x00B697E8
#define PIC_ADDRESS_119 0x00B6CBDC
#define PIC_ADDRESS_120 0x00B6DD64
#define PIC_ADDRESS_121 0x00B6EEEC
#define PIC_ADDRESS_122 0x00B6F20C
#define PIC_ADDRESS_123 0x00B6F52C
#define PIC_ADDRESS_124 0x00B74D10
#define PIC_ADDRESS_125 0x00B78104
#define PIC_ADDRESS_126 0x00BBC084
#define PIC_ADDRESS_127 0x00BC1868
#define PIC_ADDRESS_128 0x00BC88E8
#define PIC_ADDRESS_129 0x00C138E8
#define PIC_ADDRESS_130 0x00C31558











void LCD_WR_REG(u16 reg);
void LCD_WR_data(u16 data);
void LCD_WR_DATA(u16 data);
u16 LCD_RD_DATA(void);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_SetCursor(u16 x, u16 y);
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
u16 LCD_ReadPoint(u16 x, u16 y);
void LCD_Clear(u16 color);

void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color);
void LCD_DrawPoint(u16 x, u16 y, u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color);
void DrawArcRect(u16 x1, u16 y1, u16 x2, u16 y2, u16 r, u32 c);

void DrawProgressBar(u16 x1, u16 y1, u16 x2, u16 y2,
					 u16 pic_width, u16 pic_height,
					 u8 scale, u16 bar_color, u16 bc_color, u8 pic);
void DrawProgressBar_2(u16 x1, u16 y1, u16 x2, u16 y2, u8 scale, u16 bar_color, u16 bc_color);

void LCD_Show_Curve(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
					float curve_1, float curve_2, uint8_t *curve_1_arr, uint8_t *curve_2_arr);
void LCD_Show_Grid(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t unit);

void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);	   // ????????
void LCD_ShowChinese12x12(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // ???????12x12????
void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // ???????16x16????
void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // ???????24x24????
void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // ???????32x32????

void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode); // ?????????
void LCD_ShowChar1(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 size, u8 mode);
void LCD_ShowString(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode);	   // ????????
u32 mypow(u8 m, u8 n);																   // ????
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey, u8 mode); // ???????????
void LCD_ShowIntNum_nozero(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey, u8 mode);
void LCD_ShowIntNum64(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc);
void LCD_ShowIntNum128(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc);
void LCD_ShowIntNum138(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc);
void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey); // ?????¦Ë§³??????

void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[]); // ?????
void TranferPicturetoTFT_LCD(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, uint8_t Pic_Num);
void Set_Dir(u8 dir);
void LCD_Init(void);

void LCD_ShowPictureFromFlash(u16 x, u16 y, u16 length, u16 width, uint8_t Pic_Num);

void LCD_VISION(uint16_t x, uint16_t y,  uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey);
void LCD_VISION_1(uint16_t x, uint16_t y,  uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey);
#endif
