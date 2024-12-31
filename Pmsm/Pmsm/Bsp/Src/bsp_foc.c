#include "bsp_foc.h"

uint32_t tim1_uptate_over = 0, tim6_uptate_over = 0, tim3_uptate_over = 0; // 溢出计数
foc_callbak foc_func[FOC_FUNC_NUM];                                        // 回调函数数组
foc_handler foc_data_handler;

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

/**
 * @brief: TIM PWM初始化、ADC采样触发
 * @return {*} : None
 */
void TIM1_PWM_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); // 启动 PWM 通道1 互补信号输出
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); // 启动 PWM 通道2 互补信号输出
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3); // 启动 PWM 通道1 互补信号输出

    HAL_TIM_Base_Start_IT(&htim6); // speed calculate

    // __HAL_TIM_MOE_ENABLE(&htim1);
    // HAL_TIM_Base_Start_IT(&htim1); // update interrupt event(ENABLE ADC JEOC)

    // // CC4触发ADC采样
    // HAL_ADCEx_InjectedStart(&hadc1); // 注入通道启动
    // HAL_ADCEx_InjectedStart_IT(&hadc1);
}

/**
 * @brief: 控制环参数初始化
 * @return {*} : None
 */
void FOC_Init(void)
{
    // 转矩开环
    foc_data_handler.id = 0;
    foc_data_handler.iq = 0;
    foc_data_handler.torque_loop.target_torque = 0;
    foc_data_handler.flux_loop.target_flux = 0;

    // 速度开环
    foc_data_handler.ud = 0;
    foc_data_handler.uq = 0;
}

/**
 * @brief:FOC的功能注册函数接口
 * @param {foc_register_func_list} id : 函数enum
 * @param {foc_callbak} func : 函数回调
 * @return {*} : None
 */
void foc_register_func(foc_register_func_list id, foc_callbak func)
{
    foc_func[id] = func;
    return;
}

/**
 * @brief: clark变换: i_abc->i_alphab, i_beta
 * @param {foc_handler} *foc_data
 * @return {*} : None
 */
static void Clarke_Transform(foc_handler *foc_data)
{
    /* Calculate pIalpha using the equation, pIalpha = Ia */
    foc_data->i_aphla = foc_data->ia;
    /* Calculate pIbeta using the equation, pIbeta = (1/sqrt(3)) * Ia + (2/sqrt(3)) * Ib */
    foc_data->i_bphla = ((float)_1_SQRT3 * foc_data->ia + (float)_2_SQRT3 * foc_data->ib);
}

/**
 * @brief: park变换: i_alphab, i_beta->id, iq
 * @param {foc_handler} *foc_data
 * @return {*} : None
 */
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

/**
 * @brief: park逆变换: uq, ud->u_alphab, u_beta
 * @param {foc_handler} *foc_data
 * @return {*} : None
 */
static void Park_Inv_Transform(foc_handler *foc_data)
{
    float cosVal = 0, sinVal = 0;
    cosVal = arm_cos_f32(foc_data->elec_angle);
    sinVal = arm_sin_f32(foc_data->elec_angle);

    foc_data->u_alpha = foc_data->ud * cosVal - foc_data->uq * sinVal;
    foc_data->u_beta = foc_data->ud * sinVal + foc_data->uq * cosVal;
}

/**
 * @brief: clark逆变换: u_abc<-u_alphab, u_beta->计算扇区及作用时间
 * @param {foc_handler} *foc_data
 * @return {*} : None
 */
static void Clarke_Inv_Transform(foc_handler *foc_data)
{
    foc_data->ua = foc_data->u_alpha;
    foc_data->ub = -0.5f * foc_data->u_alpha + SQRT3_2 * foc_data->u_beta;
    foc_data->uc = -0.5f * foc_data->u_alpha - SQRT3_2 * foc_data->u_beta;
}

/**
 * @brief: SVPWM Control
 * @param {foc_handler} *foc_data
 * @param {float} T
 * @param {float} Ts_pwn : 单路PWM周期
 * @param {float} Udc_tem : 母线电压
 * @return {*} : None
 */
void Foc_Svpwm(foc_handler *foc_data, float Ts_pwn, float Udc_tem)
{
    float U1 = 0, U2 = 0, U3 = 0;
    uint8_t A = 0, B = 0, C = 0, N = 0, sector = 0;

    float Tx = 0, Ty = 0, T0 = 0, Ta = 0, Tb = 0, Tc = 0;
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
    Ta = T0 / 2; // 最先作用的PWM对的时刻(需要按不同扇区分配)
    Tb = Ta + Tx / 2;
    Tc = Tb + Ty / 2;

    /* 3、SVPWM控制 */
    switch (sector) {
    case 1:
        Tcmp1 = Ta; // 第一个PWM=CCR1对应的 导通时刻(!!!PWM1!!!),扇区1对应CCR1=Ta
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
    TIM1->CCR4 = 5250 - DEADTIME_NS;
}

/**
 * @brief: 过调制
 * @param {foc_handler} *foc_data
 * @return {*} : None
 */
void Circle_Limitation(foc_handler *foc_data)
{
    float V_temp = 0, temp = 0;

    V_temp = foc_data->uq * foc_data->uq + foc_data->ud * foc_data->ud;
    if (V_temp > R2 * R2) { // 过调制
        foc_data->uq = foc_data->uq * R2 / (V_temp / 32768);
        foc_data->ud = foc_data->ud * R2 / (V_temp / 32768);
    }
}

/**
 * @brief: FOC控制(高频任务)
 * @param {foc_handler} *foc_data
 * @return {*} : None
 */
void FOC_Control(foc_handler *foc_data)
{
    // electrical angle -- hall->dpp
    HALL_IncElectricalAngle(); // 电角度dpp

    foc_data->elec_angle = HALL_GetElectricalAngle() * K_CON;

    // 开环
    foc_data->uq = 15.0f;
    foc_data->ud = 0.0f;

    // inv park ud,uq->u_alpha,u_beta
    foc_transform[PARK_INVERSE_TRANSFORM](foc_data);

    // SVPWM
    Foc_Svpwm(foc_data, PWM_FREQ, 23.0f);
}

/* TIM Update event */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1) {
        if (tim1_uptate_over < 0xFFFFFFFF)
            tim1_uptate_over++;
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);

        __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_JEOC);
        HAL_ADCEx_InjectedStart_IT(&hadc1);
    }

    if (htim->Instance == TIM6) {
        if (tim6_uptate_over < 0xFFFFFFFF)
            tim6_uptate_over++;
        // HALL_ClearRotorFreq_Dpp();
    }

    if (htim->Instance == TIM3) { // HALL捕获定时器
        if (tim3_uptate_over < 0xFFFFFFFF) {
            tim3_uptate_over++;
        }

        // an update event occured for this interrupt request generation
        if (bGP1_OVF_Counter < U8_MAX) {
            bGP1_OVF_Counter++;
        }

        if (bGP1_OVF_Counter >= HALL_MAX_OVERFLOWS) {
            HallTimeOut = TRUE;
            HALL_ClearRotorFreq_Dpp();
        }
    }
}
