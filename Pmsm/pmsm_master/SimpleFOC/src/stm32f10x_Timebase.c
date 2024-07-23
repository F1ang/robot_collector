/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_Timebase.c
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This module handles time base. It used in display and 
*                      fault management, speed regulation, motor ramp-up  
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
#include "stm32f10x_MClib.h"
#include "MC_Globals.h"
#include "stm32f10x_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TB_Prescaler_5ms    31    // ((31+1)*(9374+1)/60000000) sec -> 5 ms 
#define TB_AutoReload_5ms    9374

#define TB_Prescaler_500us    29    // ((29+1)*(999+1)/60000000) sec -> 500 us 
#define TB_AutoReload_500us    999

#define SYSTICK_PRE_EMPTION_PRIORITY 3
#define SYSTICK_SUB_PRIORITY 0

#define SPEED_SAMPLING_TIME   PID_SPEED_SAMPLING_TIME

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u16 hStart_Up_TimeLeft_500us =0;
static volatile u16 hTimebase_500us = 0;
static volatile u16 hTimebase_display_500us = 0;
static volatile u16 hKey_debounce_500us = 0;
volatile u8 bPID_Speed_Sampling_Time_500us = PID_SPEED_SAMPLING_TIME;
static u16 hSpeedMeas_Timebase_500us = SPEED_SAMPLING_TIME;

#ifdef FLUX_TORQUE_PIDs_TUNING  
static u16 hTorqueSwapping = SQUARE_WAVE_PERIOD; 
#endif

/**
 * @description: 系统滴答定时器
 * @return {None}
 */
void TB_Init(void)
{   
  uint32_t prioritygroup = 0x00;
  
  
  prioritygroup = NVIC_GetPriorityGrouping();
  
  /* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	SysTick_Config(36000);
  NVIC_SetPriority (SysTick_IRQn, NVIC_EncodePriority(prioritygroup, SYSTICK_PRE_EMPTION_PRIORITY, SYSTICK_SUB_PRIORITY));
}

/*******************************************************************************
* Function Name  : TB_Wait
* Description    : The function wait for a delay to be over.   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TB_Wait(u16 time)
{
hTimebase_500us = time;    // delay = 'time' value * 0.5ms
while (hTimebase_500us != 0) // wait and do nothing!
{}  

}

/*******************************************************************************
* Function Name  : TB_Set_Delay_500us
* Description    : Set delay utilized by main.c state machine.   
* Input          : Time out value
* Output         : None
* Return         : None
*******************************************************************************/
void TB_Set_Delay_500us(u16 hDelay)
{
  hTimebase_500us = hDelay;
}  

/*******************************************************************************
* Function Name  : TB_Delay_IsElapsed
* Description    : Check if the delay set by TB_Set_Delay_500us is elapsed.   
* Input          : None
* Output         : True if delay is elapsed, false otherwise 
* Return         : None
*******************************************************************************/
bool TB_Delay_IsElapsed(void)
{
 if (hTimebase_500us == 0)
 {
   return (TRUE);
 }
 else 
 {
   return (FALSE);
 }
}  

/*******************************************************************************
* Function Name  : TB_Set_DisplayDelay_500us
* Description    : Set Delay utilized by MC_Display.c module.   
* Input          : Time out value
* Output         : None
* Return         : None
*******************************************************************************/
void TB_Set_DisplayDelay_500us(u16 hDelay)
{
  hTimebase_display_500us = hDelay;
}  

/*******************************************************************************
* Function Name  : TB_DisplayDelay_IsElapsed
* Description    : Check if the delay set by TB_Set_DisplayDelay_500us is elapsed.   
* Input          : None
* Output         : True if delay is elapsed, false otherwise 
* Return         : None
*******************************************************************************/
bool TB_DisplayDelay_IsElapsed(void)
{
 if (hTimebase_display_500us == 0)
 {
   return (TRUE);
 }
 else 
 {
   return (FALSE);
 }
} 

/*******************************************************************************
* Function Name  : TB_Set_DebounceDelay_500us
* Description    : Set Delay utilized by MC_Display.c module.   
* Input          : Time out value
* Output         : None
* Return         : None
*******************************************************************************/
void TB_Set_DebounceDelay_500us(u8 hDelay)
{
  hKey_debounce_500us = hDelay;
}  

/*******************************************************************************
* Function Name  : TB_DebounceDelay_IsElapsed
* Description    : Check if the delay set by TB_Set_DebounceDelay_500us is elapsed.   
* Input          : None
* Output         : True if delay is elapsed, false otherwise 
* Return         : None
*******************************************************************************/
bool TB_DebounceDelay_IsElapsed(void)
{
 if (hKey_debounce_500us == 0)
 {
   return (TRUE);
 }
 else 
 {
   return (FALSE);
 }
} 

/*******************************************************************************
* Function Name  : TB_Set_StartUp_Timeout(STARTUP_TIMEOUT)
* Description    : Set Start up time out and initialize Start_up torque in  
*                  torque control.   
* Input          : Time out value
* Output         : None
* Return         : None
*******************************************************************************/
void TB_Set_StartUp_Timeout(u16 hTimeout)
{
  hStart_Up_TimeLeft_500us = 2*hTimeout;  
}  

/*******************************************************************************
* Function Name  : TB_StartUp_Timeout_IsElapsed
* Description    : Set Start up time out.   
* Input          : None
* Output         : True if start up time out is elapsed, false otherwise 
* Return         : None
*******************************************************************************/
bool TB_StartUp_Timeout_IsElapsed(void)
{
 if (hStart_Up_TimeLeft_500us == 0)
 {
   return (TRUE);
 }
 else 
 {
   return (FALSE);
 }
} 


/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)   // 滴答系统计时
{ 
  if (hTimebase_500us != 0)  
  {
    hTimebase_500us --;
  }
  
  if (hTimebase_display_500us != 0)  
  {
    hTimebase_display_500us --;
  }
  
  if (hKey_debounce_500us != 0)  
  {
    hKey_debounce_500us --;
  }

  if (hStart_Up_TimeLeft_500us != 0)
  {
    hStart_Up_TimeLeft_500us--;
  }
  
  if (hSpeedMeas_Timebase_500us !=0)
  {
    hSpeedMeas_Timebase_500us--;
  }
  else
  {
    hSpeedMeas_Timebase_500us = SPEED_SAMPLING_TIME;            // 3us速度采样计算
    
    STO_Calc_Speed();      
    STO_Obs_Gains_Update();  
  }

  
  if (bPID_Speed_Sampling_Time_500us != 0 )  
  {
    bPID_Speed_Sampling_Time_500us --;
  }
  else
  { 
    bPID_Speed_Sampling_Time_500us = PID_SPEED_SAMPLING_TIME;   // 速度环频率2ms      
    if ((wGlobal_Flags & SPEED_CONTROL) == SPEED_CONTROL)       // 速度控制
    {
      if (State == RUN) 
      {
        if (HALL_GetSpeed() == HALL_MAX_SPEED)
        {
          MCL_SetFault(SPEED_FEEDBACK);
        }      
        //PID_Speed_Coefficients_update(XXX_Get_Speed(),PID_Speed_InitStructure);
        FOC_CalcFluxTorqueRef();  // 速度环      
      }
    }
    else
    {
      if (State == RUN)
      {
        FOC_TorqueCtrl();
      }
    }
  }
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
