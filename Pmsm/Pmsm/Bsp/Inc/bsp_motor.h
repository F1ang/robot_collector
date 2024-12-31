#ifndef _BSP_MOTOR_H_
#define _BSP_MOTOR_H_

#include "stm32f4xx_hal.h"
#include "main.h"
#include "bsp_foc.h"

/******************* 启动参数 **********************/
/*
              Speed /|\
FINAL_START_UP_SPEED |              /
                     |            /
                     |          /
                     |        /
                     |      /
                     |    /
                     |  /
                     |/_______________________________________\
                     0          FREQ_START_UP_DURATION      t /               */

/* 定义启动允许的总时间，单位毫秒 */
#define FREQ_START_UP_DURATION (uint16_t)1500 // in msec
#define FINAL_START_UP_SPEED   (uint16_t)2500 // Rotor mechanical speed (rpm)

/*
                 |I|/|\
                     |
      FINAL_I_STARTUP|       __________________
                     |     /
                     |    /
                     |   /
                     |  /
                     | /
      FIRST_I_STARTUP|/
                     |_______________________________________________\
                     0 I_START_UP_DURATION  FREQ_START_UP_DURATION t /        */

// With MB459 phase current = (X_I_START_UP * 0.64)/(32767 * Rshunt)
/* 三相电流系统的初始幅值 */
#define FIRST_I_STARTUP (uint16_t)3521
/* 三相电流系统的最终幅值。这幅值的选择应该使产生的磁场力矩与实际应用负载相匹配。 */
#define FINAL_I_STARTUP (uint16_t)4108
/* 从幅值的初始到最后，线性电流幅值增长允许时间，单位毫秒 */
#define I_START_UP_DURATION (uint16_t)350 // in msec

/* 电流采样频率 */
#define REP_RATE      (1)
#define SAMPLING_FREQ ((uint16_t)PWM_FREQ / ((REP_RATE + 1) / 2)) // Resolution: 1Hz

#define I_STARTUP_PWM_STEPS    (uint32_t)((I_START_UP_DURATION * SAMPLING_FREQ) / 1000)
#define I_INC                  (uint16_t)((FINAL_I_STARTUP - FIRST_I_STARTUP) * 1024 / I_STARTUP_PWM_STEPS)
#define FREQ_STARTUP_PWM_STEPS (uint32_t)((FREQ_START_UP_DURATION * SAMPLING_FREQ) / 1000)
#define FREQ_INC               (uint16_t)((FINAL_START_UP_SPEED * PAIRS_OF_POLES * 65536 / 60) / FREQ_STARTUP_PWM_STEPS)

#define K_CON (float)360 / 65536 // 标幺化

typedef enum {
    S_INIT,
    ALIGNMENT,
    RAMP_UP
} Start_upStatus_t;

extern void MCL_Init(void);
extern void STO_Start_Up(void);

#endif // _BSP_MOTOR_H_
