#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include <stm32f10x.h>

#include "bsp_config.h"

#define KEY1    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY2    GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)

typedef struct {
    u8 key_flag;            // 0:no key, 1:key pressed 2:key pressed
    u8 key_stop_up_down;    // 0:stop, 1:up, 2:down 3:mode
} Key_TypeDef;

extern volatile u16 key_delay;
extern Key_TypeDef key_sta;

void Key_Init(void);
u8 KEY_Scan(u8 mode);
extern void Key_Process_Handler(void);

#endif  

