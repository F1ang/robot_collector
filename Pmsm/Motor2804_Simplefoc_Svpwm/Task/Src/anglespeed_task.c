#include "anglespeed_task.h"
#include "bsp_as5600.h"
#include "bsp_foc.h"
#include "tim.h"

void AngleSpeed_Task(void const *argument)
{
    printf("anglespeed_task started\r\n");
    for (;;)
    {
        //GET_Speed(&foc_data_handler);
				//printf("%d,%d,%d\n", TIM1->CCR1, TIM1->CCR2, TIM1->CCR3); // Âí°°²¨,´æÔÚÃ÷ÏÔÃ«´Ì
        osDelay(10);
    }
}
