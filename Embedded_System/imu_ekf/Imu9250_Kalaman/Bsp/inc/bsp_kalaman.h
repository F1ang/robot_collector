#ifndef _BSP_KALAMAN_H_
#define _BSP_KALAMAN_H_

#include "stm32f4xx_hal.h"
#include "sys.h"
#include "delay.h"
#include "bsp_mpu9250.h"
#include "main.h"

extern float pitchoffset,rolloffset,yawoffset;
extern void AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,float *roll,float *pitch,float *yaw);
extern void CalibrateToZero(void);
extern void AHRS_EKF_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,float *roll,float *pitch,float *yaw);

#endif // !_BSP_KALAMAN_H_

