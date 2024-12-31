#ifndef _BSP_MATH_H_
#define _BSP_MATH_H_

#include <math.h>
#include "stm32f4xx.h"
#include "main.h"

typedef struct {
    float dt;                        // time step
    float lpf_parm;                  // LPF parameter
    float value_lpf, value_lpf_prev; // LPF variables
} lpf_filter;

extern float Limit_up_and_down(float input, float limit_down, float limit_up);
extern float Low_Pass_Filter(lpf_filter *lpf_t);

#endif /* _BSP_MATH_H_ */

