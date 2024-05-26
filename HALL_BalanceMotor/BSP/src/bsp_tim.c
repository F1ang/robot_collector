//
// Created by Hacker on 2024/5/5.
//

#include "bsp_tim.h"
#include "bsp_imu.h"
#include "imu_task.h"
#include "bsp_encoder.h"
#include "bsp_motor.h"
#include "semphr.h"
#include "bsp_usart.h"
#include "bsp_pid.h"

#define LOG_TAG  "TIM"
#include "bsp_log.h"

#define SPEED_HZ  20  // 10ms

void timx_init(void)
{
    __HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);
    HAL_TIM_Base_Start(&htim7);
}

static u8 speed_50hz = 0;
BaseType_t xHigherPriorityTaskWoken;
extern osSemaphoreId imu_semaHandle;

extern float speed_out;
extern float vertical_out;

void timx_updata_callback(TIM_HandleTypeDef *htim)
{
    __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
    //MPU6050_DMP_GetData(&robot_imu_dmp_data);
		
		Wit_imu_data();
	
    speed_50hz++;
    if (speed_50hz == SPEED_HZ) {
        speed_50hz = 0;
			 
				encoder_delta[0] = Encoder_Get_Counter(1);
        encoder_delta[1] = -(Encoder_Get_Counter(2));

			  //speed_out = Velocity(0, encoder_delta[0], encoder_delta[1]);	//速度环
				speed_out = sudu(0, encoder_delta[0],encoder_delta[1]);  
				vertical_out = Vertical(speed_out+Machine_Angle,imu_wit_data.pitch,imu_wit_data.gyro_y);			//直立环
				MOTOR_SetSpeed(1, vertical_out);
				MOTOR_SetSpeed(2, vertical_out);	
			
        xSemaphoreGiveFromISR(imu_semaHandle,&xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
    }
}

