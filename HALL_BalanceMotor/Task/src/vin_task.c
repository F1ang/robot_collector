//
// Created by Hacker on 2024/4/28.
//
#include "vin_task.h"
#include "main.h"
#include "bsp_define.h"
#include "bsp_cmd.h"
#include "bsp_vin.h"

#define LOG_TAG  "VIN_TASK"
#include "bsp_log.h"

int battery_status = IS_FULL;

#define VOLTAGE_DROP            0.13214f
#define VOLTAGE_CHG				11.50f
#define VOLTAGE_OFF				11.00f

float battery_voltage;

void vin_task(void *argument)
{
    LOG_I("vin_Task\r\n");
    for(;;)
    {
       battery_voltage = get_battery_voltage() + VOLTAGE_DROP;
        //LOG_I("%f\r\n", battery_voltage);
//        if(battery_voltage < VOLTAGE_CHG)
//        {
//            //需要充电
//            battery_status = NEED_CHARGE;
//
//            if(battery_voltage < VOLTAGE_OFF)
//            {
//                osDelay(5000);
//                battery_voltage = get_battery_voltage() + VOLTAGE_DROP;
//
//                //电压持续小于VOLTAGE_OFF 5秒，进入停机保护状态
//                if(battery_voltage < VOLTAGE_OFF)
//                {
//                    battery_status = NEED_POWEROFF;
//                    //停止电机放在电机的任务里
//
//                    //停止检测
//                    LOG_I("电压过低，停止工作\r\n");
//                    while(1)
//                    {
//                        battery_voltage = get_battery_voltage() + VOLTAGE_DROP;
//                        if(battery_voltage >= VOLTAGE_OFF)
//                        {
//                            LOG_I("电压达到要求，重新工作\r\n");
//                            break;
//                        }
//                        osDelay(200);
//                    }
//                }
//            }
//        }
//        else
//        {
//            //电池正常
//            battery_status = IS_FULL;
//        }
        osDelay(200);
    }

}


