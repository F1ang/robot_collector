/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#define PAIRS_OF_POLES 4 // 电机的极对数
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC_U_Pin        GPIO_PIN_0
#define ADC_U_GPIO_Port  GPIOA
#define ADC_V_Pin        GPIO_PIN_1
#define ADC_V_GPIO_Port  GPIOA
#define ADC_W_Pin        GPIO_PIN_2
#define ADC_W_GPIO_Port  GPIOA
#define HALL_U_Pin       GPIO_PIN_6
#define HALL_U_GPIO_Port GPIOA
#define PU_L_Pin         GPIO_PIN_7
#define PU_L_GPIO_Port   GPIOA
#define PV_L_Pin         GPIO_PIN_0
#define PV_L_GPIO_Port   GPIOB
#define PW_L_Pin         GPIO_PIN_1
#define PW_L_GPIO_Port   GPIOB
#define PU_H_Pin         GPIO_PIN_9
#define PU_H_GPIO_Port   GPIOE
#define PV_H_Pin         GPIO_PIN_11
#define PV_H_GPIO_Port   GPIOE
#define PW_H_Pin         GPIO_PIN_13
#define PW_H_GPIO_Port   GPIOE
#define HALL_V_Pin       GPIO_PIN_7
#define HALL_V_GPIO_Port GPIOC
#define HALL_W_Pin       GPIO_PIN_8
#define HALL_W_GPIO_Port GPIOC
#define OUT1_Pin         GPIO_PIN_0
#define OUT1_GPIO_Port   GPIOE
#define OUT2_Pin         GPIO_PIN_1
#define OUT2_GPIO_Port   GPIOE

/* USER CODE BEGIN Private defines */
typedef enum {
    IDLE,
    INIT,
    START,
    RUN,
    STOP,
    BRAKE,
    WAIT,
    FAULT
} SystStatus_t;

#define U8_MAX  ((uint8_t)255)
#define S8_MAX  ((int8_t)127)
#define S8_MIN  ((int8_t) - 128)
#define U16_MAX ((uint16_t)65535u)
#define S16_MAX ((int16_t)32767)
#define S16_MIN ((int16_t) - 32768)
#define U32_MAX ((uint32_t)4294967295uL)
#define S32_MAX ((int32_t)2147483647)
#define S32_MIN ((int32_t) - 2147483648)

extern volatile SystStatus_t State;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
