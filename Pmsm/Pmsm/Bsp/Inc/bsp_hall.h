#ifndef BSP_HALL_H_
#define BSP_HALL_H_

#include "bsp_foc.h"
#include "stm32f4xx_hal.h"

#define HALL_U_VALUE HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)
#define HALL_V_VALUE HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)
#define HALL_W_VALUE HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)

#define HALL_CLK 84000000UL // 84MHz时钟频率
#define HALL_PSC 128        // 预分频系数

#define S16_0_PHASE_SHIFT   (0UL)
#define S16_60_PHASE_SHIFT  (10923UL)
#define S16_120_PHASE_SHIFT (21845UL)
#define S16_180_PHASE_SHIFT (32768UL)
#define S16_240_PHASE_SHIFT (43691UL)
#define S16_300_PHASE_SHIFT (54613UL)

#define HALL_FW (+1L) // 霍尔周期正转
#define HALL_RV (-1L) // 霍尔周期反转
///< 120°电角度放置时的霍尔真值
#define HALL_5 (5UL)
#define HALL_1 (1UL)
#define HALL_3 (3UL)
#define HALL_2 (2UL)
#define HALL_6 (6UL)
#define HALL_4 (4UL)

extern uint8_t bHallState;
extern void BSP_Hall_Init(void);

extern float hall_get_Speed(foc_handler *foc_data);   // 电角速度
extern float hall_positionEst(foc_handler *foc_data); // 插值法电角度
extern float hall_pll_filter(foc_handler *foc_data);  // PLL电角度

extern void speed_updateMF_IT(TIM_HandleTypeDef *htim);                       // dpp(电角速度与电频率)插值拟合
extern void foc_processHF_IT(foc_handler *foc_data, ADC_HandleTypeDef *hadc); // 电角度更新

#endif /* BSP_HALL_H_ */
