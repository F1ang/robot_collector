//
// Created by Hacker on 2024/4/28.
//
#include "key_task.h"

#define LOG_TAG  "KEY_TASK"
#include "bsp_log.h"

void key_task(void *argument)
{
    LOG_I("key_task\r\n");
    for(;;)
    {
        osDelay(100);
    }
}

