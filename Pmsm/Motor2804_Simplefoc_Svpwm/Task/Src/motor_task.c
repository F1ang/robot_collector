#include "motor_task.h"
#include "bsp_foc.h"
#include "bsp_adc.h"

foc_handler foc_data_handler;

void Motor_Task(void const *argument)
{
    printf("Motor Task Running\r\n");
    TIM1_PWM_Init();
    foc_data_handler.m_dir = MOTOR_CW;
    for (;;)
    {
        if (foc_data_handler.m_mode == MOTOR_RUN)
            open_loop_speed_control(&foc_data_handler);
        // close_loop_curr_control(&foc_data_handler);
        else
            Start_Up(&foc_data_handler);
        printf("%d,%d,%d\n", TIM1->CCR1, TIM1->CCR2, TIM1->CCR3); // 马鞍波,存在明显毛刺
        osDelay(1);
    }
}
