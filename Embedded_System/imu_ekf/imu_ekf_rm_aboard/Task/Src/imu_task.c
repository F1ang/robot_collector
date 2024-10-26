#include "imu_task.h"
#include "bsp_imu.h"

extern UART_HandleTypeDef huart6;
extern imu_t              imu;
char buf[300];
int count;

void Imu_Task(void const * argument)
{
	printf("rm_aboard & imu & ekf\r\n");
  for(;;)
  {
		mpu_get_data();
		imu_ahrs_update();
		imu_attitude_update(); 	
		printf("quat roll:%.1f  pitch:%.1f  yaw:%.1f\r\n", imu.rol, imu.pit, imu.yaw);
    osDelay(10);
  }
}



