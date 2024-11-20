#ifndef BSP_USART_H
#define BSP_USART_H

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

void USART_Init(void);

#endif
