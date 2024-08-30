#ifndef __BSP_LCD_H__
#define	__BSP_LCD_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f10x.h"

/* 类型定义 ------------------------------------------------------------------*/
typedef enum
{
  FONT_16=16,
  FONT_24=24,
}FONT_Typdef;
#define IS_FONT(FONT)           (((FONT) == FONT_16) || ((FONT) == FONT_24))

/* 宏定义 --------------------------------------------------------------------*/
/******************************************************************************
2^26 =0X0400 0000 = 64MB,每个 BANK 有4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

选择BANK1-BORSRAM4 连接 TFT，地址范围为0X6C00 0000 ~ 0X6FFF FFFF
YS-F1P开发板选择 FSMC_A0 接LCD的DC(寄存器/数据选择)脚
寄存器基地址 = 0X6C00 0000
RAM基地址 = 0X6C00 0002 = 0X6C00 0000+(1<<(0+1))
如果电路设计时选择不同的地址线时，地址要重新计算  
*******************************************************************************/
/******************************* ILI9488 显示屏的 FSMC 参数定义 ***************/
#define FSMC_LCD_CMD                   ((u32)0x6C000000)	    //FSMC_Bank1_NORSRAM1用于LCD命令操作的地址
#define FSMC_LCD_DATA                  ((u32)0x6C000002)      //FSMC_Bank1_NORSRAM1用于LCD数据操作的地址      
#define LCD_WRITE_CMD(x)               *(volatile u16 *)FSMC_LCD_CMD  = x 
#define LCD_WRITE_DATA(x)              *(volatile u16 *)FSMC_LCD_DATA = x
#define LCD_READ_DATA()                *(volatile u16 *)FSMC_LCD_DATA

#define FSMC_LCD_BACKx                 FSMC_Bank1_NORSRAM4

/************************* ILI9488 显示屏8080通讯引脚定义 *********************/
#define FSMC_LCD_CS_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define FSMC_LCD_CS_CLK                RCC_APB2Periph_GPIOG   
#define FSMC_LCD_CS_PORT               GPIOG
#define FSMC_LCD_CS_PIN                GPIO_Pin_12

#define FSMC_LCD_DC_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define FSMC_LCD_DC_CLK                RCC_APB2Periph_GPIOF   
#define FSMC_LCD_DC_PORT               GPIOF
#define FSMC_LCD_DC_PIN                GPIO_Pin_0

#define FSMC_LCD_BK_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define FSMC_LCD_BK_CLK                RCC_APB2Periph_GPIOF    
#define FSMC_LCD_BK_PORT               GPIOF
#define FSMC_LCD_BK_PIN                GPIO_Pin_10
 
#define LCD_BK_ON()                    GPIO_SetBits(FSMC_LCD_BK_PORT,FSMC_LCD_BK_PIN)
#define LCD_BK_OFF()                   GPIO_ResetBits(FSMC_LCD_BK_PORT,FSMC_LCD_BK_PIN)

/**************** 显示方向选择，可选(1,2,3,4)四个方向 *************************/
//#define LCD_DIRECTION                  1  // 原点在屏幕左上角 X*Y=320*480   竖屏
//#define LCD_DIRECTION                  2  // 原点在屏幕右上角 X*Y=480*320   横屏
//#define LCD_DIRECTION                  3  // 原点在屏幕右下角 X*Y=320*480   竖屏
#define LCD_DIRECTION                  4  // 原点在屏幕左下角 X*Y=480*320   横屏

/******** ILI934 显示屏全屏默认（扫描方向为1时）最大宽度和最大高度*************/
#if (LCD_DIRECTION==1)||(LCD_DIRECTION==3)

 #define LCD_DEFAULT_WIDTH		         320  // X轴长度
 #define LCD_DEFAULT_HEIGTH         	 480  // Y轴长度
 
#else

 #define LCD_DEFAULT_WIDTH		         480  // X轴长度
 #define LCD_DEFAULT_HEIGTH         	 320  // Y轴长度
 
#endif

/******************************* 定义 ILI9488 显示屏常用颜色 ********************************/
#define BLUE                          (u16)0x001F	   //蓝色 
#define GREEN                         (u16)0x07E0	   //绿色 
#define RED                           (u16)0xF800	   //红色 
#define CYAN                          (u16)0x07FF	   //蓝绿色，青色
#define MAGENTA                       (u16)0xF81F	   //红紫色，洋红色
#define YELLOW                        (u16)0xFFE0	   //黄色 
#define LIGHTBLUE                     (u16)0x841F
#define LIGHTGREEN                    (u16)0x87F0
#define LIGHTRED                      (u16)0xFC10
#define LIGHTCYAN                     (u16)0x87FF
#define LIGHTMAGENTA                  (u16)0xFC1F
#define LIGHTYELLOW                   (u16)0xFFF0
#define DARKBLUE                      (u16)0x0010
#define DARKGREEN                     (u16)0x0400
#define DARKRED                       (u16)0x8000
#define DARKCYAN                      (u16)0x0410
#define DARKMAGENTA                   (u16)0x8010
#define DARKYELLOW                    (u16)0x8400
#define WHITE                         (u16)0xFFFF	   //白色
#define LIGHTGRAY                     (u16)0xD69A
#define GRAY                          (u16)0x8410	   //灰色 
#define DARKGRAY                      (u16)0x4208
#define BLACK                         (u16)0x0000	   //黑色 
#define BROWN                         (u16)0xA145
#define ORANGE                        (u16)0xFD20
#define BACKGROUND		                 WHITE     //默认背景颜色

#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define Line0                         0
#define Line1                         27
#define Line2                         54
#define Line3                         81
#define Line4                         108
#define Line5                         135
#define Line6                         162
#define Line7                         189
#define Line8                         216
#define Line9                         243


/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
u32 BSP_LCD_Init(void);
void LCD_SetDirection(u8 ucOtion);
void LCD_OpenWindow(u16 usX,u16 usY,u16 usWidth,u16 usHeight);
void LCD_Clear(u16 usX,u16 usY,u16 usWidth,u16 usHeight,u16 usColor);
void LCD_SetPointPixel(u16 usX,u16 usY,u16 usColor);
u16 LCD_GetPointPixel(u16 usX,u16 usY);
void LCD_DrawLine(u16 usX1,u16 usY1,u16 usX2,u16 usY2,u16 usColor);
void LCD_DrawRectangle(u16 usX_Start,u16 usY_Start, u16 usWidth,u16 usHeight,u16 usColor,u8 ucFilled);
void LCD_DrawCircle(u16 usX_Center,u16 usY_Center,u16 usRadius,u16 usColor,u8 ucFilled);
void LCD_DispChar_EN(u16 usX,u16 usY,const char cChar,u16 usColor_Background,u16 usColor_Foreground,FONT_Typdef font);
void LCD_DispString_EN(u16 usX,u16 usY,const char *pstr,u16 usColor_Background,u16 usColor_Foreground,FONT_Typdef font);

void LCD_SetTextColor(volatile u16 Color);
void LCD_SetBackColor(volatile u16 Color);
void LCD_DrawRect(u16 usX_Start,u16 usY_Start, u16 usWidth,u16 usHeight);
void LCD_ClearLine(volatile u8 Line);
void LCD_DisplayChar(u8 Line, u16 Column, u8 Ascii);
void LCD_DisplayStringLine(volatile u16 Line,volatile u8 *pstr);

#endif /* __BSP_LCD_H__ */

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
