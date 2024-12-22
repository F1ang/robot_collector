#include "bsp_adc.h"
#include "bsp_hall.h"

bsp_adc_t bsp_adc;

// 获取线电流
void Get_ADC_Current(foc_handler *foc_data)
{
    bsp_adc.Ia = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) / 32768 * 3.3f; // 获取注入通道值
    bsp_adc.Ib = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2) / 32768 * 3.3f;
    bsp_adc.Ic = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3) / 32768 * 3.3f;

    foc_data->ia = bsp_adc.Ia;
    foc_data->ib = bsp_adc.Ib;
    foc_data->ic = bsp_adc.Ic;
}

// 得到电流计算出的实际uq
static void CurrentLoop_Control(foc_handler *foc_data)
{
    // bsp_adc.iq_ref = 0; // 电流力矩控制ref参数
    bsp_adc.iq_meas = foc_data->iq;

    bsp_adc.iq_err = bsp_adc.iq_ref - bsp_adc.iq_meas;
    bsp_adc.iq_err_sum += bsp_adc.iq_err;
    bsp_adc.iq_pi_out = bsp_adc.kp * bsp_adc.iq_err + bsp_adc.ki * bsp_adc.iq_err_sum;

    foc_data->uq = bsp_adc.iq_pi_out; // meas uq
}

// ADC中断处理函数(高频任务)
uint32_t adc_inter_cnt = 0;
extern foc_handler foc_data_handler;
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) { // 根据ADC实例判断
        if (adc_inter_cnt < 0xFFFFFFFF)
            adc_inter_cnt++;
        HAL_ADCEx_InjectedStop_IT(&hadc1);
        // Get_ADC_Current(&foc_data_handler);
        FOC_Control(&foc_data_handler);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
    }
}
