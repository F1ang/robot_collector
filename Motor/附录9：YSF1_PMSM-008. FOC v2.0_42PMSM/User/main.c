/**
  ******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2015-10-04
  * ��    ��: ʹ�ñ�׼�ⷽ������LED������
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F1Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_MClib.h"
#include "MC_Globals.h"
#include "bsp_lcd.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the TIM1 Pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOC clock */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);

  GPIO_DeInit(GPIOA);
  GPIO_StructInit(&GPIO_InitStructure);
                  
  /* Configure PC.06, PC.07, PC.08 and PC.09 as Output push-pull for debugging
     purposes */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOA,GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11);
}

/**
  * ��������: ������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);   // NVIC3
  GPIO_Configuration();                             // PA8~PA10
  
#ifdef THREE_SHUNT  
  SVPWM_3ShuntInit();                               // ͨ������������������ɵ�������
#elif defined ICS_SENSORS
  SVPWM_IcsInit();
#elif defined SINGLE_SHUNT
  SVPWM_1ShuntInit();
#endif
  
#ifdef ENCODER
   ENC_Init();
   #ifdef OBSERVER_GAIN_TUNING
      STO_StateObserverInterface_Init();
      STO_Init();
   #endif
#elif defined HALL_SENSORS
   HALL_HallTimerInit();
   #ifdef OBSERVER_GAIN_TUNING
      STO_StateObserverInterface_Init();
      STO_Init();
   #endif
#elif defined NO_SPEED_SENSORS
    STO_StateObserverInterface_Init();
    STO_Init();
   #ifdef VIEW_ENCODER_FEEDBACK
      ENC_Init();
   #elif defined VIEW_HALL_FEEDBACK
      HALL_HallTimerInit();
   #endif
#endif

#ifdef DAC_FUNCTIONALITY   
  MCDAC_Init();
#endif

  TB_Init();        //ϵͳ�δ�ʱ��
  
  PID_Init(&PID_Torque_InitStructure, &PID_Flux_InitStructure, &PID_Speed_InitStructure);

#ifdef BRAKE_RESISTOR
    MCL_Brake_Init();
#endif
      
  KEYS_Init();
  
  /* TIM1 Counter Clock stopped when the core is halted */
//  DBGMCU_Config(DBGMCU_TIM1_STOP, ENABLE);
  
  // Init Bus voltage and Temperature average  
  MCL_Init_Arrays();
  
  BSP_LCD_Init();
  LCD_SetTextColor(Blue);
  LCD_SetBackColor(White);    
  Display_Welcome_Message(); 
  
  while(1)
  { 
    Display_LCD();
    MCL_ChkPowerStage();    
    //User interface management    
    KEYS_process();  
  
    switch (State)
    {
      case IDLE:    // Idle state   
      break;
      
      case INIT:
        MCL_Init();  // ������Ʋ��ʼ��
        TB_Set_StartUp_Timeout(3000);
        State = START; 
      break;
        
      case START:  
      //passage to state RUN is performed by startup functions; 
      break;
          
      case RUN:   // motor running       
#ifdef ENCODER
        if(ENC_ErrorOnFeedback() == TRUE)
        {
          MCL_SetFault(SPEED_FEEDBACK);
        }
#elif defined HALL_SENSORS        
        if(HALL_IsTimedOut())
        {
          MCL_SetFault(SPEED_FEEDBACK);
        } 
        if (HALL_GetSpeed() == HALL_MAX_SPEED)
        {
          MCL_SetFault(SPEED_FEEDBACK);
        } 
#elif defined NO_SPEED_SENSORS
      
#endif     
      break;  
      
      case STOP:    // motor stopped
          // shutdown power         
          /* Main PWM Output Disable */
          TIM_CtrlPWMOutputs(TIM1, DISABLE);
          
          State = WAIT;
          
#ifdef THREE_SHUNT          
          SVPWM_3ShuntAdvCurrentReading(DISABLE);
#endif   
#ifdef SINGLE_SHUNT          
          SVPWM_1ShuntAdvCurrentReading(DISABLE);
#endif
          Stat_Volt_alfa_beta.qV_Component1 = Stat_Volt_alfa_beta.qV_Component2 = 0;
          
#ifdef ICS_SENSORS
          SVPWM_IcsCalcDutyCycles(Stat_Volt_alfa_beta);
#elif defined THREE_SHUNT
          SVPWM_3ShuntCalcDutyCycles(Stat_Volt_alfa_beta);
#endif                                                
          TB_Set_Delay_500us(2000); // 1 sec delay
      break;
      
      case WAIT:    // wait state
          if (TB_Delay_IsElapsed() == TRUE) 
          {
#ifdef ENCODER            
            if(ENC_Get_Mechanical_Speed() ==0)             
            {              
              State = IDLE;              
            }
#elif defined HALL_SENSORS      
            if (HALL_IsTimedOut())
            {               
              State=IDLE;
            } 
#elif defined NO_SPEED_SENSORS
            STO_InitSpeedBuffer();
            State=IDLE; 
#endif            
          }
        break;
    
      case FAULT:                   
          if (MCL_ClearFault() == TRUE)
          {
            if((wGlobal_Flags & SPEED_CONTROL) == SPEED_CONTROL)
            {
              bMenu_index = CONTROL_MODE_MENU_1;
            }
            else
            {
              bMenu_index = CONTROL_MODE_MENU_6;
            }       
#if defined NO_SPEED_SENSORS
            STO_InitSpeedBuffer();
#endif            
            State = IDLE;
            wGlobal_Flags |= FIRST_START;
          }
        break;
    
      default:        
        break;
    }
  }
}


/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

