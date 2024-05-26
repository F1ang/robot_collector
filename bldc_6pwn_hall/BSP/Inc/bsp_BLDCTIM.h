#ifndef __BLDC_TIM_H__
#define __BLDC_TIM_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <stm32f10x.h>
#include "bsp_motor.h"
#include "bsp_hall.h"
#include "bsp_pid.h"
/********************高级定时器TIM参数定义，只限TIM1 & TIM8************/
#define BLDC_TIMx                       TIM1
#define BLDC_TIM_APBxClock_FUN          RCC_APB2PeriphClockCmd
#define BLDC_TIM_CLK                    RCC_APB2Periph_TIM1

#define BLDC_TIM_GPIO_CLK               (RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB)
#define BLDC_TIM_CH1_PORT               GPIOA
#define BLDC_TIM_CH1_PIN                GPIO_Pin_8
#define BLDC_TIM_CH2_PORT               GPIOA
#define BLDC_TIM_CH2_PIN                GPIO_Pin_9
#define BLDC_TIM_CH3_PORT               GPIOA
#define BLDC_TIM_CH3_PIN                GPIO_Pin_10

#define BLDC_TIM_CH1N_PORT              GPIOB
#define BLDC_TIM_CH1N_PIN               GPIO_Pin_13
#define BLDC_TIM_CH2N_PORT              GPIOB
#define BLDC_TIM_CH2N_PIN               GPIO_Pin_14
#define BLDC_TIM_CH3N_PORT              GPIOB
#define BLDC_TIM_CH3N_PIN               GPIO_Pin_15

#define BLDC_TIM_BKIN_PORT              GPIOB
#define BLDC_TIM_BKIN_PIN               GPIO_Pin_12

#define BLDC_TIM_PWM_FREQ               20000  // PWM频率

// 定义定时器预分频，定时器实际时钟频率为：72MHz/（BLDC_TIMx_PRESCALER+1）
#define BLDC_TIM_PRESCALER               0  // 实际时钟频率为：72MHz

// 定义定时器周期，当定时器开始计数到BLDC_TIMx_PERIOD值并且重复计数寄存器为0时更新定时器并生成对应事件和中断
#define BLDC_TIM_PERIOD                  (uint16_t)(SystemCoreClock/(BLDC_TIM_PRESCALER+1)/BLDC_TIM_PWM_FREQ)

// 定义高级定时器重复计数寄存器值，
#define BLDC_TIM_REPETITIONCOUNTER       0


extern void BLDC_TIMx_PWM_Init(void);
extern void HALL_EXTI_Callback(void);
extern void HALL_TIMx_Callback(void);
#endif	/* __BLDC_TIM_H__ */
