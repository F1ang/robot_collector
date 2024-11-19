#ifndef _BSP_FOC_H_
#define _BSP_FOC_H_

#include "stm32f4xx_hal.h"
#include "bsp_dwt.h"
#include "arm_math.h"
#include <math.h>
#include "stdint.h"
#include "stdlib.h"
#include "tim.h"

#define FOC_FUNC_NUM 5          // FOC 定义的功能回调函数数量
#define _1_SQRT3 0.57735026919f // 1/sqrt(3)
#define _2_SQRT3 1.15470053838f // 2/sqrt(3)
#define SQRT3 1.73205080757f    // sqrt(3)
#define SQRT3_2 0.86602540378f  // sqrt(3)/2

#define VOLTAGE_POWER_LIMIT 12.0f // 电压功率限制,单位:V
#define PAIRS_OF_POLES 7          // 电机的对数对数

typedef struct
{
   float ua, ub, uc;
   float u_alpha, u_beta;
   float ud, uq;

   float ia, ib, ic;
   float i_aphla, i_bphla;
   float iq, id;

   float angle;
   float angle_last;
   float elec_angle;
   float voltage_power_offset; // 电压偏置,SPWM波无负值
   float speed;
   uint32_t delt_dt; // dt
} foc_handler;

enum foc_transform_list
{
   CLARKE_TRANSFORM = 0,
   CLARKE_INVERSE_TRANSFORM,
   PARK_TRANSFORM,
   PARK_INVERSE_TRANSFORM
};

typedef enum
{
   SET_SVPWM_FUNC = 0,
   READ_CURR_FUNC,
   READ_ANGLE_FUNC,
   READ__FUNC
} foc_register_func_list;

typedef void (*foc_callbak)(foc_handler *foc_data);
extern foc_callbak foc_transform[];

extern void foc_register_func(foc_register_func_list id, foc_callbak func);
extern void TIM1_PWM_Init(void);
extern void Set_SVPWM(foc_handler *foc_data);

extern void Get_Elec_Angle(foc_handler *foc_data);
extern void GET_Speed(foc_handler *foc_data);

extern void open_loop_speed_control(foc_handler *foc_data);

#endif // !_BSP_FOC_H_
