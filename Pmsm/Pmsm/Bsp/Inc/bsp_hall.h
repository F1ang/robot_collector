#ifndef _BSP_HALL_H_
#define _BSP_HALL_H_

#include "main.h"
#include "tim.h"
#include "bsp_motor.h"

#define HALL_U_VALUE HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)
#define HALL_V_VALUE HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)
#define HALL_W_VALUE HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)

/* CLK系数 */
#define HALL_CLK           84000000UL // 84MHz时钟频率
#define HALL_PSC           128        // 预分频系数
#define HALL_COUNTER_RESET ((uint16_t)0)

/* 滑动均值 */
#define HALL_SPEED_FIFO_SIZE ((uint8_t)6) // 滑动均值滤波BUFF

/* HALL捕获:psc、cap、dir */
#define HALL_MAX_RATIO ((uint16_t)800u) // 定义可以测量的最低速度(当计数=0xFFFF)
#define NEGATIVE       (int8_t) - 1     // 负方向
#define POSITIVE       (int8_t)1        // 正方向

/* 扇区sector */
#define STATE_0 (uint8_t)0
#define STATE_1 (uint8_t)1
#define STATE_2 (uint8_t)2
#define STATE_3 (uint8_t)3
#define STATE_4 (uint8_t)4
#define STATE_5 (uint8_t)5
#define STATE_6 (uint8_t)6
#define STATE_7 (uint8_t)7

#define HALL_PHASE_SHIFT    (int16_t)120
#define S16_PHASE_SHIFT     (int16_t)(HALL_PHASE_SHIFT * 65536 / 360)
#define S16_120_PHASE_SHIFT (int16_t)(65536 / 3)
#define S16_60_PHASE_SHIFT  (int16_t)(65536 / 6)

// With digit-per-PWM unit (here 2*PI rad = 0xFFFF):
#define HALL_MAX_PSEUDO_SPEED ((int16_t) - 32768)

#define NEGATIVE_SWAP     (int8_t) - 2
#define POSITIVE_SWAP     (int8_t)2
#define ERROR             (int8_t)127
#define LOW_RES_THRESHOLD ((uint16_t)0x5500u) // If capture below, ck prsc decreases

/* 自适应转速的HALL TIM3分辨率 */

#define HALL_MAX_SPEED_FDBK_RPM ((uint32_t)30000) /* 定义转子最大机械转速（转数/分），当大于它时，速度反馈不可用：用于区别故障*/
#define HALL_MIN_SPEED_FDBK_RPM ((uint16_t)60)    /* 定义转子最小机械转速(rpm)，当小于它时，速度反馈不可用。 */
#define HALL_MAX_SPEED_FDBK     (uint16_t)(HALL_MAX_SPEED_FDBK_RPM / 6 * PAIRS_OF_POLES)
#define HALL_MIN_SPEED_FDBK     (uint16_t)(HALL_MIN_SPEED_FDBK_RPM / 6 * PAIRS_OF_POLES)
#define ROTOR_SPEED_FACTOR      ((uint32_t)((HALL_CLK * 10)) / 3)
#define PSEUDO_FREQ_CONV        ((uint32_t)(ROTOR_SPEED_FACTOR / (SAMPLING_FREQ * 10)) * 0x10000uL)
#define SPEED_OVERFLOW          ((uint32_t)(ROTOR_SPEED_FACTOR / HALL_MAX_SPEED_FDBK))
#define MAX_PERIOD              ((uint32_t)(ROTOR_SPEED_FACTOR / HALL_MIN_SPEED_FDBK))

typedef struct {
    uint16_t hCapture;
    uint16_t hPrscReg;
    int8_t bDirection;
} SpeedMeas_s;

typedef struct {
    uint32_t wPeriod;
    int8_t bDirection;
} PeriodMeas_s;

typedef enum {
    FALSE = 0,
    TRUE = !FALSE
} bool;

/* PLL锁相环跟踪 */
typedef struct {
    float pll_theta, pll_omega; // PLL跟踪法
    float pll_kp, pll_ki, pll_kd;
    float pll_Te;
} pll_filter_t;

typedef struct {
    pll_filter_t pll_filter;
} hall_hander_t;

extern uint8_t bGP1_OVF_Counter; // 捕获溢出计数器
extern bool HallTimeOut;

extern void BSP_Hall_Init(void);
extern void HALL_InitHallMeasure(void);
extern void HALL_Init_Electrical_Angle(void);
extern int16_t HALL_GetElectricalAngle(void);
extern void HALL_IncElectricalAngle(void);
extern void HALL_ClearRotorFreq_Dpp(void);
extern int16_t HALL_SetRotorFreq_Dpp(void);

#endif /* _BSP_HALL_H_ */
