/*
 * @Description: 灰度传感器
 * @Welcome: hacker
 */

#include "main.h"
#include "bsp_gray.h"
#include "bsp_define.h"
#include "bsp_pid.h"
#include "bsp_delay.h"

#define LOG_TAG    "GRAY"
#include "bsp_log.h"

int16_t gray_sta = 0x00;
int16_t gray_error = 0;
Control_Gray control_flag;

/**
 * @description: 读取灰度传感器数据 亮-0 暗-1
 * @return {*}
 */
void Read_INX(void)
{
	/* 假定yaw最大脉冲变化量12 */
	//左
	if(IN1_OUT1 == 0) {
		gray_sta |= 0x01;
		gray_error = 12;
	}
	else gray_sta &= 0xFE;

	if(IN2_OUT2 == 0) {
		gray_sta |= 0x02;
		gray_error = 8;
	}
	else gray_sta &= 0xFD;

	if(IN3_OUT3 == 0) {
		gray_sta |= 0x04;
		gray_error = 4;
	}
	else gray_sta &= 0xFB;

	if(IN4_OUT4 == 0) {
		gray_sta |= 0x08;
		gray_error = 0;
	}
	else gray_sta &= 0xF7;

	//右
	if(IN5_OUT5 == 0) {
		gray_sta |= 0x10;
		gray_error = 0;
	}
	else gray_sta &= 0xEF;

	if(IN6_OUT6 == 0) {
		gray_sta |= 0x20;
		gray_error = -4;
	}
	else gray_sta &= 0xDF;

	if(IN7_OUT7 == 0) {
		gray_sta |= 0x40;
		gray_error = -8;
	}
	else gray_sta &= 0xBF;

	if(IN8_OUT8 == 0) {
		gray_sta |= 0x80;
		gray_error = -12;
	}
	else gray_sta &= 0x7F;
}	

/**
 * @description: 基础巡线(20ms)
 * @return {*}
 * @param {int16_t} *robot_target
 */
void Find_Line(int16_t *robot_target)
{
	robot_target[0] = 150;
	robot_target[1] = 0;
	robot_target[3] = PID_FindLine(0, gray_error);
}

/**
 * @description: 停车
 * @return {*}
 * @param {int16_t} *robot_target
 */
void Stop_Line(int16_t *robot_target)
{
	robot_target[0] = 0;
	robot_target[1] = 0;
	robot_target[2] = 0;
}

/**
 * @description: 直行路口x,y,yaw
 * @return {*}
 * @param {int16_t} *robot_target
 */
void Goforword(int16_t *robot_target)
{
	robot_target[0] = 150;
	robot_target[1] = 0;
	robot_target[2] = 0;
}

/**
 * @description: 左拐路口
 * @return {*}
 * @param {int16_t} *robot_target
 */
void Turn_Left(int16_t *robot_target)
{
	robot_target[0] = 200;
	robot_target[1] = 200;
	robot_target[2] = 0;
}

/**
 * @description: 右拐路口
 * @return {*}
 * @param {int16_t} *robot_target
 */
void Turn_Right(int16_t *robot_target)
{
	robot_target[0] = 200;
	robot_target[1] = -200;
	robot_target[2] = 0;
}


