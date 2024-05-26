#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "main.h"


#define     LED1_STA(i)     if (i)  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); \
                                else HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

#define     LED2_STA(i)     if (i)  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); \
                                else HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);


#endif

