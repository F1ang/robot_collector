#include "motor_task.h"
#include "bsp_foc.h"

foc_handler foc_data_handler;

void Motor_Task(void const *argument)
{
   printf("Motor Task Running\r\n");
   TIM1_PWM_Init();
   for (;;)
   {
      open_loop_speed_control(&foc_data_handler);
      osDelay(1);
   }
}
