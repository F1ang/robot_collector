#include "imu_task.h"
#include "bsp_imu.h"


char buf[300];
int count;

void Imu_Task(void const * argument)
{
  /* ekf */
  INS_Init();
  for(;;)
  {
    INS_Task();	
    osDelay(1);
  }
}



