#ifndef BSP_GRAY_H
#define BSP_GRAY_H
#include "main.h"

#define IN1_OUT1 HAL_GPIO_ReadPin(IN1_GPIO_Port, IN1_Pin)
#define IN2_OUT2 HAL_GPIO_ReadPin(IN2_GPIO_Port, IN2_Pin)
#define IN3_OUT3 HAL_GPIO_ReadPin(IN3_GPIO_Port, IN3_Pin)
#define IN4_OUT4 HAL_GPIO_ReadPin(IN4_GPIO_Port, IN4_Pin)
#define IN5_OUT5 HAL_GPIO_ReadPin(IN5_GPIO_Port, IN5_Pin)
#define IN6_OUT6 HAL_GPIO_ReadPin(IN6_GPIO_Port, IN6_Pin)
#define IN7_OUT7 HAL_GPIO_ReadPin(IN7_GPIO_Port, IN7_Pin)
#define IN8_OUT8 HAL_GPIO_ReadPin(IN8_GPIO_Port, IN8_Pin)

typedef struct 
{
   uint8_t turn_yaw; //1-左拐路口 2-直行路口 3-右拐路口 0-停 5-巡线  4-大转弯--暂定2个ROI识别TAG
}Control_Gray;


extern int16_t gray_sta; 
extern int16_t gray_error;
extern Control_Gray control_flag;

void Read_INX(void);
void Find_Line(int16_t *robot_target);
void Stop_Line(int16_t *robot_target);
void Goforword(int16_t *robot_target);
void Turn_Left(int16_t *robot_target);
void Turn_Right(int16_t *robot_target);

#endif


