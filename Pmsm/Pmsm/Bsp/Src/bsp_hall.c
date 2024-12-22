#include "bsp_hall.h"
#include "bsp_foc.h"
#include "tim.h"

void BSP_Hall_Init(void)
{
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1); // TIM_IT_CC1
    HAL_TIM_Base_Start_IT(&htim3);              // TIM_IT_UPDATE
}

/* TIM3 CC1 Capture Interrupt  */
uint8_t bHallState = 0, bPrevHallState = 0;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        /* 读取HALL状态 */
        bPrevHallState = bHallState;
        bHallState = HALL_U_VALUE << 2 | HALL_V_VALUE << 1 | HALL_W_VALUE << 0;

        /* 计数捕获 */
        foc_data_handler.hall_ops.overcount = 0; // DEBUG
        foc_data_handler.hall_ops.hall_cap =
            (foc_data_handler.hall_ops.overcount * 0x10000L) +
            HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1); // 需考虑溢出
        foc_data_handler.hall_ops.hall_psc = htim->Instance->PSC;

        /* 调整psc */
        // if (foc_data_handler.hall_ops.overcount > 0) { // 速度过慢,预分频过低
        //     foc_data_handler.hall_ops.hall_psc = htim->Instance->PSC + 1;
        //     if (foc_data_handler.hall_ops.RatioInc != 0) { // 增加预分频
        //         foc_data_handler.hall_ops.overcount = 0;
        //         foc_data_handler.hall_ops.RatioInc = 0;
        //     } else {
        //         if (htim->Instance->PSC < 65535) {
        //             __HAL_TIM_SET_PRESCALER(htim, htim->Instance->PSC + 1);
        //             foc_data_handler.hall_ops.RatioInc = 1;
        //         }
        //     }
        // } else {                                           //
        // 速度过快,预分频过高
        //     if (foc_data_handler.hall_ops.RatioDec != 0) { // 增加预分频
        //         foc_data_handler.hall_ops.hall_psc = htim->Instance->PSC - 2;
        //         foc_data_handler.hall_ops.RatioDec = 0;
        //     } else {
        //         foc_data_handler.hall_ops.hall_psc = htim->Instance->PSC - 1;
        //         if (htim->Instance->PSC > 0) {
        //             __HAL_TIM_SET_PRESCALER(htim, htim->Instance->PSC - 1);
        //             foc_data_handler.hall_ops.RatioDec = 1;
        //         }
        //     }
        // }

        /* 电角度校准 */
        switch (bHallState) {
        case HALL_5:
            if (bPrevHallState == HALL_1) {
                foc_data_handler.hall_ops.dir = MOTOR_CW;
                foc_data_handler.hall_ops.refer_theta = S16_240_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_240_PHASE_SHIFT;
            } else if (bPrevHallState == HALL_4) {
                foc_data_handler.hall_ops.dir = MOTOR_CCW;
                foc_data_handler.hall_ops.refer_theta = S16_300_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_300_PHASE_SHIFT;
            }
            break;

        case HALL_1:
            if (bPrevHallState == HALL_3) {
                foc_data_handler.hall_ops.dir = MOTOR_CW;
                foc_data_handler.hall_ops.refer_theta = S16_180_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_180_PHASE_SHIFT;
            } else if (bPrevHallState == HALL_5) {
                foc_data_handler.hall_ops.dir = MOTOR_CCW;
                foc_data_handler.hall_ops.refer_theta = S16_240_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_240_PHASE_SHIFT;
            }
            break;

        case HALL_3:
            if (bPrevHallState == HALL_2) {
                foc_data_handler.hall_ops.dir = MOTOR_CW;
                foc_data_handler.hall_ops.refer_theta = S16_120_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_120_PHASE_SHIFT;
            } else if (bPrevHallState == HALL_1) {
                foc_data_handler.hall_ops.dir = MOTOR_CCW;
                foc_data_handler.hall_ops.refer_theta = S16_180_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_180_PHASE_SHIFT;
            }
            break;

        case HALL_2:
            if (bPrevHallState == HALL_6) {
                foc_data_handler.hall_ops.dir = MOTOR_CW;
                foc_data_handler.hall_ops.refer_theta = S16_60_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_60_PHASE_SHIFT;
            } else if (bPrevHallState == HALL_3) {
                foc_data_handler.hall_ops.dir = MOTOR_CCW;
                foc_data_handler.hall_ops.refer_theta = S16_120_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_120_PHASE_SHIFT;
            }
            break;

        case HALL_6:
            if (bPrevHallState == HALL_4) {
                foc_data_handler.hall_ops.dir = MOTOR_CW;
                foc_data_handler.hall_ops.refer_theta = S16_0_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_0_PHASE_SHIFT;
            } else if (bPrevHallState == HALL_2) {
                foc_data_handler.hall_ops.dir = MOTOR_CCW;
                foc_data_handler.hall_ops.refer_theta = S16_60_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_60_PHASE_SHIFT;
            }
            break;

        case HALL_4:
            if (bPrevHallState == HALL_5) {
                foc_data_handler.hall_ops.dir = MOTOR_CW;
                foc_data_handler.hall_ops.refer_theta = S16_300_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_300_PHASE_SHIFT;
            } else if (bPrevHallState == HALL_6) {
                foc_data_handler.hall_ops.dir = MOTOR_CCW;
                foc_data_handler.hall_ops.refer_theta = S16_0_PHASE_SHIFT;
                foc_data_handler.hall_ops.hall_theta = S16_0_PHASE_SHIFT;
            }
            break;

        default:
            break;
        }

        // 计算电频率dpp
        foc_data_handler.hall_ops.f_foc = PWM_FREQ;
        foc_data_handler.hall_ops.hall_psc = 1;
        if (foc_data_handler.m_mode == MOTOR_RUN) {
            // 滑动均值滤波（完成启动，缓存区都有6个数据，往后都除以6求平均）
            foc_data_handler.hall_ops.dpp_sum -=
                foc_data_handler.hall_ops.buffer[foc_data_handler.hall_ops.buffer_index];

            // // 计数表示频率 f_hall=clk/(psc*cap)
            // foc_data_handler.hall_ops.f_hall = HALL_CLK / (foc_data_handler.hall_ops.hall_psc *
            //                                                foc_data_handler.hall_ops.hall_cap);

            // // 电频率(每个foc周期角度的变化量)  65536/6=10923 dpp=0~65535  小角度增量
            // foc_data_handler.hall_ops.dpp =
            //     10923 / (foc_data_handler.hall_ops.f_hall / foc_data_handler.hall_ops.f_foc) *
            //     foc_data_handler.hall_ops.dir;
            // foc_hall_cnt_sum = foc_hall_cnt;
            // foc_hall_cnt = 0;
            // foc_data_handler.hall_ops.dpp = 10923 * foc_hall_cnt / foc_hall_cnt_sum;

            // 滑动均值滤波
            foc_data_handler.hall_ops.buffer[foc_data_handler.hall_ops.buffer_index] =
                foc_data_handler.hall_ops.dpp;

            foc_data_handler.hall_ops.dpp_sum +=
                foc_data_handler.hall_ops.buffer[foc_data_handler.hall_ops.buffer_index];

            foc_data_handler.hall_ops.buffer_index++;
            if (foc_data_handler.hall_ops.buffer_index >= WBuffer_MAX_SIZE)
                foc_data_handler.hall_ops.buffer_index = 0;

            /* 滑动均值-电频率  */
            foc_data_handler.hall_ops.avg_dpp =
                (foc_data_handler.hall_ops.dpp_sum / WBuffer_MAX_SIZE);
        }
    }
}

/**
 * @brief 更新电机的电角速度和机械角速度，将整型速度值/每foc执行周期转化为rad/s
 * @param   dpp 整型，指的是整型速度值/每foc执行周期
 * @param   hall 霍尔传感器对象
 * @return
 */
float hall_get_Speed(foc_handler *foc_data)
{
    /*dpp为整型速度值/每foc执行周期 ，需要后期转化为rad/s*/
    /*所以高频任务的执行周期和65535和2PI需要结合实际弄出个乘积因子完成单位转化过程*/
    static float K = (float)(PI2_VALUE / 65536 / (FOC_PERIOD / 1000000.0f)); // us->s

    foc_data->hall_ops.omega_inter = ((float)(K * foc_data->hall_ops.dpp));

    return foc_data->hall_ops.omega_inter;
}

/**
 * @brief   插值法电角度估算函数(用于FOC高频中断任务中)
 * @param   hall 霍尔传感器指针
 * @funtion 实现了插值法捕获速度值;该方式使用0~65536表示0到2pi
 */
float hall_positionEst(foc_handler *foc_data)
{
    static float theta_k = PI2_VALUE / 65536;
    /***********************电角度从0到2PI往复 **************************/
    foc_data->hall_ops.refer_theta += foc_data->hall_ops.dpp; // θ_ref
    if (foc_data->hall_ops.refer_theta > 65536L) {
        foc_data->hall_ops.refer_theta -= 65536L;
    } else if (foc_data->hall_ops.refer_theta < 0) {
        foc_data->hall_ops.refer_theta += 65536L;
    }

    // hall变化=60°=delta theta
    // foc_data->hall_ops.comp_dpp = 10923 / (foc_data->hall_ops.f_foc / foc_data->hall_ops.f_hall);
    // foc_data->hall_ops.hall_theta += (foc_data->hall_ops.dpp + foc_data->hall_ops.comp_dpp); // θ_use
    foc_data->hall_ops.hall_theta += foc_data->hall_ops.dpp; // θ_use
    if (foc_data->hall_ops.hall_theta > 65536L) {
        foc_data->hall_ops.hall_theta -= 65536L;
    } else if (foc_data->hall_ops.hall_theta < 0) {
        foc_data->hall_ops.hall_theta += 65536L;
    }
    /***********************电角度从0到2PI往复 **************************/
    foc_data->hall_ops.hall_theta_inter = (float)(foc_data->hall_ops.hall_theta * theta_k); //<theta_inter插值法估算的电角度，就是文中的θ_use浮点形式

    return foc_data->hall_ops.hall_theta_inter;
}

/**
 * @brief            锁相环法PLL核心程序(Kp与Ki根据电角速度自适应)
 * @param    hall    霍尔位置传感器结构体对象
 * @function         浮点运算直接求弧度，实现了锁相环法;

 * @warnings         ！！！注意：速度用插值法速度，PLL估算出来的速度不能引入环路
 */
float hall_pll_filter(foc_handler *foc_data)
{
    static float theta_pll_k = PI2_VALUE / 65536; //<浮点数2PI用整型65536表示
    static float error = 0;

    error = foc_data->hall_ops.hall_theta * theta_pll_k - foc_data->hall_ops.pll_theta; // rad

    /***********************电角度从0到2PI往复，误差相应要处理
     * **************************/
    if (error >= PI_VALUE) {
        error -= PI2_VALUE;
    }
    if (error <= -PI_VALUE) {
        error += PI2_VALUE;
    }
    /***********************电角度从0到2PI往复，误差响应要处理
     * **************************/

    foc_data->hall_ops.pll_omega += (foc_data->hall_ops.pll_ki * error);
    /*************************锁相环法电角速度的限幅
     * ****************************/
    /*pll_omega结构体成员是弧度制(笔者整型和弧度制混用了，这方面代码没做到统一，是无法否认的缺点)*/
    /*笔者的实验电机4对极，额定转速3000RPM*/
    /*2320的选取背景是：把3000RPM(额定转速)转换为rad/s单位，并乘以极对数*/
    /*比如极对数4乘以3000RPM约等于1230rad/s*/
    /*有时候用到弱磁可以突破额定转速到5000RPM，所以给定一个较大的值，我这里直接1230乘以2倍左右*/
    if (foc_data->hall_ops.pll_omega > 2320) {
        foc_data->hall_ops.pll_omega = 2320;
    } else if (foc_data->hall_ops.pll_omega < -2320) {
        foc_data->hall_ops.pll_omega = -2320;
    }
    /*************************锁相环法电角速度的限幅
     * ****************************/
    foc_data->hall_ops.pll_theta += (foc_data->hall_ops.pll_omega + foc_data->hall_ops.pll_kp * error) *
                                    foc_data->hall_ops.pll_Te; // pll_Te周期

    /*************************电角度从0到2PI往复 ****************************/
    if (foc_data->hall_ops.pll_theta > PI2_VALUE) {
        foc_data->hall_ops.pll_theta -= PI2_VALUE;
    }
    if (foc_data->hall_ops.pll_theta < 0) {
        foc_data->hall_ops.pll_theta += PI2_VALUE;
    }
    /*************************电角度从0到2PI往复 ****************************/

    /*不能自己参照自己*/
    /*改变Kp与Ki时，要使用插值法的速度*/
    /*插值法的速度估计非常重要，重要程度最高*/
    /*插值法的速度处理直接影响性能，包括启动，快速正反转，堵转恢复*/
    /*omega_inter：插值法电角速度*/
    ///< 绝对不能用自己估算出来的速度，否则就是自己观测自己，控制系统不稳定
    foc_data->hall_ops.pll_ki = 0.236f * foc_data->hall_ops.omega_inter *
                                foc_data->hall_ops.omega_inter; // 0.236f约等于根号5-2
    arm_sqrt_f32(foc_data->hall_ops.pll_ki, &foc_data->hall_ops.pll_kp);

    foc_data->hall_ops.pll_kp *= 1.414f;                    // 阻尼比选0.707       2*0.707f=1.414f
    foc_data->hall_ops.pll_ki *= foc_data->hall_ops.pll_Te; // pll_Te是PLL法的执行时间，单位为s。放在FOC中断中其值就是1/f_FOC

    return foc_data->hall_ops.pll_theta;
}

/**
 * @brief
 速度更新中频任务（频率1k，确保速度更新大于速度环2倍以上，速度更新频率一般1kHz）
 * @param    htim    速度更新所使用的定时器指针
 * @function         这里配合foc高频中断+霍尔中断捕获实现估算插值补偿
 *                   MF：Moderate frequency中频


 * @warnings ！！！注意：本函数配合中频任务共同运行才能完成一次完整的速度估计
                     所以，完整电角度估算与速度估算流程是共同混杂在foc任务+速度更新中频任务+霍尔中断低频任务中的
 */
void speed_updateMF_IT(TIM_HandleTypeDef *htim)
{
    static int32_t n = (int32_t)(SPEED_PERIOD / FOC_PERIOD); ///< 就是公式中的n
    if (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE)) {
        /* w1=w0+a */
        foc_data_handler.hall_ops.dpp_2 = foc_data_handler.hall_ops.dpp_1;
        foc_data_handler.hall_ops.dpp_1 = foc_data_handler.hall_ops.avg_dpp;
        foc_data_handler.hall_ops.a_dpp = foc_data_handler.hall_ops.dpp_1 - foc_data_handler.hall_ops.dpp_2;
        foc_data_handler.hall_ops.dpp = foc_data_handler.hall_ops.dpp_1 + foc_data_handler.hall_ops.a_dpp;
        /* 小角度增量 */
        foc_data_handler.hall_ops.delta_theta = foc_data_handler.hall_ops.refer_theta - foc_data_handler.hall_ops.hall_theta;

        // 8192为45°的整型值，65536为2PI
        // 当电机实际的角度追踪值与FOC所用的角度值差某个度数(取30°~60°一般合理，别取太小，尽量往35以上取)时不可接受，这里为了示范取45°
        // 故当偏差大于45°时强制让它瞬变为目标追踪值，不做补偿逼近目标追踪值；
        if (foc_data_handler.hall_ops.delta_theta > 8192) {
            foc_data_handler.hall_ops.hall_theta = foc_data_handler.hall_ops.refer_theta; //<refer_theta就是上文的θref
            foc_data_handler.hall_ops.comp_dpp = 0;
        } else if (foc_data_handler.hall_ops.delta_theta < -8192) {
            foc_data_handler.hall_ops.hall_theta = foc_data_handler.hall_ops.refer_theta;
            foc_data_handler.hall_ops.comp_dpp = 0;
        } else {
            foc_data_handler.hall_ops.comp_dpp = (foc_data_handler.hall_ops.delta_theta / n);
        }
        __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_UPDATE);
    }
}

/**
 * @brief            FOC高频中断运行流程（至少8K，这里选取10K执行频率）
 * @param            电机FOC流程对象结构体
 * @description      电机的状态机用户可自行定义，这里从简
 *                   HF：High frequency高频
 */
void foc_processHF_IT(foc_handler *foc_data, ADC_HandleTypeDef *hadc)
{
    // static float Rv_U = 0, Rv_V = 0, Rv_W = 0;                //<采样电阻电压
    // static float current_U = 0, current_V = 0, current_W = 0;
    // //<三电阻法，相电流存储变量 static float speedP = (float)(1.0f / P);
    // //<乘积因子，电角速度=极对数*机械角速度

    // if (__HAL_ADC_GET_FLAG(foc->adc_motor_output.hadc, ADC_FLAG_JEOC)) {
    /*********电流采样(入门秘法：不分扇区情况，按两电阻法直接暴力采)*******/
    // switch (foc->state) {
    // case MOTOR_ADC_ALIGN: {
    //     //<第一步：电机ADC偏置矫正程序
    //     if (adc_count1-- < 0)
    //     //<间隔一段时间（adc_count1的值来决定）让硬件稳定
    //     {
    //         //<硬件稳定后，外设采集1024（adc_count2的值来决定）叠加
    //         foc->adc_motor_output.currentA_biasQ +=
    //         HAL_ADCEx_InjectedGetValue(foc->adc_motor_output.hadc,
    //         ADC_INJECTED_RANK_1); //<A/U相电压ADC校准
    //         foc->adc_motor_output.currentB_biasQ +=
    //         HAL_ADCEx_InjectedGetValue(foc->adc_motor_output.hadc,
    //         ADC_INJECTED_RANK_2); //<B/V相电压ADC校准
    //         foc->adc_motor_output.currentC_biasQ +=
    //         HAL_ADCEx_InjectedGetValue(foc->adc_motor_output.hadc,
    //         ADC_INJECTED_RANK_3); //<C/W相电压ADC校准
    //         foc->adc_motor_output.vbus_biasQ +=
    //         HAL_ADCEx_InjectedGetValue(foc->adc_motor_output.hadc,
    //         ADC_INJECTED_RANK_4);     //<母线电压ADC校准 if (adc_count2-- <
    //         0) //<采完1024次，整型右移10位是除以1024求平均值
    //         {
    //             foc->adc_motor_output.currentA_biasQ >>= 10;
    //             foc->adc_motor_output.currentB_biasQ >>= 10;
    //             foc->adc_motor_output.currentC_biasQ >>= 10;
    //             foc->adc_motor_output.vbus_biasQ >>= 10;
    //             foc->adc_motor_output.currentA_biasF = 3.3f *
    //             foc->adc_motor_output.currentA_biasQ / 4096.0f;
    //             foc->adc_motor_output.currentB_biasF = 3.3f *
    //             foc->adc_motor_output.currentB_biasQ / 4096.0f;
    //             foc->adc_motor_output.currentC_biasF = 3.3f *
    //             foc->adc_motor_output.currentC_biasQ / 4096.0f;
    //             foc->adc_motor_output.vbus_biasF = 3.3f *
    //             foc->adc_motor_output.vbus_biasQ / 4096.0f;
    //             motor_enable(&motor1);
    //             //<电机使能，不用在意，这个读者自己根据项目需求自行定义就可
    //             foc->state = MOTOR_RUN;
    //         }
    //     }
    //     break;
    // }
    // case MOTOR_RUN: {
    //<第二步：电机双闭环运行
    //<若做了母线电压采集，请在此处引入母线电压的ADC采集接口
    // foc->voltage_result.v_dc_real=HAL_ADCEx_InjectedGetValue(foc->adc_motor_output.hadc,ADC_INJECTED_RANK_4);

    // //<精密电阻R=0.01Ω电位采集
    // Rv_U = HAL_ADCEx_InjectedGetValue(foc->adc_motor_output.hadc,
    // ADC_INJECTED_RANK_1); // IU Rv_V =
    // HAL_ADCEx_InjectedGetValue(foc->adc_motor_output.hadc,
    // ADC_INJECTED_RANK_2); // IV Rv_W =
    // HAL_ADCEx_InjectedGetValue(foc->adc_motor_output.hadc,
    // ADC_INJECTED_RANK_3); // IW
    // /** 计算：数字量转化为模拟量*/
    // Rv_U = 3.3f * Rv_U / 4096.0f;
    // Rv_V = 3.3f * Rv_V / 4096.0f;
    // Rv_W = 3.3f * Rv_W / 4096.0f;
    // /** v_ADC=1.65-10v*/
    // /** 用户可自行修改*/
    // current_U = (foc->adc_motor_output.currentA_biasF - Rv_U) * 10.0f;
    // current_V = (foc->adc_motor_output.currentB_biasF - Rv_V) * 10.0f;
    // current_W = (foc->adc_motor_output.currentC_biasF - Rv_W) * 10.0f;
    // foc->current_result.i_a = current_U;
    // foc->current_result.i_b = current_V;
    // foc->current_result.i_c = current_W;
    // clark_transform(&foc->current_result, &foc->clark_result);

    /*********二选一，根据需求注释掉就可，推荐PLL法********/
    //****************插值法****************//
    // hall_get_Speed(foc_data);                                            //<插值法估算电角速度rad/s
    // foc_data->elec_angle = 360 / (PI2_VALUE)*hall_positionEst(foc_data); //<插值法估算角度
    //***********锁相环PLL法***************//
    // hall_get_Speed(foc_data); //<插值法获取电角速度rad/s
    // foc_data->hall_ops.pll_Te = 1.0f / PWM_PERIOD; //<PLL法执行时间，单位s
    // foc_data->elec_angle = hall_pll_filter(foc_data);
    // if (tim6_uptate_over % 50 == 0)
    //     foc_data->elec_angle = foc_data->hall_ops.hall_theta * 360 / 65536UL + 60;
    //****机械角速度计算*****//
    // foc->speed_result.eleSpeed = hall1.omega_inter;
    // //<插值法或锁相环法中的估算电角速度都可以带入(推荐插值法速度),单位rad/s
    // MOTOR_GET_MECH((&motor1), speedP); //<电角速度rad/s转化为机械角速度rad/s
    // MOTOR_GET_MECHRPM((&motor1)); //<机械角速度rad/s转化为机械角速度RPM
    // park_transform(&foc->clark_result, &foc->park_result,
    // &foc->angle_result.electrical_angle); if (speed_loop_count-- < 0) {
    //     speed_loop_count = 20;
    //     //<速度更新频率为1k,但速度环执行频率必须小于次，这里取500Hz
    //     iq_set((&current_loop), speed_loop_achieve(&speed_loop,
    //     foc->speed_result.mechRPM));
    // }
    // current_loop_achieve(&current_loop, foc->park_result.i_d,
    // foc->park_result.i_q);
    // //<Rs：同步旋转轴电阻
    // //<Ls=Ld=Lq：同步旋转轴电感(SPMSM/BLDC)
    // //<数据一定要精确才能加入耦合项，若数据不精确请大胆去掉耦合项
    // //<这里加上耦合项是符合理论的，符合我的另一篇博文PI控制器的参数整定
    // foc->ipark_result.v_d = current_loop.id_loop_output -
    // foc->speed_result.eleSpeed * Ls * foc->park_result.i_q;
    // //<FLUX_VALUE：磁链系数，单位是V·s/rad
    // foc->ipark_result.v_q = current_loop.iq_loop_output +
    // foc->speed_result.eleSpeed * (Ls * foc->park_result.i_d + FLUX_VALUE);

    // circle_calculate(foc); //<电压圆限制

    // ipark_transform(&foc->ipark_result, foc->ipark_result.v_d,
    // foc->ipark_result.v_q, &foc->angle_result.electrical_angle);
    // /**ִSVPWM生成，详情请参考我的另一篇博文**/
    // svpwm_myAchieve(foc);
    // break;
    // }
    //     case MOTOR_STOP: {
    //         //<状况三：电机停止功能程序接口，用户自定义
    //         motor_disable(foc);
    //         break;
    //     }
    //     case MOTOR_REPAIR: {
    //         //<状况四：电机故障修复程序接口，用户自定义
    //         break;
    //     }
    //     }
    //     __HAL_ADC_CLEAR_FLAG(foc->adc_motor_output.hadc, ADC_FLAG_JEOC);
    // }
}
