#include "bsp_adc.h"

bsp_adc_t bsp_adc;

// 获取线电流
void Get_ADC_Current(foc_handler *foc_data)
{
    bsp_adc.Ia = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) / 32768 * 3.3f; // 获取注入通道值
    bsp_adc.Ib = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2) / 32768 * 3.3f;
    bsp_adc.Ic = -(bsp_adc.Ia + bsp_adc.Ib);

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
// 电流环控制
void close_loop_curr_control(foc_handler *foc_data)
{
    // get ia ib ic
    Get_ADC_Current(foc_data);

    // electrical angle
    Get_Elec_Angle(foc_data);

    // transform
    foc_transform[CLARKE_TRANSFORM](foc_data); // clark变换 ia,ib,ic->i_alphab,i_beta
    foc_transform[PARK_TRANSFORM](foc_data);   // park逆变换 i_alphab, i_beta->id, iq

    // PI control
    CurrentLoop_Control(foc_data);

    // transform
    foc_data->uq = Limit_up_and_down(foc_data->uq, -5.0f, 5.0f);
    foc_data->ud = 0;

    foc_transform[PARK_INVERSE_TRANSFORM](foc_data);   // park逆变换 uq,ud->u_alphab,u_beta
    foc_transform[CLARKE_INVERSE_TRANSFORM](foc_data); // clark逆变换 u_alphab,u_beta->u_abc

    // ua ub uc
    Set_SPWM(foc_data); // SVPWM控制函数接口
}

// ADC中断处理函数
uint32_t adc_inter_cnt = 0;
extern foc_handler foc_data_handler;
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) // 根据ADC实例判断
    {
        if (adc_inter_cnt < 0xFFFFFFFF)
            adc_inter_cnt++;
        HAL_ADCEx_InjectedStop_IT(&hadc1);
        Get_ADC_Current(&foc_data_handler);
        SVPWM_Control(&foc_data_handler);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
    }
}
