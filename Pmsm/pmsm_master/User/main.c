#include "bsp_config.h"


int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);   	// NVIC3
	GPIO_Configuration();

	SVPWM_3ShuntInit();                               	// 通过三相分流电阻进行完成电流采样
	HALL_HallTimerInit();							  	// Hall传感器初始化

	TB_Init();        									// 系统滴答定时器
	PID_Init(&PID_Torque_InitStructure, &PID_Flux_InitStructure, &PID_Speed_InitStructure); // 设定速度hSpeed_Reference

	// KEYS_Init();
	MCL_Init_Arrays(); 									// 温度和VBUS平均值

	State = INIT;
	hSpeed_Reference = 1000;
	
	while(1) 
	{		
		// KEYS_process(); 
		switch (State)
		{
			case IDLE:    // Idle state   
			break;
		
			case INIT:
				MCL_Init();  // 电机控制层初始化
				TB_Set_StartUp_Timeout(3000);
				State = START; 
			break;
			
			case START:  
			//passage to state RUN is performed by startup functions; 
			break;
			
			case RUN:             
				if(HALL_IsTimedOut())
				{
					MCL_SetFault(SPEED_FEEDBACK);
				} 
				if (HALL_GetSpeed() == HALL_MAX_SPEED)
				{
					MCL_SetFault(SPEED_FEEDBACK);
				} 
		
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
				if (HALL_IsTimedOut())
				{               
					State=IDLE;
				}          
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
					State = IDLE;
					wGlobal_Flags |= FIRST_START;
				}
				break;
		
			default:        
				break;
    	}	
	}	  
}


