#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include <stm32f10x.h>

#include "bsp_SysTick.h"

#define KEY1    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY2    GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)

void Key_Init(void);
u8 KEY_Scan(u8 mode);

#endif  

