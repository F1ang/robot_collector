//
// Created by Hacker on 2024/4/28.
//
#include "move_task.h"
#include "main.h"
#include "bsp_define.h"
#include "bsp_cmd.h"
#include "bsp_pid.h"
#include "bsp_imu.h"
#include "imu_task.h"
#include "bsp_usart.h"

#define LOG_TAG  "MOVE_TASK"
#include "bsp_log.h"

extern float speed_out;
extern float vertical_out;

extern osSemaphoreId move_semaHandle;

void move_task(void *argument)
{
    LOG_I("move_Task\r\n");
    for(;;)
    {	
		xSemaphoreTake(move_semaHandle,portMAX_DELAY);

		Wit_imu_data();

		encoder_delta[0] = Encoder_Get_Counter(1);
		encoder_delta[1] = -(Encoder_Get_Counter(2));

		speed_out = Velocity(0, encoder_delta[0],encoder_delta[1]);  
		vertical_out = Vertical(speed_out+Machine_Angle,imu_wit_data.pitch,imu_wit_data.gyro_y);			//Ö±Á¢»·
		MOTOR_SetSpeed(1, vertical_out);
		MOTOR_SetSpeed(2, vertical_out);
	
		osDelay(10);
    }
}


