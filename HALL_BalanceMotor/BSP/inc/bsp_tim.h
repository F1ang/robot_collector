//
// Created by Hacker on 2024/5/5.
//

#ifndef HALL_BALANCEMOTOR_BSP_TIM_H
#define HALL_BALANCEMOTOR_BSP_TIM_H

#include "main.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

extern void timx_init(void);
extern void timx_updata_callback(TIM_HandleTypeDef *htim);

#endif //HALL_BALANCEMOTOR_BSP_TIM_H
