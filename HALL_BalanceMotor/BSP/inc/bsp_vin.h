//
// Created by Hacker on 2024/4/29.
//

#ifndef HALL_BALANCEMOTOR_BSP_VIN_H
#define HALL_BALANCEMOTOR_BSP_VIN_H

#include "main.h"

enum
{
    IS_FULL = 1,
    NEED_CHARGE,
    NEED_POWEROFF
};

extern void VIN_Init(void);
extern void init_vrefint_reciprocal(void);
extern float get_temprate(void);
extern float get_battery_voltage(void);

extern uint16_t adcx_get_chx_value(ADC_HandleTypeDef *ADCx, uint32_t ch);
extern float adcx_get_chx_value_average(ADC_HandleTypeDef *ADCx, uint32_t ch, uint8_t times);


#endif //HALL_BALANCEMOTOR_BSP_VIN_H
