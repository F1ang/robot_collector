#include "bsp_config.h"


int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);   	// NVIC3
	GPIO_Configuration();

	SVPWM_3ShuntInit();                               	// 通过三相分流电阻进行完成电流采样
#ifdef  HALL_SENSORS
	HALL_HallTimerInit();							  	// Hall传感器初始化
#elif defined NO_SPEED_SENSORS							// 无速度传感器
	STO_StateObserverInterface_Init();
    STO_Init();
#endif

	TB_Init();        									// 系统滴答定时器
	PID_Init(&PID_Torque_InitStructure, &PID_Flux_InitStructure, &PID_Speed_InitStructure); // 设定速度hSpeed_Reference

	Key_Init();
	MCL_Init_Arrays(); 									// 温度和VBUS平均值

#ifdef  HALL_SENSORS
	State = INIT;
	hSpeed_Reference = 1000;	
	MCL_Init();  // 电机控制层初始化
	do {
		ENC_Start_Up();
	}while(State == INIT);
#elif defined NO_SPEED_SENSORS
	State = INIT;
	hSpeed_Reference = 200;
	MCL_Init();
#endif	

	while(1) 
	{		
		
		Key_Process_Handler();
		switch (State)
		{
			case IDLE:    // Idle state   
			break;
		
			case INIT:
#ifdef  NO_SPEED_SENSORS
				MCL_Init();  // 电机控制层初始化
				TB_Set_StartUp_Timeout(20);
				State = START; 
#endif
			break;
			
			case START:  
			//passage to state RUN is performed by startup functions; 
			break;
			
			case RUN:             
#ifdef ENCODER
		if(ENC_ErrorOnFeedback() == TRUE)
		{
		MCL_SetFault(SPEED_FEEDBACK);
		}
#elif defined HALL_SENSORS        
		if(HALL_IsTimedOut())  // ERROR
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
		
			case STOP:   
				// shutdown power         
				/* Main PWM Output Disable */
				TIM_CtrlPWMOutputs(TIM1, DISABLE);
				State = WAIT;
				SVPWM_3ShuntAdvCurrentReading(DISABLE);
				Stat_Volt_alfa_beta.qV_Component1 = Stat_Volt_alfa_beta.qV_Component2 = 0;
				SVPWM_3ShuntCalcDutyCycles(Stat_Volt_alfa_beta);                                    
				TB_Set_Delay_500us(2000); // 1 sec delay
			break;
		
			case WAIT:    
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


