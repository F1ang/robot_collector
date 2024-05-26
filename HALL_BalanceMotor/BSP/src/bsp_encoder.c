//
// Created by Hacker on 2024/4/28.
//

#include "bsp_encoder.h"
#include "tim.h"

#define LOG_TAG    "ENCODER"
#include "bsp_log.h"

void Encoder_Set_Counter(int8_t Motor_Num, int16_t count)
{
    switch(Motor_Num)
    {
        case 1: __HAL_TIM_SET_COUNTER(&htim2, count); break;
        case 2: __HAL_TIM_SET_COUNTER(&htim3, count); break;
        default:
        {
            LOG_E("Motor_Num ERROR\r\n");
            break;
        }
    }
}

/* 中断获取 */
short Encoder_Get_Counter(int8_t Motor_Num)
{
    short counter = 0;
    switch(Motor_Num)
    {
        case 1: 
					counter = (short)__HAL_TIM_GetCounter(&htim2);
//					Encoder_Set_Counter(1, ENCODER_MID_VALUE);			
			Encoder_Set_Counter(1, 0);					
			
				break;
        case 2: 
					counter = (short)__HAL_TIM_GetCounter(&htim3); 
//					Encoder_Set_Counter(2, ENCODER_MID_VALUE);
				Encoder_Set_Counter(2, 0);	
				
				break;
        default:
        {
            LOG_E("Motor_Num ERROR\r\n");
            counter = 0;
            break;
        }
    }
    return counter;
}

uint16_t Encoder_Get_Dir(int8_t Motor_Num)
{
    uint16_t direction = 1;
    switch(Motor_Num)
    {
        case 1: direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2); break;
        case 2: direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3); break;
        default:
        {
            LOG_E("Motor_Num ERROR\r\n");
            direction = 1;
            break;
        }
    }
    return direction;
}

void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

    Encoder_Set_Counter(1, ENCODER_MID_VALUE);
    Encoder_Set_Counter(2, ENCODER_MID_VALUE);
	
//		Encoder_Set_Counter(1, 0);
//    Encoder_Set_Counter(2, 0);
	
    LOG_I("Encoder Init Success\r\n");
}

