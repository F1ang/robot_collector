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

Pid_Position_t Vertical_Loop = NEW_BALANCE_PID(200,0,0,5000,7200,0,0);  //490,0,1.5  245,0,1.5
Pid_Position_t Speed_Loop = NEW_BALANCE_PID(1 ,0,0,5000,7200,0,0); //120 0.05
Pid_Position_t Turn_Loop = NEW_BALANCE_PID(1,0,0,50,1000,0,0);

extern float speed_out;
extern float vertical_out;

void move_task(void *argument)
{
    LOG_I("move_Task\r\n");
    for(;;)
    {
		/* 速度环 */
		//Speed_LoopPI(&Speed_Loop, encoder_delta[0], encoder_delta[1], 0);//速度环最高输出10度	
		//LOG_I("%d        %d\r\n", encoder_delta[0], encoder_delta[1]); // left wheel encoder error
		//LOG_I("%f\r\n", Speed_Loop.output);
			//Speed_Loop.output = 0;
//		MOTOR_SetSpeed(1, -Speed_Loop.output);
//	  MOTOR_SetSpeed(2, Speed_Loop.output);	
		
		//Speed_Loop.output = Velocity(0, encoder_delta[0], encoder_delta[1]);	//速度环
//		Vertical_Loop.output = Vertical(Speed_Loop.output+Machine_Angle,robot_imu_dmp_data.pitch,robot_imu_dmp_data.gyro.y);			//直立环
//		//LOG_I("%f\r\n", Speed_Loop.output);
			
			
//		MOTOR_SetSpeed(1, -Speed_Loop.output);
//		MOTOR_SetSpeed(2, Speed_Loop.output);	
		
	//Vertical_Loop.output = Speed_Loop.output;
			
		//Vertical_Loop.output = Speed_Loop.output;
	
//		MOTOR_SetSpeed(1, vertical_out);
//		MOTOR_SetSpeed(2, vertical_out);	

//		MOTOR_SetSpeed(1, speed_out);
//		MOTOR_SetSpeed(2, speed_out);	
		

			
		//Speed_Loop.output = 9;
		/* 直立环 */
		//Vertical_LoopPD(&Vertical_Loop, Speed_Loop.output+Machine_Angle, robot_imu_dmp_data.roll, robot_imu_dmp_data.gyro.x);	//DMP读取的角速度变化太大,可以直接读取int16的AD值	

        // MOTOR_SetSpeed(1, 1500);
        // MOTOR_SetSpeed(2, 1500);

		//Vertical(Speed_Loop.output+Med_Angle,Roll,gyrox);	
		
		//LOG_I("%f\r\n", Vertical_Loop.output);
//		LOG_I("Pitch:%d Roll:%d Yaw:%d\r\n",
//            robot_imu_dmp_data.pitch,
//            robot_imu_dmp_data.roll,
//            robot_imu_dmp_data.yaw);
			
		//LOG_I("x:%d y:%d z:%d\r\n",
//					robot_imu_dmp_data.gyro.x,
//					robot_imu_dmp_data.gyro.y,
//					robot_imu_dmp_data.gyro.z);

//      Vertical_Loop.output = 0;
//      MOTOR_SetSpeed(1, Vertical_Loop.output);
//	    MOTOR_SetSpeed(2, Vertical_Loop.output);	

//			MOTOR_SetSpeed(1, Speed_Loop.output + Turn_Loop.output);
//	    MOTOR_SetSpeed(2, Speed_Loop.output + Turn_Loop.output);	
			
       osDelay(10);
    }
}


