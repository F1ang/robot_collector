//
// Created by Hacker on 2024/4/28.
//

#include "other_task.h"
#include "main.h"
#include "bsp_define.h"
#include "bsp_cmd.h"
#include "imu_task.h"
#include "vin_task.h"

#define LOG_TAG  "OTHER_TASK"
#include "bsp_log.h"

void other_task(void *argument)
{
    LOG_I("other_task\r\n");
    for(;;)
    {
//        LOG_I("Pitch:%d Roll:%d Yaw:%d\r\n",
//            robot_imu_dmp_data.pitch,
//            robot_imu_dmp_data.roll,
//            robot_imu_dmp_data.yaw);
//        LOG_I("battery_voltage = %f\r\n", battery_voltage);
        osDelay(600);
    }

}