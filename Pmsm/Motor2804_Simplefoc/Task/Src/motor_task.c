#include "motor_task.h"
#include "bsp_foc.h"

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
        else
            Start_Up(&foc_data_handler);
        osDelay(1);
    }
}
