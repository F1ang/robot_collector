/*
 * @Welcome: Hacker
 * @Author: 硬石科技
 */
#ifndef __BSP_SYSTICK_H__
#define __BSP_SYSTICK_H__

#include <stm32f10x.h>

extern void SysTick_Init(void);
extern void HAL_IncTick(void);
extern uint32_t HAL_GetTick(void);
extern void HAL_Delay(__IO uint32_t Delay);
extern void HAL_SuspendTick(void);
extern void HAL_ResumeTick(void);

#endif /* __BSP_SYSTICK_H__ */

