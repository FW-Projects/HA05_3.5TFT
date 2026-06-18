#include "lcd.h"
#include "stdlib.h"
#include "lcdfont.h"
#include "wk_system.h"
#include "wk_dma.h"
#include "at32_spiflash.h"
#include "math.h"
#include "HA01_handle.h"
_lcd_dev lcddev;

void set_mode(gpio_mode_type pin_mode)
{
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    if (pin_mode == GPIO_MODE_OUTPUT)
    {
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
        gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
        gpio_init_struct.gpio_pins = DB0_PIN | DB1_PIN | DB2_PIN | DB10_PIN | DB11_PIN |
                                     DB12_PIN | DB13_PIN | DB14_PIN | DB15_PIN | DB3_PIN |
                                     DB4_PIN | DB5_PIN | DB6_PIN | DB7_PIN | DB8_PIN |
                                     DB9_PIN;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIOB, &gpio_init_struct);
    }
    else
    {   
        gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
        gpio_init_struct.gpio_pins = DB0_PIN | DB1_PIN | DB2_PIN | DB10_PIN | DB11_PIN |
                                     DB12_PIN | DB13_PIN | DB14_PIN | DB15_PIN | DB3_PIN |
                                     DB4_PIN | DB5_PIN | DB6_PIN | DB7_PIN | DB8_PIN |
                                     DB9_PIN;
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        gpio_init(GPIOB, &gpio_init_struct);
    }
}

// д?????????
// regval:??????
void LCD_WR_REG(u16 reg)
{
    // #if LCD_USE8BIT_MODEL==1//???8λ??????????????
    //	LCD_RS_CLR;
    //	LCD_CS_CLR;
    //	send_data(reg);
    //	LCD_WR_CLR;
    //	LCD_WR_SET;
    //	LCD_CS_SET;
    //	LCD_RS_SET;
    // #else   //???16λ??????????????
    LCD_RS_CLR;
    LCD_CS_CLR;
    DATAOUT(reg);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_RS_SET;
    LCD_CS_SET;
    // #endif
}

// дLCD????
// data:?д????
void LCD_WR_data(u16 data)
{
    // #if LCD_USE8BIT_MODEL==1//???8λ??????????????
    //	LCD_CS_CLR;
    //	send_data(data);
    //	LCD_WR_CLR;
    //	LCD_WR_SET;
    //	LCD_CS_SET;
    // #else  //???16λ??????????????
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
    // #endif
}

// дLCD????
// data:?д????
void LCD_WR_DATA(u16 data)
{
    // #if LCD_USE8BIT_MODEL==1//???8λ??????????????
    //	LCD_CS_CLR;
    //	send_data(data>>8);
    //	LCD_WR_CLR;
    //	LCD_WR_SET;
    //	send_data(data);
    //	LCD_WR_CLR;
    //	LCD_WR_SET;
    //	LCD_CS_SET;
    // #else   //???16λ??????????????
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
    // #endif
}

// DMA1_GL1_FLAG
// DMA1_FDT1_FLAG
// DMA1_HDT1_FLAG

// ??LCD????
// ?????:???????
u16 LCD_RD_DATA(void)
{
    // u16 data=0,data0=0;
    u16 data = 0;
    set_mode(GPIO_MODE_INPUT);
#if LCD_USE8BIT_MODEL == 1 // ???8λ??????????????
    LCD_CS_CLR;
    LCD_RD_CLR;
    data = DATAIN;
    LCD_RD_SET;
    LCD_RD_CLR;
    data0 = DATAIN;
    LCD_RD_SET;
    LCD_CS_SET;
    data &= 0xff;
    data = data0 | data << 8;
#else // ???16λ??????????????
    LCD_CS_CLR;
    LCD_RD_CLR;
    data = DATAIN;
    LCD_RD_SET;
    LCD_CS_SET;
#endif
    set_mode(GPIO_MODE_OUTPUT);
    return data;
}

/******************************************************************************
      ????????????????????????
      ????????x1,x2 ?????е????????????
                y1,y2 ?????е????????????
      ???????  ??
******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_WR_REG(0x2a); // ?е??????
    LCD_WR_data(x1 >> 8);
    LCD_WR_data(x1 & 0xff);
    LCD_WR_data(x2 >> 8);
    LCD_WR_data(x2 & 0xff);
    LCD_WR_REG(0x2b); // ?е??????
    LCD_WR_data(y1 >> 8);
    LCD_WR_data(y1 & 0xff);
    LCD_WR_data(y2 >> 8);
    LCD_WR_data(y2 & 0xff);
    LCD_WR_REG(0x2c); // ??????д
}

/******************************************************************************
      ????????????ù??λ??
      ????????x,y ???λ??
      ???????  ??
******************************************************************************/
void LCD_SetCursor(u16 x, u16 y)
{
    LCD_WR_REG(0x2a); // ?е??????
    LCD_WR_data(x >> 8);
    LCD_WR_data(x & 0xff);
    LCD_WR_REG(0x2b); // ?е??????
    LCD_WR_data(y >> 8);
    LCD_WR_data(y & 0xff);
}

// LCD???????
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0X29); // ???????
}

// LCD??????
void LCD_DisplayOff(void)
{
    LCD_WR_REG(0X28); // ??????
}

// ?????????????
// x,y:????
// ?????:???????
// u16 LCD_ReadPoint(u16 x,u16 y)
//{
//  	u16 rgb;
//	LCD_SetCursor(x,y);
//	LCD_WR_REG(0X2E);
// #if LCD_USE8BIT_MODEL==1//???8λ??????????????
//	LCD_DATA_IN();
//	LCD_RS_SET;
//	LCD_CS_CLR;
//	LCD_RD_CLR;
//	rgb=DATAIN;
//	LCD_RD_SET;
//	LCD_RD_CLR;
// #else //???16λ??????????????
//	rgb=LCD_RD_DATA();//dummy read
// #endif
//	rgb=LCD_RD_DATA();//??????????
//	return rgb;
// }

void LCD_Clear(u16 color)
{
    u16 i, j;
    LCD_Address_Set(0, 0, lcddev.width - 1, lcddev.height - 1); // ?????????Χ

    for (i = 0; i < lcddev.width; i++)
    {
        for (j = 0; j < lcddev.height; j++)
        {
            LCD_WR_DATA(color);
            // LCD_WR_DATA_DMA(color);
        }
    }
}

/******************************************************************************
      ????????????????????????
      ????????xsta,ysta   ???????
                xend,yend   ???????
                                color       ????????
      ???????  ??
******************************************************************************/
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{
    u16 i, j;
    LCD_Address_Set(xsta, ysta, xend - 1, yend - 1); // ?????????Χ

    for (i = ysta; i < yend; i++)
    {
        for (j = xsta; j < xend; j++)
        {
            LCD_WR_DATA(color);
        }
    }
}

/******************************************************************************
      ??????????????λ?????
      ????????x,y ????????
                color ??????
      ???????  ??
******************************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    LCD_Address_Set(x, y, x, y); // ???ù??λ??
    LCD_WR_DATA(color);
}

/******************************************************************************
      ?????????????
      ????????x1,y1   ???????
                x2,y2   ???????
                color   ??????
      ???????  ??
******************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; // ????????????
    delta_y = y2 - y1;
    uRow = x1; // ???????????
    uCol = y1;

    if (delta_x > 0)
    {
        incx = 1; // ???????????
    }
    else if (delta_x == 0)
    {
        incx = 0; // ?????
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0; // ????
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
    {
        distance = delta_x; // ????????????????
    }
    else
    {
        distance = delta_y;
    }

    for (t = 0; t < distance + 1; t++)
    {
        LCD_DrawPoint(uRow, uCol, color); // ????
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/******************************************************************************
      ???????????????
      ????????x1,y1   ???????
                x2,y2   ???????
                color   ???ε????
      ???????  ??
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x1, y1, x1, y2, color);
    LCD_DrawLine(x1, y2, x2, y2, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
}

/******************************************************************************
      ????????????
      ????????x0,y0   ???????
                r       ??
                color   ??????
      ???????  ??
******************************************************************************/
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color)
{
    int a, b;
    a = 0;
    b = r;

    while (a <= b)
    {
        LCD_DrawPoint(x0 - b, y0 - a, color); // 3
        LCD_DrawPoint(x0 + b, y0 - a, color); // 0
        LCD_DrawPoint(x0 - a, y0 + b, color); // 1
        LCD_DrawPoint(x0 - a, y0 - b, color); // 2
        LCD_DrawPoint(x0 + b, y0 + a, color); // 4
        LCD_DrawPoint(x0 + a, y0 - b, color); // 5
        LCD_DrawPoint(x0 + a, y0 + b, color); // 6
        LCD_DrawPoint(x0 - b, y0 + a, color); // 7
        a++;

        if ((a * a + b * b) > (r * r)) // ?ж????????????
        {
            b--;
        }
    }
}

/******************************************************************************
      ?????????????????
      ????????x,y???????
                *s ??????????
                fc ??????
                bc ???????
                sizey ??? ??? 16 24 32
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    while (*s != 0)
    {
        if (sizey == 12)
        {
            LCD_ShowChinese12x12(x, y, s, fc, bc, sizey, mode);
        }
        else if (sizey == 16)
        {
            LCD_ShowChinese16x16(x, y, s, fc, bc, sizey, mode);
        }
        else if (sizey == 24)
        {
            LCD_ShowChinese24x24(x, y, s, fc, bc, sizey, mode);
        }
        else if (sizey == 32)
        {
            LCD_ShowChinese32x32(x, y, s, fc, bc, sizey, mode);
        }
        else
        {
            return;
        }

        s += 2;
        x += sizey;
    }
}

/******************************************************************************
      ????????????????12x12????
      ????????x,y???????
                *s ?????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowChinese12x12(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 i, j, m = 0;
    u16 k;
    u16 HZnum;       // ???????
    u16 TypefaceNum; // ?????????????С
    u16 x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont12) / sizeof(typFNT_GB12); // ?????????

    for (k = 0; k < HZnum; k++)
    {
        if ((tfont12[k].Index[0] == *(s)) && (tfont12[k].Index[1] == *(s + 1)))
        {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);

            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // ???????
                    {
                        if (tfont12[k].Msk[i] & (0x01 << j))
                        {
                            LCD_WR_DATA(fc);
                        }
                        else
                        {
                            LCD_WR_DATA(bc);
                        }

                        m++;

                        if (m % sizey == 0)
                        {
                            m = 0;
                            break;
                        }
                    }
                    else // ??????
                    {
                        if (tfont12[k].Msk[i] & (0x01 << j))
                        {
                            LCD_DrawPoint(x, y, fc); // ???????
                        }

                        x++;

                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }

        continue; // ??????????????????????????????????????????????
    }
}

/******************************************************************************
      ????????????????16x16????
      ????????x,y???????
                *s ?????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 i, j, m = 0;
    u16 k;
    u16 HZnum;       // ???????
    u16 TypefaceNum; // ?????????????С
    u16 x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont16) / sizeof(typFNT_GB16); // ?????????

    for (k = 0; k < HZnum; k++)
    {
        if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)))
        {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);

            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // ???????
                    {
                        if (tfont16[k].Msk[i] & (0x01 << j))
                        {
                            LCD_WR_DATA(fc);
                        }
                        else
                        {
                            LCD_WR_DATA(bc);
                        }

                        m++;

                        if (m % sizey == 0)
                        {
                            m = 0;
                            break;
                        }
                    }
                    else // ??????
                    {
                        if (tfont16[k].Msk[i] & (0x01 << j))
                        {
                            LCD_DrawPoint(x, y, fc); // ???????
                        }

                        x++;

                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }

        continue; // ??????????????????????????????????????????????
    }
}

/******************************************************************************
      ????????????????24x24????
      ????????x,y???????
                *s ?????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 i, j, m = 0;
    u16 k;
    u16 HZnum;       // ???????
    u16 TypefaceNum; // ?????????????С
    u16 x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont24) / sizeof(typFNT_GB24); // ?????????

    for (k = 0; k < HZnum; k++)
    {
        if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)))
        {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);

            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // ???????
                    {
                        if (tfont24[k].Msk[i] & (0x01 << j))
                        {
                            LCD_WR_DATA(fc);
                        }
                        else
                        {
                            LCD_WR_DATA(bc);
                        }

                        m++;

                        if (m % sizey == 0)
                        {
                            m = 0;
                            break;
                        }
                    }
                    else // ??????
                    {
                        if (tfont24[k].Msk[i] & (0x01 << j))
                        {
                            LCD_DrawPoint(x, y, fc); // ???????
                        }

                        x++;

                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }

        continue; // ??????????????????????????????????????????????
    }
}

/******************************************************************************
      ????????????????32x32????
      ????????x,y???????
                *s ?????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 i, j, m = 0;
    u16 k;
    u16 HZnum;       // ???????
    u16 TypefaceNum; // ?????????????С
    u16 x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont32) / sizeof(typFNT_GB32); // ?????????

    for (k = 0; k < HZnum; k++)
    {
        if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)))
        {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);

            for (i = 0; i < TypefaceNum; i++)
            {
                for (j = 0; j < 8; j++)
                {
                    if (!mode) // ???????
                    {
                        if (tfont32[k].Msk[i] & (0x01 << j))
                        {
                            LCD_WR_DATA(fc);
                        }
                        else
                        {
                            LCD_WR_DATA(bc);
                        }

                        m++;

                        if (m % sizey == 0)
                        {
                            m = 0;
                            break;
                        }
                    }
                    else // ??????
                    {
                        if (tfont32[k].Msk[i] & (0x01 << j))
                        {
                            LCD_DrawPoint(x, y, fc); // ???????
                        }

                        x++;

                        if ((x - x0) == sizey)
                        {
                            x = x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }

        continue; // ??????????????????????????????????????????????
    }
}

/******************************************************************************
      ???????????????????
      ????????x,y???????
                num ?????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u16 temp, sizex, t, m = 0;
    u16 i, TypefaceNum; // ?????????????С
    u16 x0 = x;
    sizex = sizey / 2;
    TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
    num = num - ' ';                                     // ?????????
    LCD_Address_Set(x, y, x + sizex - 1, y + sizey - 1); // ???ù??λ??

    for (i = 0; i < TypefaceNum; i++)
    {
        if (sizey == 12)
        {
            temp = ascii_1206[num][i]; // ????6x12????
        }
        else if (sizey == 16)
        {
            temp = ascii_1608[num][i]; // ????8x16????
        }
        else if (sizey == 24)
        {
            temp = ascii_2412[num][i]; // ????12x24????
        }
        else if (sizey == 32)
        {
            temp = ascii_3216[num][i]; // ????16x32????
        }

        else if (sizey == 64)
        {
            temp = ascii_6432[num][i]; // ????16x32????
        }
        else if (sizey == 80)
        {
            temp = ascii_8040[num][i]; // ????16x32????
        }

        else
        {
            return;
        }

        for (t = 0; t < 8; t++)
        {
            if (!mode) // ???????
            {
                if (temp & (0x01 << t))
                {
                    LCD_WR_DATA(fc);
                }
                else
                {
                    LCD_WR_DATA(bc);
                }

                m++;

                if (m % sizex == 0)
                {
                    m = 0;
                    break;
                }
            }
            else // ??????
            {
                if (temp & (0x01 << t))
                {
                    LCD_DrawPoint(x, y, fc); // ???????
                }

                x++;

                if ((x - x0) == sizex)
                {
                    x = x0;
                    y++;
                    break;
                }
            }
        }
    }
}

void LCD_ShowChar1(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 size, u8 mode)
{
    u16 temp, t1 /*t*/;
    u16 t;
    u16 y0 = y;

    for (t = 0; t < 256; t++) // ???256??????
    {
        if (size == 64)
        {
            temp = GBK6464[num][t];
        }
        else
        {
            return; // ??е????
        }

        for (t1 = 0; t1 < 8; t1++) // yuan 8
        {
            if (temp & 0x80)
            {
                LCD_DrawPoint(x, y, fc);
            }
            else if (mode == 0)
            {
                LCD_DrawPoint(x, y, bc);
            }

            temp <<= 1;
            y++;

            if (y >= lcddev.height)
            {
                return; // ????????
            }

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= lcddev.width)
                {
                    return; // ????????
                }

                break;
            }
        }
    }
}

#if 1
void LCD_ShowChar2(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 size, u8 mode)
{
    u16 temp, t1 /*t*/;
    u16 t;
    u16 y0 = y;

    for (t = 0; t < 1024; t++) // ???256??????
    {
        if (size == 128)
        {
            temp = GBK128128[num][t];
        }
        else
        {
            return; // ??е????
        }

        for (t1 = 0; t1 < 8; t1++) // yuan 8
        {
            if (temp & 0x80)
            {
                LCD_DrawPoint(x, y, fc);
            }
            else if (mode == 0)
            {
                LCD_DrawPoint(x, y, bc);
            }

            temp <<= 1;
            y++;

            if (y >= lcddev.height)
            {
                return; // ????????
            }

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= lcddev.width)
                {
                    return; // ????????
                }

                break;
            }
        }
    }
}

#endif

/******************************************************************************
      ?????????????????
      ????????x,y???????
                *p ???????????
                fc ??????
                bc ???????
                sizey ???
                mode:  0???????  1??????
      ???????  ??
******************************************************************************/
void LCD_ShowString(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    while (*p != '\0')
    {
        LCD_ShowChar(x, y, *p, fc, bc, sizey, mode);
        x += sizey / 2;
        p++;
    }
}

/******************************************************************************
      ????????????????
      ????????m??????n???
      ???????  ??
******************************************************************************/
u32 mypow(u8 m, u8 n)
{
    u32 result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/******************************************************************************
      ????????????????????
      ????????x,y???????
                num ????????????
                len ??????λ??
                fc ??????
                bc ???????
                sizey ???
      ???????  ??
******************************************************************************/
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;
    u8 sizex = sizey / 2; 

    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + t * sizex, y, '0', fc, bc, sizey, mode);
                continue;
            }
            else
            {
                enshow = 1;
            }
        }

        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, mode);
    }
}


void LCD_ShowIntNum_nozero(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;
    u8 sizex = sizey / 2; 

    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + t * sizex, y, ' ', fc, bc, sizey, mode);
                continue;
            }
            else
            {
                enshow = 1;
            }
        }

        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, mode);
    }
}

void LCD_ShowIntNum64(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc)
{
    u8 t, temp;
    u8 sizey = 64;
    u8 enshow = 0;
    u8 sizex = (sizey / 2) + 5;

    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar1(x + t * sizex, y, 0, fc, bc, sizey, 0);
                continue;
            }
            else
            {
                enshow = 1;
            }
        }

        LCD_ShowChar1(x + t * sizex, y, temp, fc, bc, sizey, 0);
    }
}

#if 1
void LCD_ShowIntNum128(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc)   
{
    u8 t, temp;
    u8 sizey = 128;
    u8 enshow = 0;
    u8 sizex = (sizey / 2) + 10;

    for (t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar2(x + t * sizex, y, 0, fc, bc, sizey, 0);
                continue;
            }
            else
            {
                enshow = 1;
            }
        }

        LCD_ShowChar2(x + t * sizex, y, temp, fc, bc, sizey, 0);
    }
}

#endif

/******************************************************************************
      ??????????????λС??????
      ????????x,y???????
                num ????С??????
                len ??????λ??
                fc ??????
                bc ???????
                sizey ???
      ???????  ??
******************************************************************************/
void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey)
{
    u8 t, temp, sizex;
    u16 num1;
    sizex = sizey / 2;
    num1 = num * 100;

    for (t = 0; t < len; t++)
    {
        temp = (num1 / mypow(10, len - t - 1)) % 10;

        if (t == (len - 2))
        {
            LCD_ShowChar(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0);
            t++;
            len += 1;
        }

        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}

/******************************************************************************
      ??????????????
      ????????x,y???????
                length ??????
                width  ??????
                pic[]  ??????
      ???????  ??
******************************************************************************/
void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[])
{
    u8 picH, picL;
    u16 i, j;
    u32 k = 0;
    LCD_Address_Set(x, y, x + length - 1, y + width - 1);

    for (i = 0; i < length; i++)
    {
        for (j = 0; j < width; j++)
        {
            picH = pic[k * 2];
            picL = pic[k * 2 + 1];
            LCD_WR_DATA(picH << 8 | picL);
            k++;
        }
    }
}

/******************************************************************************/
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue); // ????
}

void Set_Dir(u8 dir)
{
    if ((dir >> 4) % 4)
    {
        lcddev.width = 480;
        lcddev.height = 320;
    }
    else
    {
        lcddev.width = 320;
        lcddev.height = 480;
    }
}

void LCD_Scan_Dir(u8 dir)
{
    u16 regval = 0;
    u16 dirreg = 0;

    if ((lcddev.dir == 1 && lcddev.id != 0X6804 && lcddev.id != 0X1963) || (lcddev.dir == 0 && lcddev.id == 0X1963)) // ?????????6804??1963???????跽???????1963?????
    {
        switch (dir)
        {
        case 0:
            dir = 6;
            break;

        case 1:
            dir = 7;
            break;

        case 2:
            dir = 4;
            break;

        case 3:
            dir = 5;
            break;

        case 4:
            dir = 1;
            break;

        case 5:
            dir = 0;
            break;

        case 6:
            dir = 3;
            break;

        case 7:
            dir = 2;
            break;
        }
    }

    switch (dir)
    {
    case L2R_U2D:
        regval |= (0 << 7) | (0 << 6) | (0 << 5);
        break;

    case L2R_D2U:
        regval |= (1 << 7) | (0 << 6) | (0 << 5);
        break;

    case R2L_U2D:
        regval |= (0 << 7) | (1 << 6) | (0 << 5);
        break;

    case R2L_D2U:
        regval |= (1 << 7) | (1 << 6) | (0 << 5);
        break;

    case U2D_L2R:
        regval |= (0 << 7) | (0 << 6) | (1 << 5);
        break;

    case U2D_R2L:
        regval |= (0 << 7) | (1 << 6) | (1 << 5);
        break;

    case D2U_L2R:
        regval |= (1 << 7) | (0 << 6) | (1 << 5);
        break;

    case D2U_R2L:
        regval |= (1 << 7) | (1 << 6) | (1 << 5);
        break;
    }

    dirreg = 0X36;
    regval |= 0X08;
    LCD_WriteReg(dirreg, regval);
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.width - 1) >> 8);
    LCD_WR_DATA((lcddev.width - 1) & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(0);
    LCD_WR_DATA(0);
    LCD_WR_DATA((lcddev.height - 1) >> 8);
    LCD_WR_DATA((lcddev.height - 1) & 0XFF);
}

/******************************************************************************/
void LCD_Display_Dir(u8 dir)
{
    if (dir == 0)
    {
        lcddev.dir = 0;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
        lcddev.width = 320;
        lcddev.height = 480;
    }
    else
    {
        lcddev.dir = 1;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
        lcddev.width = 480;
        lcddev.height = 320;
    }

    LCD_Scan_Dir(DFT_SCAN_DIR);
}

void LCD_Init(void)
{

    LCD_RES_CLR;
    wk_delay_ms(120);
    LCD_RES_SET;
    wk_delay_ms(240);
    gpio_bits_set(GPIOB, GPIO_PINS_ALL);
    LCD_WR_REG(0XD3);
    lcddev.id = LCD_RD_DATA();
    lcddev.id = LCD_RD_DATA();
    lcddev.id = LCD_RD_DATA();
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA();

    LCD_WR_REG(0xC0);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0xC1);
    LCD_WR_DATA(0x0F);

    LCD_WR_REG(0xC5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x57);
    LCD_WR_DATA(0x80);

    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0xA0);

    LCD_WR_REG(0xB4);
    LCD_WR_DATA(0x02);

    LCD_WR_REG(0x36);
    LCD_WR_DATA(0x48);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);

    LCD_WR_REG(0x21);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE9);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0XF7);
    LCD_WR_DATA(0xA9);
    LCD_WR_DATA(0x51);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x82);

    LCD_WR_REG(0xE0); // P-Gamma
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x46);
    LCD_WR_DATA(0x47);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x2D);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x0F);

    LCD_WR_REG(0XE1); // N-Gamma
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x13);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x3A);
    LCD_WR_DATA(0x46);
    LCD_WR_DATA(0x51);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x0F);

    LCD_WR_REG(0xB6);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x22);
    LCD_WR_DATA(0x3B);

    LCD_Display_Dir(1);
    LCD_WR_REG(0x2C);
    wk_delay_ms(120);
    /* Sleep Out11h */
    LCD_WR_REG(0x11);
    wk_delay_ms(200);
    LCD_WR_REG(0x29);  
}


void TranferPicturetoTFT_LCD(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, uint8_t Pic_Num)
{
    uint32_t uiDataLength = 0;
    uint32_t uiPic_Addr = NULL;
    uint16_t usPic_Data;
    static u8 Read_data[BLOCK_SIZE] = {0};
    uiDataLength = (width) * (height) * 2;
	
	if(sFWHA01_t.language_state == CHINESE)
	{
		switch (Pic_Num)
		{
		case 1:
			uiPic_Addr = PIC_ADDRESS_1;
			break;
		case 2:
			uiPic_Addr = PIC_ADDRESS_2;
			break;
		case 3:
			uiPic_Addr = PIC_ADDRESS_3;
			break;
		case 4:
			uiPic_Addr = PIC_ADDRESS_4;
			break;
		case 5:
			uiPic_Addr = PIC_ADDRESS_5;
			break;
		case 7:
			uiPic_Addr = PIC_ADDRESS_7;
			break;
		case 9:
			uiPic_Addr = PIC_ADDRESS_9;
			break;
		case 11:
			uiPic_Addr = PIC_ADDRESS_11;
			break;
		case 13:
			uiPic_Addr = PIC_ADDRESS_13;
			break;
		case 15:
			uiPic_Addr = PIC_ADDRESS_15;
			break;
		case 17:
			uiPic_Addr = PIC_ADDRESS_17;
			break;
		case 19:
			uiPic_Addr = PIC_ADDRESS_19;
			break;
		case 21:
			uiPic_Addr = PIC_ADDRESS_21;
			break;
		case 23:
			uiPic_Addr = PIC_ADDRESS_23;
			break;
		case 25:
			uiPic_Addr = PIC_ADDRESS_25;
			break;
		case 27:
			uiPic_Addr = PIC_ADDRESS_27;
			break;
		case 29:
			uiPic_Addr = PIC_ADDRESS_29;
			break;
		case 31:
			uiPic_Addr = PIC_ADDRESS_31;
			break;
		case 33:
			uiPic_Addr = PIC_ADDRESS_33;
			break;
		case 35:
			uiPic_Addr = PIC_ADDRESS_35;
			break;
		case 37:
			uiPic_Addr = PIC_ADDRESS_37;
			break;
		case 39:
			uiPic_Addr = PIC_ADDRESS_39;
			break;
		case 41:
			uiPic_Addr = PIC_ADDRESS_41;
			break;
		case 43:
			uiPic_Addr = PIC_ADDRESS_43;
			break;
		case 44:
			uiPic_Addr = PIC_ADDRESS_44;
			break;
		case 45:
			uiPic_Addr = PIC_ADDRESS_45;
			break;
		case 47:
			uiPic_Addr = PIC_ADDRESS_47;
			break;
		case 49:
			uiPic_Addr = PIC_ADDRESS_49;
			break;
		case 51:
			uiPic_Addr = PIC_ADDRESS_51;
			break;
		case 53:
			uiPic_Addr = PIC_ADDRESS_53;
			break;
		case 55:
			uiPic_Addr = PIC_ADDRESS_55;
			break;
		case 57:
			uiPic_Addr = PIC_ADDRESS_57;
			break;
		case 59:
			uiPic_Addr = PIC_ADDRESS_59;
			break;
		case 61:
			uiPic_Addr = PIC_ADDRESS_61;
			break;
		case 62:
			uiPic_Addr = PIC_ADDRESS_62;
			break;
		case 63:
			uiPic_Addr = PIC_ADDRESS_63;
			break;
		case 64:
			uiPic_Addr = PIC_ADDRESS_64;
			break;
		case 66:
			uiPic_Addr = PIC_ADDRESS_66;
			break;
		case 68:
			uiPic_Addr = PIC_ADDRESS_68;
			break;
		case 70:
			uiPic_Addr = PIC_ADDRESS_70;
			break;
		case 72:
			uiPic_Addr = PIC_ADDRESS_72;
			break;
		case 73:
			uiPic_Addr = PIC_ADDRESS_73;
			break;
		case 74:
			uiPic_Addr = PIC_ADDRESS_74;
			break;
		case 75:
			uiPic_Addr = PIC_ADDRESS_75;
			break;
		case 76:
			uiPic_Addr = PIC_ADDRESS_76;
			break;
		case 77:
			uiPic_Addr = PIC_ADDRESS_77;
			break;
		case 78:
			uiPic_Addr = PIC_ADDRESS_78;
			break;
		case 81:
			uiPic_Addr = PIC_ADDRESS_81;
			break;
		case 82:
			uiPic_Addr = PIC_ADDRESS_82;
			break;
		case 84:
			uiPic_Addr = PIC_ADDRESS_84;
			break;
		case 86:
			uiPic_Addr = PIC_ADDRESS_86;
			break;
		case 87:
			uiPic_Addr = PIC_ADDRESS_87;
			break;
		case 88:
			uiPic_Addr = PIC_ADDRESS_88;
			break;
		case 89:
			uiPic_Addr = PIC_ADDRESS_89;
			break;
		case 90:
			uiPic_Addr = PIC_ADDRESS_90;
			break;
		case 91:
			uiPic_Addr = PIC_ADDRESS_91;
			break;
		case 92:
			uiPic_Addr = PIC_ADDRESS_92;
			break;
		case 94:
			uiPic_Addr = PIC_ADDRESS_94;
			break;
		case 96:
			uiPic_Addr = PIC_ADDRESS_96;
			break;
		case 98:
			uiPic_Addr = PIC_ADDRESS_98;
			break;
		case 100:
			uiPic_Addr = PIC_ADDRESS_100;
			break;
		case 101:
			uiPic_Addr = PIC_ADDRESS_101;
			break;
		case 102:
			uiPic_Addr = PIC_ADDRESS_102;
			break;
		case 103:
			uiPic_Addr = PIC_ADDRESS_103;
			break;
		case 104:
			uiPic_Addr = PIC_ADDRESS_104;
			break;
		case 105:
			uiPic_Addr = PIC_ADDRESS_105;
			break;
		case 106:
			uiPic_Addr = PIC_ADDRESS_106;
			break;
		case 107:
			uiPic_Addr = PIC_ADDRESS_107;
			break;
		case 108:
			uiPic_Addr = PIC_ADDRESS_108;
			break;
		case 111:
			uiPic_Addr = PIC_ADDRESS_111;
			break;
		case 112:
			uiPic_Addr = PIC_ADDRESS_112;
			break;
		case 113:
			uiPic_Addr = PIC_ADDRESS_113;
			break;
		case 115:
			uiPic_Addr = PIC_ADDRESS_115;
			break;
		case 116:
			uiPic_Addr = PIC_ADDRESS_116;
			break;
		case 117:
			uiPic_Addr = PIC_ADDRESS_117;
			break;		
		case 119:
			uiPic_Addr = PIC_ADDRESS_119;
			break;
		case 120:
			uiPic_Addr = PIC_ADDRESS_120;
			break;		
		case 121:
			uiPic_Addr = PIC_ADDRESS_121;
			break;		
		case 122:
			uiPic_Addr = PIC_ADDRESS_122;
			break;	
		case 123:
			uiPic_Addr = PIC_ADDRESS_123;
			break;	
		case 124:
			uiPic_Addr = PIC_ADDRESS_124;
			break;	
		case 127:
			uiPic_Addr = PIC_ADDRESS_127;
			break;	
		case 128:
			uiPic_Addr = PIC_ADDRESS_128;
			break;	
		case 129:
			uiPic_Addr = PIC_ADDRESS_129;
			break;	
		}
	}
	else if(sFWHA01_t.language_state == ENGLISH)
	{
		switch (Pic_Num)
		{
		case 1:
			uiPic_Addr = PIC_ADDRESS_1;
			break;
		case 2:
			uiPic_Addr = PIC_ADDRESS_125;
			break;
		case 3:
			uiPic_Addr = PIC_ADDRESS_3;
			break;
		case 4:
			uiPic_Addr = PIC_ADDRESS_4;
			break;
		case 5:
			uiPic_Addr = PIC_ADDRESS_6;
			break;
		case 7:
			uiPic_Addr = PIC_ADDRESS_8;
			break;
		case 9:
			uiPic_Addr = PIC_ADDRESS_10;
			break;
		case 11:
			uiPic_Addr = PIC_ADDRESS_12;
			break;
		case 13:
			uiPic_Addr = PIC_ADDRESS_14;
			break;
		case 15:
			uiPic_Addr = PIC_ADDRESS_16;
			break;
		case 17:
			uiPic_Addr = PIC_ADDRESS_18;
			break;
		case 19:
			uiPic_Addr = PIC_ADDRESS_20;
			break;
		case 21:
			uiPic_Addr = PIC_ADDRESS_22;
			break;
		case 23:
			uiPic_Addr = PIC_ADDRESS_24;
			break;
		case 25:
			uiPic_Addr = PIC_ADDRESS_26;
			break;
		case 27:
			uiPic_Addr = PIC_ADDRESS_28;
			break;
		case 29:
			uiPic_Addr = PIC_ADDRESS_30;
			break;
		case 31:
			uiPic_Addr = PIC_ADDRESS_32;
			break;
		case 33:
			uiPic_Addr = PIC_ADDRESS_34;
			break;
		case 35:
			uiPic_Addr = PIC_ADDRESS_36;
			break;
		case 37:
			uiPic_Addr = PIC_ADDRESS_38;
			break;
		case 39:
			uiPic_Addr = PIC_ADDRESS_40;
			break;
		case 41:
			uiPic_Addr = PIC_ADDRESS_42;
			break;
		case 43:
			uiPic_Addr = PIC_ADDRESS_43;
			break;
		case 44:
			uiPic_Addr = PIC_ADDRESS_44;
			break;
		case 45:
			uiPic_Addr = PIC_ADDRESS_46;
			break;
		case 47:
			uiPic_Addr = PIC_ADDRESS_48;
			break;
		case 49:
			uiPic_Addr = PIC_ADDRESS_50;
			break;
		case 51:
			uiPic_Addr = PIC_ADDRESS_52;
			break;
		case 53:
			uiPic_Addr = PIC_ADDRESS_54;
			break;
		case 55:
			uiPic_Addr = PIC_ADDRESS_56;
			break;
		case 57:
			uiPic_Addr = PIC_ADDRESS_58;
			break;
		case 59:
			uiPic_Addr = PIC_ADDRESS_60;
			break;
		case 61:
			uiPic_Addr = PIC_ADDRESS_61;
			break;
		case 62:
			uiPic_Addr = PIC_ADDRESS_62;
			break;
		case 63:
			uiPic_Addr = PIC_ADDRESS_63;
			break;
		case 64:
			uiPic_Addr = PIC_ADDRESS_65;
			break;
		case 66:
			uiPic_Addr = PIC_ADDRESS_67;
			break;
		case 68:
			uiPic_Addr = PIC_ADDRESS_69;
			break;
		case 70:
			uiPic_Addr = PIC_ADDRESS_71;
			break;
		case 72:
			uiPic_Addr = PIC_ADDRESS_72;
			break;
		case 73:
			uiPic_Addr = PIC_ADDRESS_73;
			break;
		case 74:
			uiPic_Addr = PIC_ADDRESS_74;
			break;
		case 75:
			uiPic_Addr = PIC_ADDRESS_75;
			break;
		case 76:
			uiPic_Addr = PIC_ADDRESS_76;
			break;
		case 77:
			uiPic_Addr = PIC_ADDRESS_77;
			break;
		case 78:
			uiPic_Addr = PIC_ADDRESS_79;
			break;
		case 81:
			uiPic_Addr = PIC_ADDRESS_80;
			break;
		case 82:
			uiPic_Addr = PIC_ADDRESS_83;
			break;
		case 84:
			uiPic_Addr = PIC_ADDRESS_85;
			break;
		case 86:
			uiPic_Addr = PIC_ADDRESS_86;
			break;
		case 87:
			uiPic_Addr = PIC_ADDRESS_87;
			break;
		case 88:
			uiPic_Addr = PIC_ADDRESS_88;
			break;
		case 89:
			uiPic_Addr = PIC_ADDRESS_89;
			break;
		case 90:
			uiPic_Addr = PIC_ADDRESS_90;
			break;
		case 91:
			uiPic_Addr = PIC_ADDRESS_91;
			break;
		case 92:
			uiPic_Addr = PIC_ADDRESS_93;
			break;
		case 94:
			uiPic_Addr = PIC_ADDRESS_95;
			break;
		case 96:
			uiPic_Addr = PIC_ADDRESS_97;
			break;
		case 98:
			uiPic_Addr = PIC_ADDRESS_99;
			break;
		case 100:
			uiPic_Addr = PIC_ADDRESS_100;
			break;
		case 101:
			uiPic_Addr = PIC_ADDRESS_101;
			break;
		case 102:
			uiPic_Addr = PIC_ADDRESS_102;
			break;
		case 103:
			uiPic_Addr = PIC_ADDRESS_103;
			break;
		case 104:
			uiPic_Addr = PIC_ADDRESS_104;
			break;
		case 105:
			uiPic_Addr = PIC_ADDRESS_105;
			break;
		case 106:
			uiPic_Addr = PIC_ADDRESS_106;
			break;
		case 107:
			uiPic_Addr = PIC_ADDRESS_107;
			break;
		case 108:
			uiPic_Addr = PIC_ADDRESS_109;
			break;
		case 111:
			uiPic_Addr = PIC_ADDRESS_110;
			break;
		case 112:
			uiPic_Addr = PIC_ADDRESS_112;
			break;
		case 113:
			uiPic_Addr = PIC_ADDRESS_114;
			break;
		case 115:
			uiPic_Addr = PIC_ADDRESS_115;
			break;
		case 116:
			uiPic_Addr = PIC_ADDRESS_116;
			break;
		case 117:
			uiPic_Addr = PIC_ADDRESS_118;
			break;		
		case 119:
			uiPic_Addr = PIC_ADDRESS_119;
			break;
		case 120:
			uiPic_Addr = PIC_ADDRESS_120;
			break;			
		case 121:
			uiPic_Addr = PIC_ADDRESS_121;
			break;		
		case 122:
			uiPic_Addr = PIC_ADDRESS_122;
			break;	
		case 123:
			uiPic_Addr = PIC_ADDRESS_123;
			break;	
		case 124:
			uiPic_Addr = PIC_ADDRESS_124;
			break;	
		case 126:
			uiPic_Addr = PIC_ADDRESS_126;
			break;	
		case 127:
			uiPic_Addr = PIC_ADDRESS_127;
			break;	
		case 128:
			uiPic_Addr = PIC_ADDRESS_128;
			break;	
		case 129:
			uiPic_Addr = PIC_ADDRESS_130;
			break;	
		}
	}
    width = width + x1;
    height = height + y1;
    LCD_Address_Set(x1, y1, width - 1, height - 1);

	while (uiDataLength)
    {
        uint32_t readSize = (uiDataLength > BLOCK_SIZE) ? BLOCK_SIZE : uiDataLength;
        spiflash_read(Read_data, uiPic_Addr, readSize);  // 从Flash读取数据

        // 将读取的数据以16位（RGB565）格式发送给LCD
        for (uint32_t i = 0; i < readSize; i += 2)
        {
            uint16_t pixel = (Read_data[i] << 8) | Read_data[i + 1];
            LCD_WR_DATA(pixel);
        }

        uiPic_Addr += readSize;
        uiDataLength -= readSize;
    }
	
//    while (uiDataLength)
//    {
//        if (BLOCK_SIZE > uiDataLength)
//        {
//            spiflash_read(Read_data, uiPic_Addr, (uiDataLength));

//            for (u32 i = 0; i < (uiDataLength); i += 2)
//            {
//                usPic_Data = Read_data[i];
//                usPic_Data <<= 8;
//                usPic_Data = usPic_Data + Read_data[i + 1];
//                LCD_WR_DATA(usPic_Data);
//            }

//            uiPic_Addr += uiDataLength;
//            uiDataLength -= uiDataLength;
//        }
//        else
//        {
//            spiflash_read(Read_data, uiPic_Addr, BLOCK_SIZE);

//            for (u32 i = 0; i < BLOCK_SIZE; i += 2)
//            {
//                usPic_Data = Read_data[i];
//                usPic_Data <<= 8;
//                usPic_Data = usPic_Data + Read_data[i + 1];
//                LCD_WR_DATA(usPic_Data);
//            }

//            uiPic_Addr += BLOCK_SIZE;
//            uiDataLength -= BLOCK_SIZE;
//        }
//    }
}

/******************************************************************************
      ??????????????????
      ????????x1,y1  ???????
                x2,y2  ???????
                scale      ???????????0-100??
                bar_color  ?????????
                bc_color   ???????????
                pic[]      ?????飨?????????????
      ???????  ??
******************************************************************************/
void DrawProgressBar(u16 x1, u16 y1, u16 x2, u16 y2,
                     u16 pic_width, u16 pic_height,
                     u8 scale, u16 bar_color, u16 bc_color, u8 pic)
{
    if (scale >= 98)
    {
        scale = 98;
    }

    if (scale <= 5)
    {
        scale = 5;
    }

    u16 Bar_Length = y2 - y1;
    u16 len = Bar_Length * (100 - scale) / 100;

    LCD_Fill(x1, y1 - pic_height / 2, x2, y1 + len - pic_height / 2, bc_color);
    TranferPicturetoTFT_LCD(x1 - 1, (y1 + len) - pic_height / 2, pic_width, pic_height, pic);
    LCD_Fill(x1, y1 + len + pic_height / 2, x2, y2, bar_color);

    LCD_Fill(x1 - 1, y1 - pic_height / 2, x1, y1 + len - pic_height / 2, bc_color);
    LCD_Fill(x1 - 1, y1 + len + pic_height / 2, x1, y2, bc_color);

    LCD_Fill(x2, y1 - pic_height / 2, x2 + 1, y1 + len - pic_height / 2, bc_color);
    LCD_Fill(x2, y1 + len + pic_height / 2, x2 + 1, y2, bc_color);
}

/******************************************************************************
      函数说明：显示进度条(竖向)
      入口数据：x1,y1  起点坐标
                x2,y2  终点坐标
                scale      进度条百分比（0-100）
                bar_color  进度条颜色
                bc_color   进度条背景色
      返回值：  无
******************************************************************************/
/******************************************************************************
      函数说明：显示进度条(横向)
      入口数据：x1,y1  起点坐标
                x2,y2  终点坐标
                scale      进度条百分比（0-100）
                bar_color  进度条颜色
                bc_color   进度条背景色
      返回值：  无
******************************************************************************/
void DrawProgressBar_2(u16 x1, u16 y1, u16 x2, u16 y2, u8 scale, u16 bar_color, u16 bc_color)
{
    if (scale >= 100)
    {
        scale = 100;
    }

    if (scale <= 1)
    {
        scale = 1;
    }

    u16 Bar_Length = x2 - x1;
    u16 len = Bar_Length * ((float)(100 - scale) / 100);

    LCD_Fill(x1, y1, x2 - len, y2, bar_color);

    LCD_Fill(x2 - len, y1, x2, y2, bc_color);
}

void Drawarc(int x, int y, int a, int b, u16 r, u32 c)
{
    float rad, x_tp, y_tp, i;
    i = a;

    for (; i < b; i = i + 0.2)
    {
        rad = 0.01745 * i;
        x_tp = r * cos(rad) + x;
        y_tp = -r * sin(rad) + y;
        LCD_DrawPoint((u16)x_tp, (u16)y_tp, c);
    }
}
void DrawArcRect(u16 x1, u16 y1, u16 x2, u16 y2, u16 r, u32 c)
{
    LCD_DrawLine(x1 + r, y1, x2 - r, y1, c);
    LCD_DrawLine(x1 + r, y1 + 1, x2 - r, y1 + 1, c);
    LCD_DrawLine(x1, y1 + r, x1, y2 - r, c);
    LCD_DrawLine(x1 + 1, y1 + r, x1 + 1, y2 - r, c);
    LCD_DrawLine(x1 + r, y2, x2 - r, y2, c);
    LCD_DrawLine(x1 + r, y2 + 1, x2 - r, y2 + 1, c);
    LCD_DrawLine(x2, y1 + r, x2, y2 - r, c);
    LCD_DrawLine(x2 + 1, y1 + r, x2 + 1, y2 - r, c);
    //         Drawarc(x1 + r, y1 + r, 90, 180, r, c);
    //         Drawarc(x2 - r, y1 + r, 0, 90, r, c);
    //         Drawarc(x1 + r, y2 - r, 180, 270, r, c);
    //         Drawarc(x2 - r, y2 - r, 270, 360, r, c);
    //         Drawarc(x1 + r, y1 + r, 90, 180, r, c);
    //         Drawarc(x2 - r, y1 + r, 0, 90, r, c);
    //         Drawarc(x1 + r, y2 - r, 180, 270, r, c);
    //         Drawarc(x2 - r, y2 - r, 270, 360, r, c);
}
// 通用电压到Y坐标映射函数
int voltage_to_y(float voltage, float v_min, float v_max, int y_top, int y_bottom)
{
    // 计算显示区域高度
    int display_height = y_bottom - y_top;

    // 确保电压在范围内
    if (voltage < v_min)
    {
        voltage = v_min;
    }

    if (voltage > v_max)
    {
        voltage = v_max;
    }

    // 计算电压在范围内的比例位置
    float position = (voltage - v_min) / (v_max - v_min);
    // 反转比例（因为屏幕Y轴向下增加）
    position = 1.0 - position;
    // 计算Y坐标（四舍五入）
    int y = y_top + (int)(position * display_height + 0.5);
    return y;
}
void LCD_Show_Grid(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t unit)
{
    uint8_t i, j;
    uint16_t x_line_space = 0;
    uint16_t y_line_space = 0;
    x_line_space = (x2 - x1) / 5;
    y_line_space = (y2 - y1) / 5;
    for (i = 0; i < 5; i++)
    {
        if (i == 0)
        {
            LCD_DrawRectangle(x1, y1, x2, y2, color);
        }
        else
        {
            LCD_DrawLine(x1, y1 + i * y_line_space, x2, y1 + i * y_line_space, color);
            LCD_DrawLine(x1 + i * x_line_space, y1, x1 + i * x_line_space, y2, color);
        }
    }
	TranferPicturetoTFT_LCD(20, 42, 47, 25, CURVE_ICON);
//        LCD_ShowString(x1 + 2, y1 + 2, (uint8_t *)"C", color, BLACK, 16, 0);
	LCD_ShowString(x1 + 3, y1 + 30, (uint8_t *)"500(932)", 0xf460, BLACK, 12, 0);
	LCD_ShowString(x1 + 3, y1 + 72, (uint8_t *)"400(752)", 0xf460, BLACK, 12, 0);
	LCD_ShowString(x1 + 3, y1 + 114, (uint8_t *)"300(572)", 0xf460, BLACK, 12, 0);
	LCD_ShowString(x1 + 3, y1 + 156, (uint8_t *)"200(392)", 0xf460, BLACK, 12, 0);
	LCD_ShowString(x1 + 3, y1 + 198, (uint8_t *)"100(212)", 0xf460, BLACK, 12, 0);

	TranferPicturetoTFT_LCD(x2-25, y1+2, 23, 23, CURVE_FAN_ICON);
    LCD_ShowString(x2 - 20, y1 + 30, (uint8_t *)"200", 0x4c38, BLACK, 12, 0);
    LCD_ShowString(x2 - 20, y1 + 72, (uint8_t *)"150", 0x4c38, BLACK, 12, 0);
    LCD_ShowString(x2 - 20, y1 + 114, (uint8_t *)"100", 0x4c38, BLACK, 12, 0);
    LCD_ShowString(x2 - 14, y1 + 156, (uint8_t *)"50", 0x4c38, BLACK, 12, 0);
    LCD_ShowString(x2 - 8, y1 + 198, (uint8_t *)"0", 0x4c38, BLACK, 12, 0);
}
void LCD_Show_Curve(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                    float curve_1, float curve_2, uint8_t *curve_1_arr, uint8_t *curve_2_arr)
{
    int i;
    volatile static uint16_t input_data_1 = 0;
    volatile static uint16_t input_data_2 = 0;

    /* get s1 temp data*/
    if (curve_1 >= 500)
    {
        curve_1 = 500;
    }

    if (curve_1 <= 100)
    {
        curve_1 = 100;
    }

    /* get s1 power data*/
    if (curve_2 >= 200)
    {
        curve_2 = 200;
    }

    if (curve_2 <= 0)
    {
        curve_2 = 0;
    }

    input_data_1 = voltage_to_y(curve_1, 100, 600, y1, y2);
    input_data_2 = voltage_to_y(curve_2, 0, 250, y1, y2);

    for (i = 0; i < (x2 - x1 - 1); i++)
    {

        LCD_DrawLine(x1 + i, curve_1_arr[i], x1 + i + 2, curve_1_arr[i + 1], BLACK);

        LCD_DrawLine(x1 + i, curve_2_arr[i], x1 + i + 2, curve_2_arr[i + 1], BLACK);

        if (i < (x2 - x1 - 2))
        {

            LCD_DrawLine(x1 + i, curve_1_arr[i + 1], x1 + i + 2, curve_1_arr[i + 2], 0xf460);

            LCD_DrawLine(x1 + i, curve_2_arr[i + 1], x1 + i + 2, curve_2_arr[i + 2], 0x4c38);
        }

        curve_1_arr[i] = curve_1_arr[i + 1];

        curve_2_arr[i] = curve_2_arr[i + 1];
    }

    curve_1_arr[(x2 - x1 - 1)] = input_data_1;

    curve_2_arr[(x2 - x1 - 1)] = input_data_2;
}


void LCD_VISION(uint16_t x, uint16_t y,  uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
{
	uint8_t t, temp;
    uint8_t enshow = 0;
    uint8_t sizex = sizey / 2;

    for (t = 0; t < len; t++)
    {
		switch(t)
		{
			case 1:
				LCD_ShowChar(x + t * sizex, y, 'V', fc, bc, sizey, 0);
				break;
			case 2:
				LCD_ShowChar(x + t * sizex, y, '1', fc, bc, sizey, 0);
				break;
			case 3:
				LCD_ShowChar(x + t * sizex, y, '.', fc, bc, sizey, 0);
				break;
			case 4:
				LCD_ShowChar(x + t * sizex, y, '0', fc, bc, sizey, 0);
				break;
			case 5:
				LCD_ShowChar(x + t * sizex, y, '.', fc, bc, sizey, 0);
				break;
			case 6:
				LCD_ShowChar(x + t * sizex, y, '7', fc, bc, sizey, 0);
				break;
		}
        
    }
}

void LCD_VISION_1(uint16_t x, uint16_t y,  uint8_t len, uint16_t fc, uint16_t bc, uint8_t sizey)
{
	uint8_t t, temp;
    uint8_t enshow = 0;
    uint8_t sizex = sizey / 2;

    for (t = 0; t < len; t++)
    {
		switch(t)
		{
			case 1:
				LCD_ShowChar(x + t * sizex, y, 'V', fc, bc, sizey, 0);
				break;
			case 2:
				LCD_ShowChar(x + t * sizex, y, '1', fc, bc, sizey, 0);
				break;
			case 3:
				LCD_ShowChar(x + t * sizex, y, '.', fc, bc, sizey, 0);
				break;
			case 4:
				LCD_ShowChar(x + t * sizex, y, '0', fc, bc, sizey, 0);
				break;
			case 5:
				LCD_ShowChar(x + t * sizex, y, '.', fc, bc, sizey, 0);
				break;
			case 6:
				LCD_ShowChar(x + t * sizex, y, '1', fc, bc, sizey, 0);
				break;
		}
        
    }
}