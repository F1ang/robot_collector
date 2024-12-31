#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include "adc.h"
#include "bsp_foc.h"
#include "main.h"
#include "stm32f4xx_hal_adc_ex.h"

#define NB_CONVERSIONS 16

extern void SVPWM_3ShuntCurrentReadingCalibration(void);
extern void SVPWM_3ShuntAdvCurrentReading(FunctionalState cmd);

#endif // _BSP_ADC_H_
