//
// Created by Hacker on 2024/4/28.
//
#include "pi_task.h"
#include "main.h"
#include "bsp_define.h"
#include "bsp_cmd.h"

#define LOG_TAG  "PI_TASK"
#include "bsp_log.h"

void pi_task(void *argument)
{
    LOG_I("pi_Task\r\n");
    for(;;)
    {
        osDelay(200);
    }
}