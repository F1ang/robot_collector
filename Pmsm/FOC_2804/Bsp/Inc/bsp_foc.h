#ifndef _BSP_FOC_H_
#define _BSP_FOC_H_

#include "stm32f4xx_hal.h"
#include "bsp_dwt.h"
#include "arm_math.h"
#include <math.h>
#include "stdint.h"
#include "stdlib.h"
#include "tim.h"
#include "adc.h"

#define FOC_FUNC_NUM 5          // FOC 定义的功能回调函数数量
#define _1_SQRT3 0.57735026919f // 1/sqrt(3)
#define _2_SQRT3 1.15470053838f // 2/sqrt(3)
#define SQRT3 1.73205080757f    // sqrt(3)
#define SQRT3_2 0.86602540378f  // sqrt(3)/2

#define VOLTAGE_POWER_LIMIT 12.0f // 电压功率限制,单位:V
#define PAIRS_OF_POLES 7          // 电机的对数对数

#define PI_VALUE 3.14159265358979323846f
#define DEG2RAD(x) (x * PI_VALUE / 180.0f)
#define ABS_X(x) ((x) < 0 ? -(x) : (x))

#define START_UP_UQ 3.0f // 启动时Uq

#define CKTIM 168000000
#define PWM_FREQ 16000
#define PWM_PRSC 0
#define PWM_PERIOD CKTIM / (2 * PWM_FREQ * (PWM_PRSC + 1))

#define DEADTIME_NS 1000                                                      // DT
#define MAX_TNTR_NS 1550                                                      // TN
#define TW_AFTER ((uint16_t)(((DEADTIME_NS + MAX_TNTR_NS) * 168uL) / 1000ul)) // DT+TN

#define SAMPLING_TIME_NS 700
#define TW_BEFORE (((uint16_t)(((((uint16_t)(SAMPLING_TIME_NS))) * 168uL) / 1000ul)) + 1)

typedef enum
{
    MOTOR_START_UP = 0, // 预定位
    MOTOR_RUN = 1,      // 运行
} MOTOR_MODE;

typedef enum
{
    MOTOR_CW = -1, // 正转
    MOTOR_CCW = 1,
} MOTOR_DIR;

typedef struct
{
    float kp, ki, kd;
    float target_pos, real_pos;
    float error_pos, error_pos_last;
} position_loop;

typedef struct
{
    float kp, ki, kd;
    float target_speed, real_speed;
    float error_speed, error_speed_last;
} speed_loop;

typedef struct
{
    float ua, ub, uc;
    float u_alpha, u_beta;
    float ud, uq;

    float ia, ib, ic;
    float i_aphla, i_bphla;
    float iq, id;

    float angle, angle_cal, angle_norm_deg, angle_norm_rad; // 原始角度与rad角度 归一化弧度(0~2pi)
    float angle_last, circle_num, angle_delta;
    float elec_angle;
    float angle_offset; // 机械角度偏移

    float voltage_power_offset; // 电压偏置,SPWM波无负值
    float speed;
    float speed_last;
    uint32_t delt_dt; // dt

    float speed_lpf_a;

    MOTOR_MODE m_mode;
    MOTOR_DIR m_dir;
    position_loop pos_loop;
    speed_loop speed_loop;
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
extern foc_handler foc_data_handler;

extern void foc_register_func(foc_register_func_list id, foc_callbak func);
extern void TIM1_PWM_Init(void);
extern void Set_SPWM(foc_handler *foc_data);

extern void Get_Elec_Angle(foc_handler *foc_data);
extern void GET_Speed(foc_handler *foc_data);

extern void open_loop_speed_control(foc_handler *foc_data);
extern void Start_Up(foc_handler *foc_data);
extern void Position_Control(foc_handler *foc_data);
extern void Speed_Control(foc_handler *foc_data);
extern float Limit_up_and_down(float input, float limit_down, float limit_up);
extern void Foc_Svpwm(foc_handler *foc_data, float Ts_pwn, float Udc_tem);
extern void SVPWM_Control(foc_handler *foc_data);

#endif // !_BSP_FOC_H_
