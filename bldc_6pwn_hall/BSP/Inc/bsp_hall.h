#ifndef __BSP_HALL_H__
#define __BSP_HALL_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f10x.h>

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define HALL_RCC_CLOCKGPIO            RCC_APB2Periph_GPIOC
#define HALL_RCC_CLOCKAFIO            RCC_APB2Periph_AFIO
#define HALL_GPIO_PORTSOURCE          GPIO_PortSourceGPIOC
#define HALL_EXTI_IRQHandler          EXTI9_5_IRQHandler
#define HALL_EXTI_IRQn                EXTI9_5_IRQn
#define HALL_GPIO                     GPIOC

#define HALL_U_GPIO_PIN               GPIO_Pin_6
#define HALL_U_GPIO_PINSOURCE         GPIO_PinSource6
#define HALL_U_EXITLINE               EXTI_Line6
#define HALL_V_GPIO_PIN               GPIO_Pin_7
#define HALL_V_GPIO_PINSOURCE         GPIO_PinSource7
#define HALL_V_EXITLINE               EXTI_Line7
#define HALL_W_GPIO_PIN               GPIO_Pin_8
#define HALL_W_GPIO_PINSOURCE         GPIO_PinSource8
#define HALL_W_EXITLINE               EXTI_Line8


/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
extern void HALL_GPIO_Init(void);

extern void HALL_TIMx_Init(void);


#endif  // __BSP_HALL_H__

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
