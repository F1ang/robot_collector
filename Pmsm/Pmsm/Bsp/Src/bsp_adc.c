#include "bsp_adc.h"

uint32_t adc_inter_cnt = 0;

/* ADC偏移值 */
uint16_t hPhaseAOffset;
uint16_t hPhaseBOffset;
uint16_t hPhaseCOffset;

/**
 * @brief: 获取原始线电流
 * @param {foc_handler} *foc_data
 * @return {*}
 */
void Get_ADC_Current(foc_handler *foc_data)
{
    foc_data->adc_t.Ia_Raw = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1); // 获取注入通道值
    foc_data->adc_t.Ib_Raw = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
    foc_data->adc_t.Ic_Raw = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
}

/**
 * @brief: 相电流
 * @param {foc_handler} *foc_data
 * @return {*} : None
 */
void Get_Ia_Ib(foc_handler *foc_data)
{
    if (foc_data->sector == 0)
        return;
    switch (foc_data->sector) {
    case 4:
    case 5: // Current on Phase C not accessible
        foc_data->ia =
            (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) / 32768 * 3.3f;
        foc_data->ib =
            (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2) / 32768 * 3.3f;
        foc_data->ic = -foc_data->ia - foc_data->ib;
        break;

    case 6:
    case 1: // Current on Phase A not accessible
        foc_data->ib =
            (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2) / 32768 * 3.3f;
        foc_data->ic =
            (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3) / 32768 * 3.3f;
        foc_data->ia = -foc_data->ib - foc_data->ic;
        break;

    case 2:
    case 3: // Current on Phase B not accessible
        foc_data->ia =
            (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) / 32768 * 3.3f;
        foc_data->ic =
            (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3) / 32768 * 3.3f;
        foc_data->ib = -foc_data->ia - foc_data->ic;
        break;
    default:
        foc_data->ia = 0;
        foc_data->ib = 0;
        foc_data->ic = 0;
        break;
    }
    return;
}

/**
 * @brief: 开启JEOC中断
 * @return {*}
 */
void SVPWM_InjectedConvConfig(void)
{
    /* ADC1 Injected conversions trigger is TIM1 TRGO */
    ADC_InjectionConfTypeDef sConfigInjected;

    // 配置注入通道触发器
    sConfigInjected.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJECCONV_T1_TRGO; // 明确指定触发源为 TIM1 TRGO
    if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK) {
        Error_Handler();
    }
    HAL_ADCEx_InjectedStart_IT(&hadc1); // JEOC中断开启
}

/**
 * @brief: ADC的offset、开启JEOC中断
 * @return {*} : None
 */
void SVPWM_3ShuntCurrentReadingCalibration(void)
{
    static uint16_t bIndex;
    ADC_InjectionConfTypeDef sConfigInjected;

    /* ADC1 Injected group of conversions end interrupt disabling */
    HAL_ADCEx_InjectedStop_IT(&hadc1); // 确保中断已停止

    hPhaseAOffset = 0;
    hPhaseBOffset = 0;
    hPhaseCOffset = 0;

    /* ADC1 Injected conversions trigger is given by software and enabled */
    sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START; // 明确指定触发源为软件触发
    if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK) {
        Error_Handler();
    }

    /* Clear the ADC1 JEOC pending flag */
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_JEOC);

    HAL_ADCEx_InjectedStart(&hadc1); // 开启注入通道转换

    for (bIndex = 0; bIndex < NB_CONVERSIONS; bIndex++) {
        while (!__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_JEOC)) {
        }

        hPhaseAOffset += (HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) >> 3);
        hPhaseBOffset += (HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2) >> 3);
        hPhaseCOffset += (HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3) >> 3);

        /* Clear the ADC1 JEOC pending flag */
        __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_JEOC);

        /* Start the ADC1 Injected Conversion */
        HAL_ADCEx_InjectedStart(&hadc1);
    }

    SVPWM_InjectedConvConfig(); // ADC通道和IT的触发方式
}

/**
 * @brief:触发ADC中断的外部事件
 * @param {FunctionalState} cmd : 开启或关闭
 * @return {*}
 */
void SVPWM_3ShuntAdvCurrentReading(FunctionalState cmd)
{
    if (cmd == ENABLE) {
        // ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_CC4);
        ADC1->CR2 |= 0x00001000; // ADC1触发源 外部触发=CC4

        __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
        HAL_TIM_Base_Start_IT(&htim1); // CC4比较事件触发ADC1采集电流,update重启ADC1 JEOC中断
    } else {
        HAL_TIM_Base_Stop_IT(&htim1);

        // ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);
        ADC1->CR2 &= 0xFFFFEFFF;

        // ReEnable EXT. ADC Triggering
        ADC1->CR2 |= 0x00008000;
    }
}

/**
 * @brief: ADC中断处理函数(高频任务)
 * @param {ADC_HandleTypeDef} *hadc
 * @return {*} : None
 */
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) { // 根据ADC实例判断
        if (adc_inter_cnt < 0xFFFFFFFF)
            adc_inter_cnt++;

        if (State == START || State == RUN)
            HAL_ADCEx_InjectedStop_IT(&hadc1);

        switch (State) {
        case START: // 2,外同步加速
            STO_Start_Up();
            break;

        case RUN: // 3,运行
            FOC_Control(&foc_data_handler);
            break;

        default:
            break;
        }
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
    }
}
