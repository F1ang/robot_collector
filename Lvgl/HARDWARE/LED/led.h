#ifndef _LED_H
#define _LED_H
#include "sys.h"

#define LEDR(i) if (i == 1) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); \
                    else HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);

#define LEDG(i) if (i == 1) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); \
                    else HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);   	
                    
#define LEDB(i) if (i == 1) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); \
                    else HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);	 	

#define LEDR_TOGGLE HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5)

#define LEDG_TOGGLE HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0)

#define LEDB_TOGGLE HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1)

void LED_Init(void);
#endif
