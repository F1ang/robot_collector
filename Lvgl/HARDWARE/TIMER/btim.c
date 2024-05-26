#include "led.h"
#include "btim.h"
#include "lvgl.h"


TIM_HandleTypeDef g_timx_handle;      /* ��ʱ��x��� */

/**
 * @brief       ������ʱ��TIMX��ʱ�жϳ�ʼ������
 * @note
 *              ������ʱ����ʱ������APB1,��PPRE1 �� 2��Ƶ��ʱ��
 *              ������ʱ����ʱ��ΪAPB1ʱ�ӵ�2��, ��APB1Ϊ168, ���Զ�ʱ��ʱ�� = 84Mhz
 *              ��ʱ�����ʱ����㷽��: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=��ʱ������Ƶ��,��λ:Mhz
 *
 * @param       arr: �Զ���װֵ��
 * @param       psc: ʱ��Ԥ��Ƶ��
 * @retval      ��
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    BTIM_TIMX_INT_CLK_ENABLE();                                         /* ʹ��TIMxʱ�� */
    
    g_timx_handle.Instance = BTIM_TIMX_INT;                             /* ͨ�ö�ʱ��x */
    g_timx_handle.Init.Prescaler = psc;                                 /* ��Ƶ */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                /* ���ϼ����� */
    g_timx_handle.Init.Period = arr;                                    /* �Զ�װ��ֵ */
    g_timx_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;          /* ʱ�ӷ�Ƶ���� */
    HAL_TIM_Base_Init(&g_timx_handle);
    
    HAL_NVIC_SetPriority(BTIM_TIMX_INT_IRQn, 1, 3);                     /* �����ж����ȼ�����ռ���ȼ�1�������ȼ�3 */
    HAL_NVIC_EnableIRQ(BTIM_TIMX_INT_IRQn);                             /* ����ITMx�ж� */
    
    HAL_TIM_Base_Start_IT(&g_timx_handle);                              /* ʹ�ܶ�ʱ��x�Ͷ�ʱ��x�����ж� */
    
    
}

/**
 * @brief       ��ʱ���жϷ�����
 * @param       ��
 * @retval      ��
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_handle);
}

/**
* @brief       ��ʱ�������жϻص�����
* @param        htim:��ʱ�����ָ��
* @retval      ��
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
   if (htim == (&g_timx_handle))
   {
       lv_tick_inc(1);   /*lvgl��1ms����*/
   }
}









