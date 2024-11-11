#include "logger_task.h"
#include "bsp_imu.h"

int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xffff);  
  return ch;
} 
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart3, &ch, 1, 0xffff);
  return ch;
}


void Logger_Task(void const * argument)
{
  for(;;)
  {
    //printf("Accel_x:%.1f  Gyro_x:%.1f  mx:%d\r\n", INS.Accel[X], INS.Gyro[X], mpu_data.mx); 
		printf("%f,%f,%f\n", INS.Roll, INS.Pitch, INS.Yaw);
    HAL_GPIO_TogglePin(LED8_GPIO_Port, LED8_Pin);
    osDelay(10);
  }
}
