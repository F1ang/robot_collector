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
		// mpu_get_data();
		// imu_ahrs_update();
		// imu_attitude_update(); 
		//printf("xxx quat roll:%.1f  pitch:%.1f  yaw:%.1f\r\n", INS.Roll, INS.Pitch, INS.Yaw);
    INS_Task();	
    osDelay(1);
  }
}



