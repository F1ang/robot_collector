//
// Created by Hacker on 2024/5/5.
//

/*
  1、运算时间
    tim7:2khz中断(0.5ms),获取imu数据   
    2khz/20 = 100hz(10ms),计算编码速度,进行pid运算
  2、数据单位
    (1)姿态角
    roll:2.63 pitch:9.61  yaw:-5.33
    roll:3.26 pitch:18.79 yaw:-5.59
    (2)角速度
    X:0.00 Y:0.24 Z:-0.12
    X:4.15 Y:62.01 Z:-2.56
    X:-5.80 Y:-101.20 Z:3.11
  3、PID参数
    #define SPEED_KP 0.22
    float  Vertical_Kp = -370; 
    float  Vertical_Kd = -25; 

    float Velocity_Kp = -SPEED_KP;         
    float Velocity_Ki = -SPEED_KP/200;   
  4、编码器数据
    float Machine_Angle = 3.01;	//机械角度中值，自平衡的角度值 
    short encoder_delta[2];	// [-42,42]   超速-189 -177
  5、优化
    (1)跌倒输出清零
    (2)遥控+转向环
    (3)10ms 获取imu数据 进行PID控制 
    注:如果采用MPU6050,采样周期可以设200hz(5ms),在imu_task任务中进行数据融合,然后得到姿态角和角速度数据,在move_task()获取相对应数据即可
    本文采用了外置的IMU.
*/

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
extern osSemaphoreId move_semaHandle;


extern float speed_out;
extern float vertical_out;

void timx_updata_callback(TIM_HandleTypeDef *htim)
{
    __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
    //MPU6050_DMP_GetData(&robot_imu_dmp_data);
		//Wit_imu_data();
	
    speed_50hz++;
    if (speed_50hz == SPEED_HZ) {
        speed_50hz = 0;
			 
				// encoder_delta[0] = Encoder_Get_Counter(1);
        // encoder_delta[1] = -(Encoder_Get_Counter(2));

				// speed_out = Velocity(0, encoder_delta[0],encoder_delta[1]);  
				// vertical_out = Vertical(speed_out+Machine_Angle,imu_wit_data.pitch,imu_wit_data.gyro_y);			//直立环
				// MOTOR_SetSpeed(1, vertical_out);
				// MOTOR_SetSpeed(2, vertical_out);	
			
        xSemaphoreGiveFromISR(move_semaHandle,&xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
    }
}

