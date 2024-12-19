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

#define FOC_FUNC_NUM 5              // FOC 定义的功能回调函数数量
#define _1_SQRT3     0.57735026919f // 1/sqrt(3)
#define _2_SQRT3     1.15470053838f // 2/sqrt(3)
#define SQRT3        1.73205080757f // sqrt(3)
#define SQRT3_2      0.86602540378f // sqrt(3)/2

#define VOLTAGE_POWER_LIMIT 12.0f // 电压功率限制,单位:V
#define PAIRS_OF_POLES      4     // 电机的对数对数

#define PI_VALUE   3.14159265358979323846f
#define PI2_VALUE  2 * PI_VALUE
#define DEG2RAD(x) (x * PI_VALUE / 180.0f)
#define ABS_X(x)   ((x) < 0 ? -(x) : (x))

#define START_UP_UQ 3.0f // 启动时Uq

#define CKTIM      168000000
#define PWM_FREQ   16000
#define PWM_PRSC   0
#define PWM_PERIOD CKTIM / (2 * PWM_FREQ * (PWM_PRSC + 1))

#define DEADTIME_NS 1000                                                         // DT
#define MAX_TNTR_NS 1550                                                         // TN
#define TW_AFTER    ((uint16_t)(((DEADTIME_NS + MAX_TNTR_NS) * 168uL) / 1000ul)) // DT+TN

#define SAMPLING_TIME_NS 700
#define TW_BEFORE        (((uint16_t)(((((uint16_t)(SAMPLING_TIME_NS))) * 168uL) / 1000ul)) + 1)

#define MMI 0.95 // 过调制
#define R1  32768
#define R2  MMI *R1

/* 电机参数 */
#define SHUNT_RESISTOR (0.02f)     ///< 采样电阻阻值(单位：Ω）
#define U_DC           (24.0f)     ///< 电机的直流动力供电电压
#define P              (4UL)       ///< 电机极对数
#define Rs             (0.68f)     ///< 定子电阻(单位:ohms@25℃)
#define Ls             (0.000426f) ///< 定子电感(单位:H@1khz)
#define NORMAL_CURRENT (4f)        ///< 正常运行电流(单位:A)
#define NORMAL_SPEED   (3000UL)    ///< 额定转速(单位:rpm)
#define FLUX_VALUE     (0.0083817f)
#define B_EMF          (0.0580703f)  ///< 反电动势常数(单位:V.s/rad)
#define J              (0.00000173f) ///< 转子惯量(单位:kg*m2)1kg*m^2=1N*m^2/(m/s^2)=1N*m*s^2
#define MOS_DELAY      (80UL)        ///< 开关管最大开关时延(单位:ns)

/* TIM1参数 */
#define TS             (10499.0f) ///< PWM周期浮点型F
#define TS_UL          (10499UL)  ///< PWM周期定点型Q
#define PERIOD         (5249.0f)  ///< 输出定时器的重装载值F
#define PERIOD_UL      (5249UL)   ///< 输出定时器的重装载值Q
#define PWM_HalfPerMax (2624)     ///< 输出定时器的重装载值中值Q
#define PWM_MODE2                 ///< 此处采用PWM模式2
#define FOC_PERIOD     (62.5f)    ///< FOC核心部分的执行周期 (单位:us):1/PWM_FREQ
#define SPEED_PERIOD   (1000UL)   ///< 速度更新周期 (单位:us)

#define WBuffer_MAX_SIZE (6UL)

typedef enum {
    MOTOR_START_UP = 0, // 预定位
    MOTOR_RUN = 1,      // 运行
} MOTOR_MODE;

typedef enum {
    MOTOR_CW = -1, // 正转
    MOTOR_CCW = 1,
} MOTOR_DIR;

typedef struct {
    float kp, ki, kd;
    float target_pos, real_pos;
    float error_pos, error_pos_last;
} position_loop;

typedef struct {
    float kp, ki, kd;
    float target_speed, real_speed;
    float error_speed, error_speed_last;
} speed_loop;

typedef struct {
    float kp, ki, kd, kp_flux, ki_flux, kd_flux;
    float target_torque, real_torque, target_flux, real_flux;
    float error_torque, error_torque_last;
} torque_loop;

typedef struct {
    int16_t hall_psc;
    int32_t hall_cap;
    int8_t dir, overcount;
    uint8_t RatioDec;                                          // 预分频变小标志位
    uint8_t RatioInc;                                          // 预分频变大标志位
    float refer_theta, hall_theta, comp_dpp, hall_theta_inter; // 插值拟合法 小角度增量
    float omega_inter;                                         // 角速度插值法
    float pll_theta, pll_omega;                                // PLL跟踪法
    float pll_kp, pll_ki, pll_kd;
    float pll_Te;

    uint32_t f_hall, f_foc;
    int32_t dpp;                      // 电频率(2pi~65536)
    int32_t buffer[WBuffer_MAX_SIZE]; // 均值速度
    uint8_t buffer_index;             // 记录当前buffer缓存的下标
    int32_t dpp_sum, avg_dpp;         // 均值电频率

    int32_t dpp_2;
    int32_t dpp_1;
    int32_t delta_theta; // 就是文中的Δθ（插值法）
    int32_t a_dpp;
} hall_hander;

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
    uint8_t sector;     // 扇区号

    float voltage_power_offset; // 电压偏置,SPWM波无负值
    float speed;
    float speed_last;
    uint32_t delt_dt; // dt

    float speed_lpf_a;

    MOTOR_MODE m_mode;
    MOTOR_DIR m_dir;
    position_loop pos_loop;
    speed_loop speed_loop;
    torque_loop tq_loop;
    hall_hander hall_ops;
} foc_handler;

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

typedef void (*foc_callbak)(foc_handler *foc_data);
extern foc_callbak foc_transform[];
extern foc_handler foc_data_handler;

extern void foc_register_func(foc_register_func_list id, foc_callbak func);
extern void TIM1_PWM_Init(void);

extern void Start_Up(foc_handler *foc_data);
extern float Limit_up_and_down(float input, float limit_down, float limit_up);
extern void Foc_Svpwm(foc_handler *foc_data, float Ts_pwn, float Udc_tem);
extern void FOC_Control(foc_handler *foc_data);

#endif // !_BSP_FOC_H_
