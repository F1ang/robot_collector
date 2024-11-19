#include "logger_task.h"
#include "bsp_foc.h"

int fputc(int ch, FILE *f)
{
   HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
   return ch;
}
int fgetc(FILE *f)
{
   uint8_t ch = 0;
   HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
   return ch;
}

extern uint16_t angle_raw;
extern uint16_t angle;
void Logger_Task(void const *argument)
{
   for (;;)
   {
      printf("angle_raw: %d, angle: %d\r\n", angle_raw, angle);
      osDelay(500);
   }
}
