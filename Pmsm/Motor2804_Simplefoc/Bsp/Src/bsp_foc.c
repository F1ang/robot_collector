/*
 * @Welcome: Hacker
 * @Description: Excellent Day
 */
#include "bsp_foc.h"
#include "bsp_as5600.h"

foc_callbak foc_func[FOC_FUNC_NUM]; // 回调函数数组

//--------------------------------------------------------------
/* FOC变换函数接口 */
static void Clarke_Transform(foc_handler *foc_data);
static void Clarke_Inv_Transform(foc_handler *foc_data);
static void Park_Transform(foc_handler *foc_data);
static void Park_Inv_Transform(foc_handler *foc_data);

foc_callbak foc_transform[] = {
    [CLARKE_TRANSFORM] = Clarke_Transform,
    [CLARKE_INVERSE_TRANSFORM] = Clarke_Inv_Transform,
    [PARK_TRANSFORM] = Park_Transform,
    [PARK_INVERSE_TRANSFORM] = Park_Inv_Transform,
};

// clark变换: i_abc->i_alphab, i_beta
static void Clarke_Transform(foc_handler *foc_data)
{
    /* Calculate pIalpha using the equation, pIalpha = Ia */
    foc_data->i_aphla = foc_data->ia;
    /* Calculate pIbeta using the equation, pIbeta = (1/sqrt(3)) * Ia + (2/sqrt(3)) * Ib */
    foc_data->i_bphla = ((float)_1_SQRT3 * foc_data->ia + (float)_2_SQRT3 * foc_data->ib);
}

// park变换: i_alphab, i_beta->id, iq
static void Park_Transform(foc_handler *foc_data)
{
    float cosVal = 0, sinVal = 0;
    cosVal = arm_cos_f32(foc_data->elec_angle);
    sinVal = arm_sin_f32(foc_data->elec_angle);

    /* Calculate pId using the equation, pId = Ialpha * cosVal + Ibeta * sinVal */
    foc_data->id = foc_data->i_aphla * cosVal + foc_data->i_bphla * sinVal;

    /* Calculate pIq using the equation, pIq = - Ialpha * sinVal + Ibeta * cosVal */
    foc_data->iq = -foc_data->i_aphla * sinVal + foc_data->i_bphla * cosVal;
}

// park逆变换: uq, ud->u_alphab, u_beta
static void Park_Inv_Transform(foc_handler *foc_data)
{
    float cosVal = 0, sinVal = 0;
    cosVal = arm_cos_f32(foc_data->elec_angle);
    sinVal = arm_sin_f32(foc_data->elec_angle);

    foc_data->u_alpha = -foc_data->uq * sinVal;
    foc_data->u_beta = foc_data->uq * cosVal;
}

// clark逆变换: u_abc<-u_alphab, u_beta->计算扇区及作用时间
static void Clarke_Inv_Transform(foc_handler *foc_data)
{
    foc_data->ua = foc_data->u_alpha + foc_data->voltage_power_offset / 2;
    foc_data->ub = -0.5f * foc_data->u_alpha + SQRT3_2 * foc_data->u_beta + foc_data->voltage_power_offset / 2;
    foc_data->uc = -0.5f * foc_data->u_alpha - SQRT3_2 * foc_data->u_beta + foc_data->voltage_power_offset / 2;
}
//--------------------------------------------------------------

// SVPWM控制函数接口
void Set_SVPWM(foc_handler *foc_data)
{
    /* Load compare registers values */
    TIM1->CCR1 = foc_data->ua / VOLTAGE_POWER_LIMIT * 5250;
    TIM1->CCR2 = foc_data->ub / VOLTAGE_POWER_LIMIT * 5250;
    TIM1->CCR3 = foc_data->uc / VOLTAGE_POWER_LIMIT * 5250;
    // TIM1->CCR4 = 0; // To Syncronyze the ADC
}

// TIM PWM初始化
void TIM1_PWM_Init(void)
{
    // HAL_TIM_Base_Start(&htim1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); // 启动 PWM 通道1 互补信号输出
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); // 启动 PWM 通道2 互补信号输出
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3); // 启动 PWM 通道1 互补信号输出
    __HAL_TIM_MOE_ENABLE(&htim1);
    HAL_TIM_Base_Start(&htim1);
}

// 获取电角度
void Get_Elec_Angle(foc_handler *foc_data)
{
    foc_data->angle = bsp_as5600GetAngle();
    foc_data->elec_angle = PAIRS_OF_POLES * foc_data->angle;
}

void GET_Speed(foc_handler *foc_data)
{
    float dt = 0;
    dt = DWT_GetDeltaT(&(foc_data->delt_dt));

    foc_data->angle = bsp_as5600GetAngle();
    foc_data->speed = (foc_data->angle - foc_data->angle_last) / dt;
    foc_data->angle_last = foc_data->angle;
}

/**
 * @brief:FOC的功能注册函数接口
 * @param {foc_register_func_list} id : 函数enum
 * @param {foc_callbak} func : 函数回调
 * @return {*} : none
 */
void foc_register_func(foc_register_func_list id, foc_callbak func)
{
    foc_func[id] = func;
    return;
}

// 速度开环 input:uq=x ud=0
void open_loop_speed_control(foc_handler *foc_data)
{
//	  foc_data->uq = 4;
//		foc_data->ud = 0;
	
    // electrical angle
    Get_Elec_Angle(foc_data);

    // transform
    foc_transform[PARK_INVERSE_TRANSFORM](foc_data);   // park逆变换 uq,ud->u_alphab,u_beta
    foc_transform[CLARKE_INVERSE_TRANSFORM](foc_data); // clark逆变换 u_alphab,u_beta->u_abc

    // ua ub uc
    Set_SVPWM(foc_data); // SVPWM控制函数接口
}
