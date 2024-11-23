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

// 返回(limit_down,limit_up)间值
static float Limit_up_and_down(float input, float limit_down, float limit_up)
{
    float output = 0;
    output = (input < limit_down) ? limit_down : ((input > limit_up) ? limit_up : input);
    return output;
}

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
    foc_data->ua = Limit_up_and_down(foc_data->ua, 0, 10.0f);
    foc_data->ub = Limit_up_and_down(foc_data->ub, 0, 10.0f);
    foc_data->uc = Limit_up_and_down(foc_data->uc, 0, 10.0f);
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

// 一阶线性低通滤波
static float Low_Pass_Filter(foc_handler *foc_data, float dt)
{
    float lpf_speed = 0;
    lpf_speed = (foc_data->speed_lpf_a * foc_data->speed_last) + (1.0f - foc_data->speed_lpf_a) * foc_data->speed;
    foc_data->speed_last = lpf_speed;
    return lpf_speed;
}

// 获取电角度
void Get_Elec_Angle(foc_handler *foc_data)
{
    float angle_delta = 0, angle_raw = 0;
    // 总机械角度
    foc_data->angle = bsp_as5600GetAngle();
    foc_data->angle_cal = DEG2RAD(foc_data->angle); // 总电机转角(rad)
    angle_delta = DEG2RAD(foc_data->angle - foc_data->angle_last - foc_data->angle_offset);
    foc_data->angle_last = foc_data->angle;

    // 归一化角度(0~2pi)
    foc_data->angle_norm_deg = fmod((foc_data->angle - foc_data->angle_offset), 360.0f); // 归一化机械角度(°)
    foc_data->angle_norm_rad = DEG2RAD(foc_data->angle_norm_deg);

    foc_data->elec_angle = PAIRS_OF_POLES * foc_data->angle;
}

void GET_Speed(foc_handler *foc_data)
{
    float dt = 0;
    dt = DWT_GetDeltaT(&(foc_data->delt_dt));

    foc_data->angle = bsp_as5600GetAngle();
    foc_data->speed = (foc_data->angle - foc_data->angle_last) / dt;
    foc_data->angle_last = foc_data->angle;

    foc_data->speed = Low_Pass_Filter(foc_data, dt); // 一阶线性低通滤波
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
    // electrical angle
    Get_Elec_Angle(foc_data);

    // control
    Position_Control(foc_data);

    // transform
    foc_data->uq = Limit_up_and_down(foc_data->uq, -5.0f, 5.0f);
    foc_data->ud = 0;

    foc_transform[PARK_INVERSE_TRANSFORM](foc_data);   // park逆变换 uq,ud->u_alphab,u_beta
    foc_transform[CLARKE_INVERSE_TRANSFORM](foc_data); // clark逆变换 u_alphab,u_beta->u_abc

    // ua ub uc
    Set_SVPWM(foc_data); // SVPWM控制函数接口
}

// 预定位(开环强拖)
void Start_Up(foc_handler *foc_data)
{
    foc_data->uq = START_UP_UQ; // 开环拖动theta=0,Ud=a,Uq=0
    foc_data->ud = 0.0f;
    foc_data->elec_angle = 0.0f;
    foc_transform[PARK_INVERSE_TRANSFORM](foc_data);
    foc_transform[CLARKE_INVERSE_TRANSFORM](foc_data);
    Set_SVPWM(foc_data); // SVPWM控制函数接口
    HAL_Delay(3000);

    foc_data->angle_offset = bsp_as5600GetAngle();
    HAL_Delay(20);

    foc_data->uq = 0;
    foc_data->ud = 0.0f;
    foc_data->elec_angle = 0.0f;
    foc_transform[PARK_INVERSE_TRANSFORM](foc_data);
    foc_transform[CLARKE_INVERSE_TRANSFORM](foc_data);
    foc_data->m_mode = MOTOR_RUN;
}

// 位置环(rad)
void Position_Control(foc_handler *foc_data)
{
    //foc_data->pos_loop.kp = 1.0f;
    foc_data->pos_loop.real_pos = foc_data->angle_norm_deg;
    foc_data->pos_loop.error_pos = foc_data->pos_loop.target_pos - foc_data->pos_loop.real_pos;
    foc_data->uq = foc_data->pos_loop.kp * foc_data->pos_loop.error_pos;
}