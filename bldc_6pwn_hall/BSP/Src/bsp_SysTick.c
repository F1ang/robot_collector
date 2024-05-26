/*
 * @Welcome: Hacker
 * @Author: 硬石科技
 */
#include "bsp_SysTick.h"

static __IO uint32_t uwTick;

/**
  * 函数功能: 初始化配置系统滴答定时器 SysTick 72M 1ms中断
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void SysTick_Init(void)
{
  uint32_t prioritygroup = 0x00;
  
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	SysTick_Config(SystemCoreClock / 1000);
  
  prioritygroup = NVIC_GetPriorityGrouping();  
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(prioritygroup, 0, 0));
}

void HAL_IncTick(void)
{
  uwTick++;
}
uint32_t HAL_GetTick(void)
{
  return uwTick;
}
void HAL_Delay(__IO uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay)
  {
  }
}
void HAL_SuspendTick(void)
{
  /* Disable SysTick Interrupt */
  CLEAR_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);
}
void HAL_ResumeTick(void)
{
  /* Enable SysTick Interrupt */
  SET_BIT(SysTick->CTRL,SysTick_CTRL_TICKINT_Msk);
}

