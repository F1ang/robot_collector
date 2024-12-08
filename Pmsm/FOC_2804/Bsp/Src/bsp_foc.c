/*
 * @Welcome: Hacker
 * @Description: Excellent Day
 */
#include "bsp_foc.h"
#include "bsp_adc.h"
#include "bsp_as5600.h"

foc_callbak foc_func[FOC_FUNC_NUM]; // 回调函数数组

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

//--------------------------------------------------------------
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

// static void Clarke_Inv_Transform(foc_handler *foc_data)
// {
//     foc_data->ua = foc_data->u_alpha;
//     foc_data->ub = -0.5f * foc_data->u_alpha + SQRT3_2 * foc_data->u_beta;
//     foc_data->uc = -0.5f * foc_data->u_alpha - SQRT3_2 * foc_data->u_beta;
// }

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
//--------------------------------------------------------------

//--------------------------------------------------------------
// 返回(limit_down,limit_up)间值
float Limit_up_and_down(float input, float limit_down, float limit_up)
{
    float output = 0;
    output = (input < limit_down) ? limit_down : ((input > limit_up) ? limit_up : input);
    return output;
}

// 一阶线性低通滤波
static float Low_Pass_Filter(foc_handler *foc_data, float dt)
{
    float lpf_speed = 0;
    foc_data->speed_lpf_a = 0.8;
    lpf_speed = (foc_data->speed_lpf_a * foc_data->speed_last) + (1.0f - foc_data->speed_lpf_a) * foc_data->speed;
    foc_data->speed_last = lpf_speed;
    return lpf_speed;
}
//--------------------------------------------------------------

//--------------------------------------------------------------
// TIM PWM初始化、ADC采样触发
void TIM1_PWM_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); // 启动 PWM 通道1 互补信号输出
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); // 启动 PWM 通道2 互补信号输出
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3); // 启动 PWM 通道1 互补信号输出

    __HAL_TIM_MOE_ENABLE(&htim1);
    Start_Up(&foc_data_handler);

    HAL_TIM_Base_Start_IT(&htim1); // update interrupt event
    HAL_TIM_Base_Start_IT(&htim6);

    // CC4触发ADC采样
    HAL_ADCEx_InjectedStart(&hadc1); // 注入通道启动
    HAL_ADCEx_InjectedStart_IT(&hadc1);
}
//--------------------------------------------------------------

//--------------------------------------------------------------
// 获取电角度
void Get_Elec_Angle(foc_handler *foc_data)
{
    // 总机械角度
    foc_data->angle = bsp_as5600GetAngle();
    foc_data->angle_cal = DEG2RAD(foc_data->angle); // 总电机转角(rad)
    // foc_data->angle_delta = DEG2RAD(foc_data->angle - foc_data->angle_last - foc_data->angle_offset);
    // foc_data->angle_last = foc_data->angle;

    // 归一化角度(0~2pi)
    foc_data->angle_norm_deg = fmod((foc_data->angle - foc_data->angle_offset), 360.0f); // 归一化机械角度(°)
    foc_data->angle_norm_rad = DEG2RAD(foc_data->angle_norm_deg);

    foc_data->elec_angle = PAIRS_OF_POLES * foc_data->angle;
}

// 获取速度
void GET_Speed(foc_handler *foc_data)
{
    float dt = 0, angle_speed = 0;
    dt = DWT_GetDeltaT(&(foc_data->delt_dt));

    angle_speed = bsp_as5600GetAngle() - foc_data->angle_offset;
    foc_data->angle_delta = DEG2RAD(angle_speed) - DEG2RAD(foc_data->angle_last); // 分辨率低,存在不可避免偏差
    foc_data->speed = (foc_data->angle_delta / dt) * 10 / 4;
    foc_data->angle_last = angle_speed;

    // foc_data->speed = Low_Pass_Filter(foc_data, dt); // 一阶线性低通滤波
}

// SPWM控制函数接口
void Set_SPWM(foc_handler *foc_data)
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
//--------------------------------------------------------------

//--------------------------------------------------------------
// 预定位(开环强拖)
void Start_Up(foc_handler *foc_data)
{
    foc_data->uq = START_UP_UQ; // 开环拖动theta=0,Ud=a,Uq=0
    foc_data->ud = 0.0f;
    foc_data->elec_angle = 0.0f;
    foc_transform[PARK_INVERSE_TRANSFORM](foc_data);
    foc_transform[CLARKE_INVERSE_TRANSFORM](foc_data);
    Set_SPWM(foc_data); // SVPWM控制函数接口
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

// 速度开环 input:uq=x ud=0
void open_loop_speed_control(foc_handler *foc_data)
{
    // electrical angle
    Get_Elec_Angle(foc_data);

    // control loop
    // Position_Control(foc_data);
    Speed_Control(foc_data); // 速度环(rad/s)

    // transform
    foc_data->uq = Limit_up_and_down(foc_data->uq, -5.0f, 5.0f);
    foc_data->ud = 0;

    foc_transform[PARK_INVERSE_TRANSFORM](foc_data);   // park逆变换 uq,ud->u_alphab,u_beta
    foc_transform[CLARKE_INVERSE_TRANSFORM](foc_data); // clark逆变换 u_alphab,u_beta->u_abc

    // ua ub uc
    Set_SPWM(foc_data); // SVPWM控制函数接口
}

// 位置环(rad)
void Position_Control(foc_handler *foc_data)
{
    // foc_data->pos_loop.kp = 1.0f;
    foc_data->pos_loop.real_pos = foc_data->angle_norm_deg;
    foc_data->pos_loop.error_pos = foc_data->pos_loop.target_pos - foc_data->pos_loop.real_pos;
    foc_data->uq = foc_data->pos_loop.kp * foc_data->pos_loop.error_pos;

    // 双环控制接口
    // foc_data->speed_loop.target_speed = foc_data->pos_loop.kp * foc_data->pos_loop.error_pos;
}

// 速度环(rad/s)
void Speed_Control(foc_handler *foc_data)
{
    // foc_data->speed_loop.kp = 0.8f; // 速度环增益(单位rad/s)
    foc_data->speed_loop.real_speed = foc_data->speed;
    foc_data->speed_loop.error_speed = foc_data->speed_loop.target_speed - foc_data->speed_loop.real_speed;
    foc_data->uq = foc_data->speed_loop.kp * foc_data->speed_loop.error_speed;
}
//--------------------------------------------------------------

//--------------------------------------------------------------
/**
 * @brief:
 * @param {foc_handler} *foc_data
 * @param {float} T
 * @param {float} Ts_pwn : 单路PWM周期
 * @param {float} Udc_tem : 母线电压
 * @return {*} : none
 */
float Tx = 0, Ty = 0, T0 = 0, Ta = 0, Tb = 0, Tc = 0;
void Foc_Svpwm(foc_handler *foc_data, float Ts_pwn, float Udc_tem)
{
    float U1 = 0, U2 = 0, U3 = 0;
    uint8_t A = 0, B = 0, C = 0, N = 0, sector = 0;

    //    float Tx = 0, Ty = 0, T0 = 0, Ta = 0, Tb = 0, Tc = 0;
    float K = SQRT3 * Ts_pwn / Udc_tem; // 公共乘积因子
    uint16_t Tcmp1 = 0, Tcmp2 = 0, Tcmp3 = 0, Tcmp4 = 0;
    uint16_t hDeltaDuty = 0;

    /* 1、扇区判断 */
    U1 = foc_data->u_beta;
    U2 = SQRT3_2 * foc_data->u_alpha - foc_data->u_beta / 2;
    U3 = -SQRT3_2 * foc_data->u_alpha - foc_data->u_beta / 2;

    A = (U1 > 0) ? 1 : 0;
    B = (U2 > 0) ? 1 : 0;
    C = (U3 > 0) ? 1 : 0;
    N = 4 * C + 2 * B + A;

    switch (N) {
    case 3:
        sector = 1;
        break;
    case 1:
        sector = 2;
        break;
    case 5:
        sector = 3;
        break;
    case 4:
        sector = 4;
        break;
    case 6:
        sector = 5;
        break;
    case 2:
        sector = 6;
        break;
    default:
        break;
    }
    foc_data->sector = sector;

    /* 2、矢量作用总时间 */
    switch (sector) {
    case 1:
        Tx = K * U2; // 7段式SVPWM中 U4先作用,后U6
        Ty = K * U1;
        break;
    case 2:
        Tx = -K * U2;
        Ty = -K * U3;
        break;
    case 3:
        Tx = K * U1;
        Ty = K * U3;
        break;
    case 4:
        Tx = -K * U1;
        Ty = -K * U2;
        break;
    case 5:
        Tx = K * U3;
        Ty = K * U2;
        break;
    case 6:
        Tx = -K * U3;
        Ty = -K * U1;
        break;
    default:
        break;
    }

    // 过调制处理
    if ((Tx + Ty) > Ts_pwn) {
        Tx = Tx * Ts_pwn / (Tx + Ty);
        Ty = Ty * Ts_pwn / (Tx + Ty);
    }

    // 7段式-CCR分配导通时间
    T0 = (Ts_pwn - Tx - Ty) / 2;
    Ta = T0 / 2;
    Tb = Ta + Tx / 2;
    Tc = Tb + Ty / 2;

    /* 3、SVPWM控制 */
    switch (sector) {
    case 1:
        Tcmp1 = Ta; // 第一个PWM=CCR1对应的 导通时刻(!!!PWM1!!!),扇区1对应Ta
        Tcmp2 = Tb;
        Tcmp3 = Tc;

        // 1、下桥臂导通时间长的相,CCR4下一轮触发ADC采样
        // 2、扇区1,因A相上桥臂导通时间长(即下桥臂导通时间短),故不在A相采样

        // 3、A上桥臂导通时刻Tcmp1+死区DT+开关延迟TN(时间轴)->定采样点与PWM_PERIOD关系->B、C相
        // 4、中心对称,即周期(计数)取 PWM_FREQ 的一半
        // 5、注:CCR1~CCR3用PWM1  CCR4用PWM2
        // 6、A B C三相存在相互滞后/超前,u16强制转化
        // 7、PWM1与PWM2反向,利用PWM1:>CCRx,输出低,下桥臂导通,触发采样.   核心:上下桥臂反向
        // 反之上桥臂最快导通的时刻为Ta(7段式特性=此相导通最长),以最长导通的中心时刻分析ADC采样点
        if ((uint16_t)(PWM_PERIOD - Tcmp1) > TW_AFTER) // 情况1
            Tcmp4 = PWM_PERIOD - 1;                    // CCR4中心对齐模式!!!PWM2!!!
        else {
            // 已无法在PWM_PERIOD中心点采样,A导通只能前后采样
            hDeltaDuty = (uint16_t)(Tcmp1 - Tcmp2);              // A B相在扇区1,上桥臂均有导通时间
            if (hDeltaDuty > (uint16_t)(PWM_PERIOD - Tcmp1) * 2) // 情况3:B相上桥臂先导通于A相,在A相导通前Ts采样
            {
                // B超前A(即C也超前A),触发在A导通前采样(即A相下桥臂导通前)
                Tcmp4 = Tcmp1 - TW_BEFORE; // Ts before Phase A
            } else {
                // (PWM_PERIOD-Tcmp1)*2,足够采样
                Tcmp4 = Tcmp1 + TW_AFTER; // 情况2:A相上桥臂导通后,B、C相上桥臂导通前采样
                if (Tcmp4 >= PWM_PERIOD)  // 情况4:不可在A下桥臂导通或者之前采样
                {
                    // PWM4Direction = PWM1_MODE;  // 降低PWM频率(提高占空比)
                    Tcmp4 = (2 * PWM_PERIOD) - Tcmp4 - 1; // 可以最终TIM反极性输出
                }
            }
        }

        /* 使能ADC注入中断 */

        break;

    case 2:
        Tcmp1 = Tb; // 上桥臂功率管导通周期  扇区矢量作用时间 CCR1
        Tcmp2 = Ta;
        Tcmp3 = Tc;

        // 扇区2,不在B相采样(B上桥臂导通最长)
        if ((uint16_t)(PWM_PERIOD - Tcmp2) > TW_AFTER) // 情况1
            Tcmp4 = PWM_PERIOD - 1;                    // CCR4中心对齐模式PWM2
        else {
            // 已无法在PWM_PERIOD中心点采样,B导通只能前后采样
            hDeltaDuty = (uint16_t)(Tcmp2 - Tcmp1);
            if (hDeltaDuty > (uint16_t)(PWM_PERIOD - Tcmp2) * 2) // 情况3
            {
                Tcmp4 = Tcmp2 - TW_BEFORE; // Ts before Phase B
            } else {
                Tcmp4 = Tcmp2 + TW_AFTER; // 情况2   DT + Tn after Phase B
                if (Tcmp4 >= PWM_PERIOD)  // 情况4(不可在A下桥臂导通或者之前采样)
                {
                    // PWM4Direction = PWM1_MODE;  // 降低PWM频率(提高占空比)
                    Tcmp4 = (2 * PWM_PERIOD) - Tcmp4 - 1; // 可以最终TIM反极性输出
                }
            }
        }
        break;

    case 3:
        Tcmp1 = Tc; // 上桥臂功率管导通周期  扇区矢量作用时间 CCR1
        Tcmp2 = Ta;
        Tcmp3 = Tb;

        // 扇区3,不在B相采样(B上桥臂导通最长)
        if ((uint16_t)(PWM_PERIOD - Tcmp2) > TW_AFTER) // 情况1
            Tcmp4 = PWM_PERIOD - 1;                    // CCR4中心对齐模式PWM2
        else {
            // 已无法在PWM_PERIOD中心点采样,B导通只能前后采样
            hDeltaDuty = Tcmp2 - Tcmp3;
            if (hDeltaDuty > (uint16_t)(PWM_PERIOD - Tcmp2) * 2) // 情况3
            {
                Tcmp4 = Tcmp2 - TW_BEFORE; // Ts before Phase B
            } else {
                Tcmp4 = Tcmp2 + TW_AFTER; // 情况2  DT + Tn after Phase B
                if (Tcmp4 >= PWM_PERIOD)  // 情况4(不可在A下桥臂导通或者之前采样)
                {
                    // PWM4Direction = PWM1_MODE;  // 降低PWM频率(提高占空比)
                    Tcmp4 = (2 * PWM_PERIOD) - Tcmp4 - 1; // 可以最终TIM反极性输出
                }
            }
        }
        break;

    case 4:
        Tcmp1 = Tc; // 上桥臂功率管导通周期  扇区矢量作用时间 CCR1
        Tcmp2 = Tb;
        Tcmp3 = Ta;

        // 扇区4,不在C相采样(C上桥臂导通最长)
        if ((uint16_t)(PWM_PERIOD - Tcmp3) > TW_AFTER) // 情况1
            Tcmp4 = PWM_PERIOD - 1;                    // CCR4中心对齐模式PWM2
        else {
            // 已无法在PWM_PERIOD中心点采样,C导通只能前后采样
            hDeltaDuty = Tcmp3 - Tcmp2;
            if (hDeltaDuty > (uint16_t)(PWM_PERIOD - Tcmp3) * 2) // 情况3
            {
                Tcmp4 = Tcmp3 - TW_BEFORE; // Ts before Phase C
            } else {
                Tcmp4 = Tcmp3 + TW_AFTER; // 情况2   DT + Tn after Phase C
                if (Tcmp4 >= PWM_PERIOD)  // 情况4(不可在A下桥臂导通或者之前采样)
                {
                    // PWM4Direction = PWM1_MODE;  // 降低PWM频率(提高占空比)
                    Tcmp4 = (2 * PWM_PERIOD) - Tcmp4 - 1; // 可以最终TIM反极性输出
                }
            }
        }
        break;

    case 5:
        Tcmp1 = Tb; // 上桥臂功率管导通周期  扇区矢量作用时间 CCR1
        Tcmp2 = Tc;
        Tcmp3 = Ta;

        // 扇区5,不在C相采样(C上桥臂导通最长)
        if ((uint16_t)(PWM_PERIOD - Tcmp3) > TW_AFTER) // 情况1
            Tcmp4 = PWM_PERIOD - 1;                    // CCR4中心对齐模式PWM2
        else {
            // 已无法在PWM_PERIOD中心点采样,B导通只能前后采样
            hDeltaDuty = Tcmp3 - Tcmp1;
            if (hDeltaDuty > (uint16_t)(PWM_PERIOD - Tcmp3) * 2) // 情况3
            {
                Tcmp4 = Tcmp3 - TW_BEFORE; // Ts before Phase C
            } else {
                Tcmp4 = Tcmp3 + TW_AFTER; // 情况2  DT + Tn after Phase C
                if (Tcmp4 >= PWM_PERIOD)  // 情况4(不可在A下桥臂导通或者之前采样)
                {
                    // PWM4Direction = PWM1_MODE;  // 降低PWM频率(提高占空比)
                    Tcmp4 = (2 * PWM_PERIOD) - Tcmp4 - 1; // 可以最终TIM反极性输出
                }
            }
        }
        break;

    case 6:
        Tcmp1 = Ta; // 上桥臂功率管导通周期  扇区矢量作用时间 CCR1
        Tcmp2 = Tc;
        Tcmp3 = Tb;

        // 扇区5,不在A相采样(A上桥臂导通最长)
        if ((uint16_t)(PWM_PERIOD - Tcmp1) > TW_AFTER) // 情况1
            Tcmp4 = PWM_PERIOD - 1;                    // CCR4中心对齐模式PWM2
        else {
            // 已无法在PWM_PERIOD中心点采样,C导通只能前后采样
            hDeltaDuty = Tcmp1 - Tcmp3;
            if (hDeltaDuty > (uint16_t)(PWM_PERIOD - Tcmp1) * 2) // 情况3
            {
                Tcmp4 = Tcmp1 - TW_BEFORE; // Ts before Phase A
            } else {
                Tcmp4 = Tcmp1 + TW_AFTER; // 情况2 DT + Tn after Phase A
                if (Tcmp4 >= PWM_PERIOD)  // 情况4(不可在A下桥臂导通或者之前采样)
                {
                    // PWM4Direction = PWM1_MODE;  // 降低PWM频率(提高占空比)
                    Tcmp4 = (2 * PWM_PERIOD) - Tcmp4 - 1; // 可以最终TIM反极性输出
                }
            }
        }
        break;
    default:
        break;
    }

    TIM1->CCR1 = Tcmp1;
    TIM1->CCR2 = Tcmp2;
    TIM1->CCR3 = Tcmp3;
    TIM1->CCR4 = 5250 / 2;
}

// SVPWM控制
void SVPWM_Control(foc_handler *foc_data)
{
    // electrical angle
    Get_Elec_Angle(foc_data);

    // park逆变换 uq,ud->u_alphab,u_beta
    foc_transform[PARK_INVERSE_TRANSFORM](foc_data);
    // clark逆变换 u_alphab,u_beta->u_abc
    // foc_transform[CLARKE_INVERSE_TRANSFORM](foc_data);

    // SVPWM
    Foc_Svpwm(foc_data, PWM_FREQ, 11.0f);
}

// 相电流
void Get_Ia_Ib(foc_handler *foc_data)
{
    if (foc_data->sector == 0)
        return;
    switch (foc_data->sector) {
    case 4:
    case 5: // Current on Phase C not accessible
        foc_data->ia = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) / 32768 * 3.3f;
        foc_data->ib = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2) / 32768 * 3.3f;
        foc_data->ic = -foc_data->ia - foc_data->ib;
        break;

    case 6:
    case 1: // Current on Phase A not accessible
        foc_data->ib = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2) / 32768 * 3.3f;
        foc_data->ic = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3) / 32768 * 3.3f;
        foc_data->ia = -foc_data->ib - foc_data->ic;
        break;

    case 2:
    case 3: // Current on Phase B not accessible
        foc_data->ia = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1) / 32768 * 3.3f;
        foc_data->ic = (float)HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3) / 32768 * 3.3f;
        foc_data->ib = -foc_data->ia - foc_data->ic;
        break;
    default:
        foc_data->ia = 0;
        foc_data->ib = 0;
        foc_data->ic = 0;
        break;
    }
}

// 转矩环(电流环)
void Current_Control(foc_handler *foc_data)
{
    float error_id = 0, error_iq = 0;
    foc_data->tq_loop.kp = 2;
    foc_data->tq_loop.target_torque = 2;

    foc_data->tq_loop.real_torque = foc_data->iq;

    error_iq = foc_data->tq_loop.target_torque - foc_data->tq_loop.real_torque;
    foc_data->uq = foc_data->tq_loop.kp * error_iq;

    error_id = foc_data->tq_loop.target_flux - foc_data->tq_loop.real_flux;
    foc_data->ud = foc_data->tq_loop.kp_flux * error_id;
}

// 过调制
#define MMI 0.95
#define R1  32768
#define R2  MMI *R1
void Circle_Limitation(foc_handler *foc_data)
{
    float V_temp = 0, temp = 0;

    V_temp = foc_data->uq * foc_data->uq + foc_data->ud * foc_data->ud;
    if (V_temp > R2 * R2) { // 过调制
        foc_data->uq = foc_data->uq * R2 / (V_temp / 32768);
        foc_data->ud = foc_data->ud * R2 / (V_temp / 32768);
    }
}

// SVPWM
void SVPWM(foc_handler *foc_data, float Ts)
{
    float sum;
    float k_svpwm, u1, u2, u3;
    float t1, t2, t3, t4, t5, t6, t7, ta, tb, tc;

    // step1 计算u1、u2和u3
    // 计算SVPWM算法中的三个控制电压u1、u2和u3
    u1 = foc_data->u_beta;
    u2 = -0.8660254f * foc_data->u_alpha - 0.5f * foc_data->u_beta; // sqrt(3)/2 ≈ 0.86603
    u3 = 0.8660254f * foc_data->u_alpha - 0.5f * foc_data->u_beta;
    // step2：扇区判断
    // 根据u1、u2和u3的正负情况确定所处的扇区
    foc_data->sector = (u1 > 0.0f) + ((u2 > 0.0f) << 1) + ((u3 > 0.0f) << 2); // N=4*C+2*B+A

    // step3:计算基本矢量电压作用时间（占空比）
    // 根据扇区的不同，计算对应的t_a、t_b和t_c的值，表示生成的三相电压的时间
    switch (foc_data->sector) {
    case 5:
        // 扇区5
        t4 = u3;
        t6 = u1;
        sum = t4 + t6;
        if (sum > Ts) {
            k_svpwm = Ts / sum; //
            t4 = k_svpwm * t4;
            t6 = k_svpwm * t6;
        }
        t7 = (Ts - t4 - t6) / 2;
        ta = t4 + t6 + t7;
        tb = t6 + t7;
        tc = t7;
        break;
    case 1:
        // 扇区1
        t2 = -u3;
        t6 = -u2;
        sum = t2 + t6;
        if (sum > Ts) {
            k_svpwm = Ts / sum; // 计算缩放系数
            t2 = k_svpwm * t2;
            t6 = k_svpwm * t6;
        }
        t7 = (Ts - t2 - t6) / 2;
        ta = t6 + t7;
        tb = t2 + t6 + t7;
        tc = t7;
        break;
    case 3:
        // 扇区3
        t2 = u1;
        t3 = u2;
        sum = t2 + t3;
        if (sum > Ts) {
            k_svpwm = Ts / sum; //
            t2 = k_svpwm * t2;
            t3 = k_svpwm * t3;
        }
        t7 = (Ts - t2 - t3) / 2;
        ta = t7;
        tb = t2 + t3 + t7;
        tc = t3 + t7;
        break;

    case 2:
        // 扇区2
        t1 = -u1;
        t3 = -u3;
        sum = t1 + t3;
        if (sum > Ts) {
            k_svpwm = Ts / sum; //
            t1 = k_svpwm * t1;
            t3 = k_svpwm * t3;
        }
        t7 = (Ts - t1 - t3) / 2;
        ta = t7;
        tb = t3 + t7;
        tc = t1 + t3 + t7;
        break;

    case 6:
        // 扇区6
        t1 = u2;
        t5 = u3;
        sum = t1 + t5;
        if (sum > Ts) {
            k_svpwm = Ts / sum; //
            t1 = k_svpwm * t1;
            t5 = k_svpwm * t5;
        }
        t7 = (Ts - t1 - t5) / 2;
        ta = t5 + t7;
        tb = t7;
        tc = t1 + t5 + t7;
        break;

    case 4:
        // 扇区4
        t4 = -u2;
        t5 = -u1;
        sum = t4 + t5;
        if (sum > Ts) {
            k_svpwm = Ts / sum; //
            t4 = k_svpwm * t4;
            t5 = k_svpwm * t5;
        }
        t7 = (Ts - t4 - t5) / 2;
        ta = t4 + t5 + t7;
        tb = t7;
        tc = t5 + t7;
        break;

    default:
        break;
    }

    // step4：6路PWM输出
    TIM1->CCR1 = PWM_PERIOD * ta;
    TIM1->CCR2 = PWM_PERIOD * tb;
    TIM1->CCR3 = PWM_PERIOD * tc;
    TIM1->CCR4 = 5250 / 2;
}

// FOC控制
void FOC_Control(foc_handler *foc_data)
{
    // electrical angle -- hall->dpp
    Get_Elec_Angle(foc_data);

    // ia ib
    // Get_Ia_Ib(foc_data);

    // // clark ia、ib->i_alpha,i_beta
    // foc_transform[CLARKE_TRANSFORM](foc_data);
    // // Park i_alpha,i_beta->i_q i_d
    // foc_transform[PARK_TRANSFORM](foc_data);

    // // uq、ud
    // Current_Control(foc_data);

    // // circle limitation
    // Circle_Limitation(foc_data);

    // 开环
    foc_data->uq = 2.0f;
    foc_data->ud = 0.0f;

    // inv park ud,uq->u_alpha,u_beta
    foc_transform[PARK_INVERSE_TRANSFORM](foc_data);

    // SVPWM
    Foc_Svpwm(foc_data, PWM_FREQ, 11.0f);
    // SVPWM(foc_data, 1.0f);
}

/* TIM Update event */
uint32_t tim1_uptate_over = 0, tim6_uptate_over = 0; // 溢出计数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1) {
        if (tim1_uptate_over < 0xFFFFFFFF)
            tim1_uptate_over++;
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_ADCEx_InjectedStart_IT(&hadc1);
    }
    if (htim->Instance == TIM6) {
        if (tim6_uptate_over < 0xFFFFFFFF)
            tim6_uptate_over++;
    }
}