#include "anglespeed_task.h"
#include "bsp_as5600.h"
#include "bsp_foc.h"

void AngleSpeed_Task(void const *argument)
{
    printf("anglespeed_task started\r\n");
    for (;;)
    {
        GET_Speed(&foc_data_handler);
        osDelay(10);
    }
}
