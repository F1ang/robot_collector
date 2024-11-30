#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include "main.h"
#include "adc.h"
#include "bsp_foc.h"

typedef struct
{
    float iq_ref, id_ref, iq_meas, id_meas;
    float Ia, Ib, Ic;
    float Ua, Ub, Uc;
    float iq_err, id_err, iq_err_last, id_err_last, iq_err_sum;
    float iq_pi_out;

    // PID parameters
    float kp, ki, kd;
} bsp_adc_t;

extern bsp_adc_t bsp_adc;
extern void Get_ADC_Current(foc_handler *foc_data);
extern void close_loop_curr_control(foc_handler *foc_data);

#endif // _BSP_ADC_H_
