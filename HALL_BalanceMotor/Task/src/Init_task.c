//
// Created by Hacker on 2024/4/28.
//

#include "Init_task.h"

#define LOG_TAG  "INIT_TASK"
#include "bsp_log.h"

void Init_Task(void)
{
    Log_Init();
    VIN_Init();
    Encoder_Init();
    Motor_Init();
	  USART_Init();
}


