#ifndef _BSP_FOC_H_
#define _BSP_FOC_H_

#include "adc.h"
#include "arm_math.h"
#include "bsp_dwt.h"
#include "stdint.h"
#include "stdlib.h"
#include "stm32f4xx_hal.h"
#include "tim.h"
#include <math.h>
#include "main.h"

#include "bsp_hall.h"
#include "bsp_adc.h"

/* 中心对齐的PWM周期 */
#define CKTIM      168000000 // 168Mhz时钟频率
#define PWM_FREQ   16000     // 16kHz PWM频率
#define PWM_PRSC   0         // 预分频系数
#define PWM_PERIOD CKTIM / (2 * PWM_FREQ * (PWM_PRSC + 1))

/* 下桥臂采样的MOS管 */
#define DEADTIME_NS 1000                                                         // DT
#define MAX_TNTR_NS 1550                                                         // TN
#define TW_AFTER    ((uint16_t)(((DEADTIME_NS + MAX_TNTR_NS) * 168uL) / 1000ul)) // DT+TN

#define SAMPLING_TIME_NS 700 // ADC采样转换时间
#define TW_BEFORE \
    (((uint16_t)(((((uint16_t)(SAMPLING_TIME_NS))) * 168uL) / 1000ul)) + 1) // TW

/* 过调制 */
#define MMI 0.95    // 过调制
#define R1  32768   // 原圆R1
#define R2  MMI *R1 // 过调制后圆R2

/* 数学常数 */
#define FOC_FUNC_NUM 5                       // FOC 定义的功能回调函数数量
#define _1_SQRT3     0.57735026919f          // 1/sqrt(3)
#define _2_SQRT3     1.15470053838f          // 2/sqrt(3)
#define SQRT3        1.73205080757f          // sqrt(3)
#define SQRT3_2      0.86602540378f          // sqrt(3)/2
#define PI_VALUE     3.14159265358979323846f // pi
#define PI2_VALUE    2 * PI_VALUE            // 2pi
#define DEG2RAD(x)   (x * PI_VALUE / 180.0f) // 角度转弧度
#define ABS_X(x)     ((x) < 0 ? -(x) : (x))  // 取绝对值

/* 电机参数 */
#define VOLTAGE_POWER_LIMIT 23.0f       // 电压功率限制,单位:V
#define SHUNT_RESISTOR      (0.02f)     // 采样电阻阻值(单位：Ω）
#define U_DC                (24.0f)     // 电机的直流动力供电电压
#define P                   (4UL)       // 电机极对数
#define Rs                  (0.68f)     // 定子电阻(单位:ohms@25℃)
#define Ls                  (0.000426f) // 定子电感(单位:H@1khz)
#define NORMAL_CURRENT      (4f)        // 正常运行电流(单位:A)
#define NORMAL_SPEED        (3000UL)    // 额定转速(单位:rpm)
#define FLUX_VALUE          (0.0083817f)
#define B_EMF               (0.0580703f)  // 反电动势常数(单位:V.s/rad)
#define J                   (0.00000173f) // 转子惯量(单位:kg*m2)1kg*m^2=1N*m^2/(m/s^2)=1N*m*s^2
#define MOS_DELAY           (80UL)        // 开关管最大开关时延(单位:ns)

/* TIM1参数 */
#define TS             (10499.0f) ///< PWM周期浮点型F
#define TS_UL          (10499UL)  ///< PWM周期定点型Q
#define PERIOD         (5249.0f)  ///< 输出定时器的重装载值F
#define PERIOD_UL      (5249UL)   ///< 输出定时器的重装载值Q
#define PWM_HalfPerMax (2624)     ///< 输出定时器的重装载值中值Q
#define PWM_MODE2                 ///< 此处采用PWM模式2
#define FOC_PERIOD     (62.5f)    ///< FOC核心部分的执行周期 (单位:us):1/PWM_FREQ
#define SPEED_PERIOD   (1000UL)   ///< 速度更新周期 (单位:us)

#define WBuffer_MAX_SIZE   (6UL) // 滑动均值滤波
#define HALL_MAX_OVERFLOWS ((uint16_t)2)

typedef enum {
    MOTOR_CW = 1, // 正转
    MOTOR_CCW = -1,
} MOTOR_DIR;

/* 位置PID */
typedef struct {
    float kp_pos, ki_pos, kd_pos;
    float target_pos, real_pos;
    float error_pos, error_pos_last;
    float integral_pos; // 积分项
} position_loop_t;

/* 速度PID */
typedef struct {
    float kp_speed, ki_speed, kd_speed;
    float target_speed, real_speed;
    float error_speed, error_speed_last;
    float integral_speed; // 积分项
} speed_loop_t;

/* 转矩PID */
typedef struct {
    float kp_torque, ki_torque, kd_torque;
    float target_torque, real_torque;
    float error_torque, error_torque_last;
    float integral_torque; // 积分项
} torque_loop_t;

/* 励磁PID */
typedef struct {
    float kp_flux, ki_flux, kd_flux;
    float target_flux, real_flux;
    float error_flux, error_flux_last;
    float integral_flux; // 积分项
} flux_loop_t;

/* ADC数据 */
typedef struct {
    float Ia_Raw, Ib_Raw, Ic_Raw;
} adc_hander_t;

enum foc_transform_list {
    CLARKE_TRANSFORM = 0,
    CLARKE_INVERSE_TRANSFORM,
    PARK_TRANSFORM,
    PARK_INVERSE_TRANSFORM
};

typedef enum {
    SET_SVPWM_FUNC = 0,
    READ_CURR_FUNC,
    READ_ANGLE_FUNC,
    READ__FUNC
} foc_register_func_list;

typedef struct {
    float angle_machine, elec_angle; // 机械角度和电角度
    uint32_t delt_dt;                // dt
    uint8_t sector;                  // 扇区号

    float ua, ub, uc; // 相电压
    float u_alpha, u_beta;
    float ud, uq;

    float ia, ib, ic; // 相电流
    float i_aphla, i_bphla;
    float iq, id;

    MOTOR_DIR m_dir;           // 电机方向
    position_loop_t pos_loop;  // 位置环
    speed_loop_t speed_loop;   // 速度环
    torque_loop_t torque_loop; // 转矩环
    flux_loop_t flux_loop;     // 励磁环

    // hall_hander_t hall_t; // hall
    adc_hander_t adc_t; // adc
} foc_handler;

typedef void (*foc_callbak)(foc_handler *foc_data);
extern foc_callbak foc_transform[];
extern uint32_t tim6_uptate_over; // 溢出计数
extern foc_handler foc_data_handler;

extern void foc_register_func(foc_register_func_list id, foc_callbak func);
extern void TIM1_PWM_Init(void);
extern void Foc_Svpwm(foc_handler *foc_data, float Ts_pwn, float Udc_tem);
extern void FOC_Control(foc_handler *foc_data);

extern void Get_ADC_Current(foc_handler *foc_data);
extern void Get_Ia_Ib(foc_handler *foc_data);

extern void FOC_Init(void);

#endif // !_BSP_FOC_H_
