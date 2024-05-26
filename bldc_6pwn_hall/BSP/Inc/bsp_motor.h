#ifndef __BSP_MOTOR_H__
#define __BSP_MOTOR_H__

#include <stdio.h>
#include "stm32f10x.h"

#define  MOTOR_RATED_SPEED        2500              //额定转速
#define  MOTOR_MAX_SPEED          3500              //空载转速
#define  MOTOR_MIN_SPEED          100               
#define  MOTOR_POLE_PAIR_NUM      4                 //极对数（8 磁极数）

typedef enum
{
  CW=0,  // 顺时钟方向(对着电机轴看)，一般定义为反转
  CCW=1  // 逆时针方向(对着电机轴看)，一般定义为正转
}MOTOR_DIR;

typedef enum 
{
  STOP=0,  // 停机
  RUN=1    // 运行
} MOTOR_STATE;

typedef struct
{
  int          motor_speed;        // 电机转速(RPM):0..3500
  MOTOR_STATE  motor_state;        // 电机旋转状态
  MOTOR_DIR    motor_direction;    // 电机转动方向
  uint32_t     step_counter;       // 霍尔传感器步数，用于测量电机转速
  uint16_t     stalling_count;     // 停机标志，如果该值超2000，认为电机停止旋转
} MOTOR_DEVICE;


extern MOTOR_DEVICE bldc_dev;
extern uint16_t speed_duty;

#endif







