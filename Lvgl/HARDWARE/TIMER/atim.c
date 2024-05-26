/**
 ****************************************************************************************************
 * @file        atim.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-19
 * @brief       �߼���ʱ�� ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F407���������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211019
 * ��һ�η���
 *
 ****************************************************************************************************
 */


#include "./BSP/TIMER/atim.h"
#include "./BSP/LED/led.h"

/******************************************************************************************/
/* ��ʱ�����þ�� ���� */

/* �߼���ʱ��PWM */
TIM_HandleTypeDef g_atimx_pwm_chy_handle;     /* ��ʱ��x��� */
TIM_OC_InitTypeDef g_atimx_oc_pwm_chy_handle; /* ��ʱ�������� */

/* g_npwm_remain��ʾ��ǰ��ʣ�¶��ٸ�����Ҫ����
 * ÿ����෢��256������
 */
static uint32_t g_npwm_remain = 0;
/******************************************************************************************/
/* HAL ͨ�ûص��ӿں��� */

/**
 * @brief       �߼���ʱ��TIMX NPWM�жϷ�����
 * @param       ��
 * @retval      ��
 */
void ATIM_TIMX_NPWM_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_atimx_pwm_chy_handle); /* ��ʱ�����ô����� */
}

/**
 * @brief       ��ʱ�������жϻص�����
 * @param        htim:��ʱ�����ָ��
 * @note        �˺����ᱻ��ʱ���жϺ�����ͬ���õ�
 * @retval      ��
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    uint16_t npwm = 0;

    if (htim == (&g_atimx_pwm_chy_handle))  /* �߼���ʱ���ж�ʵ��ص�ִ�е����� */
    {
        if (g_npwm_remain > 256)            /* ���д���256��������Ҫ���� */
        {
            g_npwm_remain = g_npwm_remain - 256;
            npwm = 256;
        }
        else if (g_npwm_remain % 256)       /* ����λ��������256��������Ҫ���� */
        {
            npwm = g_npwm_remain % 256;
            g_npwm_remain = 0;              /* û�������� */
        }

        if (npwm)   /* ������Ҫ���� */
        {
            g_atimx_pwm_chy_handle.Instance->RCR = npwm - 1;                        /* �����ظ������Ĵ���ֵΪnpwm-1, ��npwm������ */
            HAL_TIM_GenerateEvent(&g_atimx_pwm_chy_handle, TIM_EVENTSOURCE_UPDATE); /* ����һ�θ����¼�,���ж����洦��������� */
            __HAL_TIM_ENABLE(&g_atimx_pwm_chy_handle);                              /* ʹ�ܶ�ʱ��TIMX */
        }
        else
        {
            g_atimx_pwm_chy_handle.Instance->CR1 &= ~(1 << 0); /* �رն�ʱ��TIMX��ʹ��HAL Disable�����PWMͨ����Ϣ���˴����� */
        }
    }
}

/**
 * @brief       �߼���ʱ��TIMX ͨ��Y ���ָ������PWM ��ʼ������
 * @note
 *              �߼���ʱ����ʱ������APB2, ��PCLK2 = 168Mhz, ��������PPRE2����Ƶ, ���
 *              �߼���ʱ��ʱ�� = 168Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void atim_timx_npwm_chy_init(uint16_t arr, uint16_t psc)
{
    ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE();   /* TIMX ͨ��IO��ʱ��ʹ�� */
    ATIM_TIMX_NPWM_CHY_CLK_ENABLE();        /* TIMX ʱ��ʹ�� */

    g_atimx_pwm_chy_handle.Instance = ATIM_TIMX_NPWM;                   /* ��ʱ��x */
    g_atimx_pwm_chy_handle.Init.Prescaler = psc;                        /* ��ʱ����Ƶ */
    g_atimx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_DOWN;     /* ���¼���ģʽ */
    g_atimx_pwm_chy_handle.Init.Period = arr;                           /* �Զ���װ��ֵ */
    g_atimx_pwm_chy_handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   /* ��Ƶ���� */
    g_atimx_pwm_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /*ʹ��TIMx_ARR���л���*/
    g_atimx_pwm_chy_handle.Init.RepetitionCounter = 0;                  /* ��ʼʱ������*/
    HAL_TIM_PWM_Init(&g_atimx_pwm_chy_handle);                          /* ��ʼ��PWM */

    g_atimx_oc_pwm_chy_handle.OCMode = TIM_OCMODE_PWM1;                 /* ģʽѡ��PWM1 */
    g_atimx_oc_pwm_chy_handle.Pulse = arr/2;
    g_atimx_oc_pwm_chy_handle.OCPolarity = TIM_OCPOLARITY_HIGH;          /* ����Ƚϼ���Ϊ�� */
    HAL_TIM_PWM_ConfigChannel(&g_atimx_pwm_chy_handle, &g_atimx_oc_pwm_chy_handle, ATIM_TIMX_NPWM_CHY); /* ����TIMxͨ��y */
    HAL_TIM_PWM_Start(&g_atimx_pwm_chy_handle, ATIM_TIMX_NPWM_CHY);     /* ������ӦPWMͨ�� */
    __HAL_TIM_CLEAR_IT(&g_atimx_pwm_chy_handle, TIM_IT_UPDATE);
    __HAL_TIM_ENABLE_IT(&g_atimx_pwm_chy_handle, TIM_IT_UPDATE);        /* ��������ж� */

    HAL_NVIC_SetPriority(ATIM_TIMX_NPWM_IRQn, 1, 3); /* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */
    HAL_NVIC_EnableIRQ(ATIM_TIMX_NPWM_IRQn);         /* ����ITMx�ж� */
}


/**
 * @brief       ��ʱ���ײ�������ʱ��ʹ�ܣ���������
                �˺����ᱻHAL_TIM_PWM_Init()����
 * @param       htim:��ʱ�����
 * @retval      ��
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == ATIM_TIMX_NPWM)
    {
        GPIO_InitTypeDef gpio_init_struct;
        ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE(); /* ����ͨ��y��CPIOʱ�� */
        ATIM_TIMX_NPWM_CHY_CLK_ENABLE();

        gpio_init_struct.Pin = ATIM_TIMX_NPWM_CHY_GPIO_PIN;         /* ͨ��y��CPIO�� */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                    /* ����������� */
        gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
        gpio_init_struct.Alternate = ATIM_TIMX_NPWM_CHY_GPIO_AF;    /* �˿ڸ��� */
        HAL_GPIO_Init(ATIM_TIMX_NPWM_CHY_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief       �߼���ʱ��TIMX NPWM����PWM����
 * @param       rcr: PWM�ĸ���, 1~2^32�η���
 * @retval      ��
 */
void atim_timx_npwm_chy_set(uint32_t npwm)
{
    if (npwm == 0)
        return;

    g_npwm_remain = npwm;                                                   /* ����������� */
    HAL_TIM_GenerateEvent(&g_atimx_pwm_chy_handle, TIM_EVENTSOURCE_UPDATE); /* ����һ�θ����¼�,���ж����洦��������� */
    __HAL_TIM_ENABLE(&g_atimx_pwm_chy_handle);                              /* ʹ�ܶ�ʱ��TIMX */
}
