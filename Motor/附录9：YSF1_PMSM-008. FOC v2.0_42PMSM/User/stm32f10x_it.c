/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_MClib.h"
#include "MC_Globals.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BRAKE_HYSTERESIS (u16)((OVERVOLTAGE_THRESHOLD/16)*15)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC1_2_IRQHandler(void)
{
  //if(ADC_GetITStatus(ADC1, ADC_IT_JEOC) == SET))
  if((ADC1->SR & ADC_FLAG_JEOC) == ADC_FLAG_JEOC)
  {
    //It clear JEOC flag
    ADC1->SR = ~(u32)ADC_FLAG_JEOC;
      
    if (SVPWMEOCEvent())
    {
      #ifdef DAC_FUNCTIONALITY 
      #if (defined OBSERVER_GAIN_TUNING || defined NO_SPEED_SENSORS)
      MCDAC_Update_Value(LO_ANGLE,(s16)(STO_Get_Electrical_Angle()));
      MCDAC_Update_Value(LO_I_A, STO_Get_wIalfa_est());        
      MCDAC_Update_Value(LO_I_B, STO_Get_wIbeta_est());        
      MCDAC_Update_Value(LO_BEMF_A, STO_Get_wBemf_alfa_est()); 
      MCDAC_Update_Value(LO_BEMF_B,STO_Get_wBemf_beta_est()); 
      #endif  
      #endif
      
      MCL_Calc_BusVolt();
      switch (State)
      {
          case RUN:          
            FOC_Model();       
          break;       
    
          case START:        
          #ifdef NO_SPEED_SENSORS
           #ifdef VIEW_ENCODER_FEEDBACK
           ENC_Start_Up();
           if ( (wGlobal_Flags & FIRST_START) != FIRST_START)
           {
             STO_Start_Up();
           }           
           #else
           STO_Start_Up();
           #endif
          #elif defined ENCODER
           ENC_Start_Up();       
          #elif defined HALL_SENSORS
           State = RUN;
          #endif       
          break; 
    
          default:
          break;
      }
      #ifdef BRAKE_RESISTOR
        if((wGlobal_Flags & BRAKE_ON) == BRAKE_ON)
        {
          u16 aux;
        #ifdef THREE_SHUNT
          aux = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
        #elif defined SINGLE_SHUNT
          aux = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_2);
        #endif          
          if (aux < BRAKE_HYSTERESIS)
          {
           wGlobal_Flags &= ~BRAKE_ON;
           MCL_Set_Brake_Off();
          }
        }
      #endif    
      #ifdef DAC_FUNCTIONALITY  
        MCDAC_Update_Value(I_A,Stat_Curr_a_b.qI_Component1);
        MCDAC_Update_Value(I_B,Stat_Curr_a_b.qI_Component2);
        MCDAC_Update_Value(I_ALPHA,Stat_Curr_alfa_beta.qI_Component1);
        MCDAC_Update_Value(I_BETA,Stat_Curr_alfa_beta.qI_Component2);
        MCDAC_Update_Value(I_Q,Stat_Curr_q_d.qI_Component1);
        MCDAC_Update_Value(I_D,Stat_Curr_q_d.qI_Component2);
        MCDAC_Update_Value(I_Q_REF,hTorque_Reference);
        MCDAC_Update_Value(I_D_REF,hFlux_Reference);
        MCDAC_Update_Value(V_Q,Stat_Volt_q_d.qV_Component1);
        MCDAC_Update_Value(V_D,Stat_Volt_q_d.qV_Component2);
        MCDAC_Update_Value(V_ALPHA,Stat_Volt_alfa_beta.qV_Component1);
        MCDAC_Update_Value(V_BETA,Stat_Volt_alfa_beta.qV_Component2); 
        #if (defined ENCODER || defined VIEW_ENCODER_FEEDBACK)
        MCDAC_Update_Value(SENS_ANGLE,ENC_Get_Electrical_Angle());
        #elif (defined HALL_SENSORS)
        if (State != RUN)
        {HALL_IncElectricalAngle();}
        MCDAC_Update_Value(SENS_ANGLE,HALL_GetElectricalAngle());  
        #elif (defined VIEW_HALL_FEEDBACK)
        HALL_IncElectricalAngle();  
        MCDAC_Update_Value(SENS_ANGLE,HALL_GetElectricalAngle());
        #endif
        #if ((defined OBSERVER_GAIN_TUNING) && (!defined(NO_SPEED_SENSORS)))
        STO_Calc_Rotor_Angle(Stat_Volt_alfa_beta,Stat_Curr_alfa_beta,MCL_Get_BusVolt());    
        #endif
  
        MCDAC_Update_Output();
      #endif    
    }
  }
  else 
  {
    #ifdef THREE_SHUNT    
    if(ADC_GetITStatus(ADC1, ADC_IT_AWD) == SET)
    #elif defined SINGLE_SHUNT
    if(ADC_GetITStatus(ADC2, ADC_IT_AWD) == SET)
    #endif  
    {
#ifdef BRAKE_RESISTOR
      //Analog watchdog interrupt has been generated 
     MCL_Set_Brake_On(); 
     wGlobal_Flags |= BRAKE_ON;
#else
     MCL_SetFault(OVER_VOLTAGE);
#endif
    #ifdef THREE_SHUNT    
     ADC_ClearFlag(ADC1, ADC_FLAG_AWD);
    #elif defined SINGLE_SHUNT
     ADC_ClearFlag(ADC2, ADC_FLAG_AWD);
    #endif     
    }    
  }
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
  MCL_SetFault(OVER_CURRENT);
  TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
  // Clear Update Flag
  TIM_ClearFlag(TIM1, TIM_FLAG_Update); 

#ifndef ICS_SENSORS  
  SVPWMUpdateEvent();
#endif
}


/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifndef TIMER2_HANDLES_ENCODER
#ifndef TIMER2_HANDLES_HALL
void TIM2_IRQHandler(void)
{  
}
#endif
#endif

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifndef TIMER3_HANDLES_ENCODER
#ifndef TIMER3_HANDLES_HALL
void TIM3_IRQHandler(void)
{  
}
#endif
#endif

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#ifndef TIMER4_HANDLES_ENCODER
#ifndef TIMER4_HANDLES_HALL
void TIM4_IRQHandler(void)
{  
}
#endif
#endif

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
