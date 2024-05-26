//
// Created by Hacker on 2024/4/28.
//

#ifndef HALL_BALANCEMOTOR_IMU_TASK_H
#define HALL_BALANCEMOTOR_IMU_TASK_H

#include "main.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "bsp_imu.h"

extern struct imu_data robot_imu_dmp_data;
extern float Machine_Angle;	
extern int32_t encoder_inc[2];
extern int16_t encoder_delta[2];

#endif //HALL_BALANCEMOTOR_IMU_TASK_H




