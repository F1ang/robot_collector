//
// Created by Hacker on 2024/4/28.
//

#include "led_task.h"

#define LOG_TAG  "LED_TASK"
#include "bsp_log.h"


void led_task(void *argument)
{
    LOG_I("led_task\r\n");
    for(;;)
    {
        HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
        osDelay(1000);
    }
}



