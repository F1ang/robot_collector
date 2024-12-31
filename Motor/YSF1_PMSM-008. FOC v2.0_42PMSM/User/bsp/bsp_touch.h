#ifndef __BSP_XPT2046_H__
#define	__BSP_XPT2046_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f10x.h"

/* 类型定义 ------------------------------------------------------------------*/
#pragma pack(4)     // 4字节对齐
typedef struct 
{	
  s32 cal_flag;
  s32 lcd_x[5];
	s32 lcd_y[5];
  s32 touch_x[5];
  s32 touch_y[5];
	s32 adjust[7];
}XPT2046_Calibration;
#pragma pack()


/* 宏定义 --------------------------------------------------------------------*/

/******************************* XPT2046 触摸屏中断引脚定义 ***************************/
#define XPT2046_EXTI_GPIO_CLK                        RCC_APB2Periph_GPIOE   
#define XPT2046_EXTI_GPIO_PORT                       GPIOE
#define XPT2046_EXTI_GPIO_PIN                        GPIO_Pin_2

#define XPT2046_EXTI_ActiveLevel                     0
#define XPT2046_EXTI_Read()                          GPIO_ReadInputDataBit(XPT2046_EXTI_GPIO_PORT,XPT2046_EXTI_GPIO_PIN )

/******************************* XPT2046 触摸屏模拟SPI引脚定义 ***************************/
#define XPT2046_SPI_GPIO_CLK                         RCC_APB2Periph_GPIOE

#define XPT2046_SPI_CS_PIN		                       GPIO_Pin_6
#define XPT2046_SPI_CS_PORT		                       GPIOE

#define	XPT2046_SPI_CLK_PIN	                         GPIO_Pin_3
#define XPT2046_SPI_CLK_PORT	                       GPIOE

#define	XPT2046_SPI_MOSI_PIN	                       GPIO_Pin_4
#define	XPT2046_SPI_MOSI_PORT	                       GPIOE

#define	XPT2046_SPI_MISO_PIN	                       GPIO_Pin_5
#define	XPT2046_SPI_MISO_PORT	                       GPIOE

#define XPT2046_CS_HIGT()                            GPIO_SetBits (XPT2046_SPI_CS_PORT,XPT2046_SPI_CS_PIN )    
#define XPT2046_CS_LOW()                             GPIO_ResetBits (XPT2046_SPI_CS_PORT,XPT2046_SPI_CS_PIN )  

#define XPT2046_CLK_HIGH()                           GPIO_SetBits (XPT2046_SPI_CLK_PORT,XPT2046_SPI_CLK_PIN )    
#define XPT2046_CLK_LOW()                            GPIO_ResetBits (XPT2046_SPI_CLK_PORT,XPT2046_SPI_CLK_PIN ) 

#define XPT2046_MOSI_HIGH()                          GPIO_SetBits (XPT2046_SPI_MOSI_PORT,XPT2046_SPI_MOSI_PIN ) 
#define XPT2046_MOSI_LOW()                           GPIO_ResetBits (XPT2046_SPI_MOSI_PORT,XPT2046_SPI_MOSI_PIN )

#define XPT2046_MISO()                               GPIO_ReadInputDataBit (XPT2046_SPI_MISO_PORT,XPT2046_SPI_MISO_PIN )


/* 扩展变量 ------------------------------------------------------------------*/
//extern XPT2046_Calibration cal_value;

/* 函数声明 ------------------------------------------------------------------*/
void Touch_Init_GPIO(void);
u8 XPT2046_Touch_Calibrate(void);
void XPT2046_Get_TouchedPoint(u16 *pLCD_x,u16 *pLCD_y);
void Palette_draw_point(u16 x, u16 y,u16 usColor);
#endif /* __BSP_XPT2046_H__ */

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
