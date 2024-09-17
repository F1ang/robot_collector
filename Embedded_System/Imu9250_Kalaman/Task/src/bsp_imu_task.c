#include "bsp_imu_task.h"
#include "bsp_usart.h"
#include "bsp_mpu9250.h"

//void Imu_Task(void const *argument)
//{
//    short igx,igy,igz;
//    float gx,gy,gz;
//    printf("imu_task!\r\n");
//    if(MPU9250_Init()){
//         printf("error\r\n");
//    }
//    for (;;)
//    {
////        MPU_Get_Gyro(&igx,&igy,&igz,&gx,&gy,&gz);
////        printf("%0.1f %0.1f %0.1f",gx,gy,gz);	
//        osDelay(10);
//    }
//}
