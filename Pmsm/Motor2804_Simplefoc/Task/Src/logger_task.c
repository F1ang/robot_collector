#include "logger_task.h"
#include "bsp_foc.h"
#include "bsp_usart.h"

void Logger_Task(void const *argument)
{
    USART_Init();
    for (;;)
    {
        cmd();
        osDelay(500);
    }
}
