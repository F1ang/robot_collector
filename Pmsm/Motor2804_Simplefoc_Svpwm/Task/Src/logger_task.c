#include "logger_task.h"
#include "bsp_foc.h"
#include "bsp_usart.h"
#include "bsp_cmd.h"
#include "bsp_adc.h"
#include "tim.h"

void Logger_Task(void const *argument)
{
    USART_Init();
    for (;;)
    {
        // Get_ADC_Current(&foc_data_handler);
        // printf("Ia = %0.2f, Ib = %0.2f, Ic = %0.2f\r\n", bsp_adc.Ia, bsp_adc.Ib, bsp_adc.Ic);
        // printf("%d,%d, %d\n", TIM1->CCR1, TIM1->CCR2, TIM1->CCR3);
        cmd();
        osDelay(500);
    }
}
