//
// Created by Hacker on 2024/4/28.
//

#include "main.h"
#include "bsp_define.h"
#include "bsp_cmd.h"
#include "cmd_task.h"
#include "bsp_cmd.h"

#define LOG_TAG    "CMD"
#include "bsp_log.h"

void cmd_task(void *argument)
{
    LOG_I("cmd_task\r\n");
    for(;;)
    {
        cmd();
        osDelay(200);
    }
}










