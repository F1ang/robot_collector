//
// Created by Hacker on 2024/4/28.
//

#include "imu_task.h"
#include "main.h"
#include "bsp_define.h"
#include "bsp_cmd.h"
#include "bsp_imu.h"
#include "bsp_motor.h"
#include "bsp_encoder.h"
#include "bsp_pid.h"
#include "math.h"
#include "semphr.h"
#include "bsp_tim.h"
#include "bsp_usart.h"

#define LOG_TAG  "IMU_TASK"
#include "bsp_log.h"

struct imu_data robot_imu_dmp_data;
extern osSemaphoreId imu_semaHandle;

float Machine_Angle = 3.01;	//机械角度中值，自平衡的角度值
int32_t encoder_inc[2];	//编码器累加值
short encoder_delta[2];	//编码器变化值

float speed_out = 0;
float vertical_out = 0;

void imu_task(void *argument)
{
    LOG_I("imu_Task\r\n");
    MPU6050_Init();
    timx_init();
    for(;;)
    {
       //MPU6050_DMP_GetData(&robot_imu_dmp_data);  /* 200hz = 5ms */
       xSemaphoreTake(imu_semaHandle,portMAX_DELAY);  /* 50hz计算speed */

       //计算编码器变化值
//       encoder_delta[0] = Encoder_Get_Counter(1) - ENCODER_MID_VALUE;
//       encoder_delta[1] = -(Encoder_Get_Counter(2) - ENCODER_MID_VALUE);
//			
//			 encoder_delta[0] = Encoder_Get_Counter(1);
//       encoder_delta[1] = -(Encoder_Get_Counter(2));

//			 speed_out = Velocity(0, encoder_delta[0], encoder_delta[1]);	//速度环
//			
//			 vertical_out = Vertical(speed_out+Machine_Angle,imu_wit_data.pitch,imu_wit_data.gyro_y);			//直立环
//		
       //计算编码器累加值-里程
    //    encoder_inc[0] += encoder_delta[0];
    //    encoder_inc[1] += encoder_delta[1];
        //LOG_I("%d %d\r\n", encoder_delta[0], encoder_delta[1]);

//			LOG_I("Pitch:%d Roll:%d Yaw:%d\r\n",
//            robot_imu_dmp_data.pitch,
//            robot_imu_dmp_data.roll,
//            robot_imu_dmp_data.yaw);
			
//			 LOG_I("x:%d y:%d z:%d\r\n",
//            robot_imu_dmp_data.gyro.x,
//            robot_imu_dmp_data.gyro.y,
//            robot_imu_dmp_data.gyro.z);
			
       osDelay(5);
    }
}
