/**
 ****************************************************************************************************
 * @file        atim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-19
 * @brief       高级定时器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F407电机开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211019
 * 第一次发布
 *
 ****************************************************************************************************
 */


#include "./BSP/TIMER/atim.h"
#include "./BSP/LED/led.h"

/******************************************************************************************/
/* 定时器配置句柄 定义 */

/* 高级定时器PWM */
TIM_HandleTypeDef g_atimx_pwm_chy_handle;     /* 定时器x句柄 */
TIM_OC_InitTypeDef g_atimx_oc_pwm_chy_handle; /* 定时器输出句柄 */

/* g_npwm_remain表示当前还剩下多少个脉冲要发送
 * 每次最多发送256个脉冲
 */
static uint32_t g_npwm_remain = 0;
/******************************************************************************************/
/* HAL 通用回调接口函数 */

/**
 * @brief       高级定时器TIMX NPWM中断服务函数
 * @param       无
 * @retval      无
 */
void ATIM_TIMX_NPWM_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_atimx_pwm_chy_handle); /* 定时器共用处理函数 */
}

/**
 * @brief       定时器更新中断回调函数
 * @param        htim:定时器句柄指针
 * @note        此函数会被定时器中断函数共同调用的
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    uint16_t npwm = 0;

    if (htim == (&g_atimx_pwm_chy_handle))  /* 高级定时器中断实验回调执行的内容 */
    {
        if (g_npwm_remain > 256)            /* 还有大于256个脉冲需要发送 */
        {
            g_npwm_remain = g_npwm_remain - 256;
            npwm = 256;
        }
        else if (g_npwm_remain % 256)       /* 还有位数（不到256）个脉冲要发送 */
        {
            npwm = g_npwm_remain % 256;
            g_npwm_remain = 0;              /* 没有脉冲了 */
        }

        if (npwm)   /* 有脉冲要发送 */
        {
            g_atimx_pwm_chy_handle.Instance->RCR = npwm - 1;                        /* 设置重复计数寄存器值为npwm-1, 即npwm个脉冲 */
            HAL_TIM_GenerateEvent(&g_atimx_pwm_chy_handle, TIM_EVENTSOURCE_UPDATE); /* 产生一次更新事件,在中断里面处理脉冲输出 */
            __HAL_TIM_ENABLE(&g_atimx_pwm_chy_handle);                              /* 使能定时器TIMX */
        }
        else
        {
            g_atimx_pwm_chy_handle.Instance->CR1 &= ~(1 << 0); /* 关闭定时器TIMX，使用HAL Disable会清除PWM通道信息，此处不用 */
        }
    }
}

/**
 * @brief       高级定时器TIMX 通道Y 输出指定个数PWM 初始化函数
 * @note
 *              高级定时器的时钟来自APB2, 而PCLK2 = 168Mhz, 我们设置PPRE2不分频, 因此
 *              高级定时器时钟 = 168Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void atim_timx_npwm_chy_init(uint16_t arr, uint16_t psc)
{
    ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE();   /* TIMX 通道IO口时钟使能 */
    ATIM_TIMX_NPWM_CHY_CLK_ENABLE();        /* TIMX 时钟使能 */

    g_atimx_pwm_chy_handle.Instance = ATIM_TIMX_NPWM;                   /* 定时器x */
    g_atimx_pwm_chy_handle.Init.Prescaler = psc;                        /* 定时器分频 */
    g_atimx_pwm_chy_handle.Init.CounterMode = TIM_COUNTERMODE_DOWN;     /* 向下计数模式 */
    g_atimx_pwm_chy_handle.Init.Period = arr;                           /* 自动重装载值 */
    g_atimx_pwm_chy_handle.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;   /* 分频因子 */
    g_atimx_pwm_chy_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /*使能TIMx_ARR进行缓冲*/
    g_atimx_pwm_chy_handle.Init.RepetitionCounter = 0;                  /* 开始时不计数*/
    HAL_TIM_PWM_Init(&g_atimx_pwm_chy_handle);                          /* 初始化PWM */

    g_atimx_oc_pwm_chy_handle.OCMode = TIM_OCMODE_PWM1;                 /* 模式选择PWM1 */
    g_atimx_oc_pwm_chy_handle.Pulse = arr/2;
    g_atimx_oc_pwm_chy_handle.OCPolarity = TIM_OCPOLARITY_HIGH;          /* 输出比较极性为低 */
    HAL_TIM_PWM_ConfigChannel(&g_atimx_pwm_chy_handle, &g_atimx_oc_pwm_chy_handle, ATIM_TIMX_NPWM_CHY); /* 配置TIMx通道y */
    HAL_TIM_PWM_Start(&g_atimx_pwm_chy_handle, ATIM_TIMX_NPWM_CHY);     /* 开启对应PWM通道 */
    __HAL_TIM_CLEAR_IT(&g_atimx_pwm_chy_handle, TIM_IT_UPDATE);
    __HAL_TIM_ENABLE_IT(&g_atimx_pwm_chy_handle, TIM_IT_UPDATE);        /* 允许更新中断 */

    HAL_NVIC_SetPriority(ATIM_TIMX_NPWM_IRQn, 1, 3); /* 设置中断优先级，抢占优先级1，子优先级3 */
    HAL_NVIC_EnableIRQ(ATIM_TIMX_NPWM_IRQn);         /* 开启ITMx中断 */
}


/**
 * @brief       定时器底层驱动，时钟使能，引脚配置
                此函数会被HAL_TIM_PWM_Init()调用
 * @param       htim:定时器句柄
 * @retval      无
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == ATIM_TIMX_NPWM)
    {
        GPIO_InitTypeDef gpio_init_struct;
        ATIM_TIMX_NPWM_CHY_GPIO_CLK_ENABLE(); /* 开启通道y的CPIO时钟 */
        ATIM_TIMX_NPWM_CHY_CLK_ENABLE();

        gpio_init_struct.Pin = ATIM_TIMX_NPWM_CHY_GPIO_PIN;         /* 通道y的CPIO口 */
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;                    /* 复用推完输出 */
        gpio_init_struct.Pull = GPIO_PULLUP;                        /* 上拉 */
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* 高速 */
        gpio_init_struct.Alternate = ATIM_TIMX_NPWM_CHY_GPIO_AF;    /* 端口复用 */
        HAL_GPIO_Init(ATIM_TIMX_NPWM_CHY_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief       高级定时器TIMX NPWM设置PWM个数
 * @param       rcr: PWM的个数, 1~2^32次方个
 * @retval      无
 */
void atim_timx_npwm_chy_set(uint32_t npwm)
{
    if (npwm == 0)
        return;

    g_npwm_remain = npwm;                                                   /* 保存脉冲个数 */
    HAL_TIM_GenerateEvent(&g_atimx_pwm_chy_handle, TIM_EVENTSOURCE_UPDATE); /* 产生一次更新事件,在中断里面处理脉冲输出 */
    __HAL_TIM_ENABLE(&g_atimx_pwm_chy_handle);                              /* 使能定时器TIMX */
}
