//
// Created by Hacker on 2024/4/28.
//

#ifndef HALL_BALANCEMOTOR_BSP_ENCODER_H
#define HALL_BALANCEMOTOR_BSP_ENCODER_H

#include "main.h"

#define ENCODER_MID_VALUE 30000

extern void Encoder_Init(void);
extern void Encoder_Set_Counter(int8_t Motor_Num, int16_t count);
extern short Encoder_Get_Counter(int8_t Motor_Num);
extern uint16_t Encoder_Get_Dir(int8_t Motor_Num);

#endif //HALL_BALANCEMOTOR_BSP_ENCODER_H
