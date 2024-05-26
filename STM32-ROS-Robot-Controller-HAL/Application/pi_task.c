/*
	Copyright 2022 Fan Ziqi

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#include "main.h"
#include "bsp_define.h"
#include "bsp_usart.h"
#include "bsp_imu.h"
#include "string.h"
#include "bsp_gray.h"

#define LOG_TAG    "PI"
#include "bsp_log.h"

extern UART_HandleTypeDef huart2;

extern struct imu_data robot_imu_dmp_data;
extern int16_t robot_odom[]; //里程计数据，绝对值和变化值，x y yaw dx dy dyaw
extern int16_t encoder_delta[];	//编码器变化值
extern int16_t encoder_delta_target[];  //编码器目标变化值
extern float battery_voltage;

int16_t SendData[1] = {0};
void send_to_pi(void)
{
	SendData[0] = gray_sta;	
	//发送串口数据
	USART_Send_Pack(&huart2, SendData, 1, 06);
}

void pi_task(void const * argument)
{
	while(1)
	{
		//printf("%d \r\n", robot_odom[1]);
		send_to_pi();
		osDelay(20);
	}
}

