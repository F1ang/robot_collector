#include "bsp_motor.h"
#include "bsp_foc.h"
#include "bsp_hall.h"

Start_upStatus_t Start_Up_State = S_INIT;
volatile int16_t hSpeed_Reference;
int16_t hFreq_Inc;
int32_t hI_Inc;
uint32_t wTime = 0;
int32_t wStart_Up_Freq = 0; // 启动频率
int32_t wStart_Up_I;        // 启动电流
int16_t hAngle = 0;

/**
 * @brief: 初始化磁场定向算法参数
 * @return {*}
 */
void MCL_Init(void)
{
    FOC_Init();
    HALL_InitHallMeasure(); // CC1使能、HALL捕获溢出/非溢出处理标志

    HALL_Init_Electrical_Angle();            // 1,HALL预定位
    SVPWM_3ShuntCurrentReadingCalibration(); // ADC的漂移、开启JEOC中断

    Foc_Svpwm(&foc_data_handler, PWM_FREQ, 23.0f);

    HAL_Delay(100);

    __HAL_TIM_MOE_ENABLE(&htim1);

    SVPWM_3ShuntAdvCurrentReading(ENABLE);
}

/**
 * @brief: 外同步加速
 * @return {*} : None
 */
void STO_Start_Up(void)
{
    int16_t hAux;

    switch (Start_Up_State) {
    case S_INIT:
        // Init Ramp-up variables
        if (hSpeed_Reference >= 0) // 目标速度参考值
        {
            hFreq_Inc = FREQ_INC; // 频率增量
            hI_Inc = I_INC;       // 电流增量
            if (wTime == 0) {
                wStart_Up_I = FIRST_I_STARTUP * 1024; // 开始启动电流
            }
        } else {
            hFreq_Inc = -(int16_t)FREQ_INC;
            hI_Inc = -(int16_t)I_INC;
            if (wTime == 0) {
                wStart_Up_I = -(int32_t)FIRST_I_STARTUP * 1024;
            }
        }
        Start_Up_State = ALIGNMENT;
        break;

    case ALIGNMENT:
        Start_Up_State = RAMP_UP;
        break;

    case RAMP_UP:
        wTime++;
        if (wTime <= I_STARTUP_PWM_STEPS) // 启动励磁频率/电流++
        {
            wStart_Up_Freq += hFreq_Inc;
            wStart_Up_I += hI_Inc;
        } else if (wTime <= FREQ_STARTUP_PWM_STEPS) // 电流不动，启动励磁频率++
        {
            wStart_Up_Freq += hFreq_Inc;
        } else // 电流不动，启动励磁频率已最大，启动失败
        {
            // Re_initialize Start Up
        }

        // 启动电流，启动励磁频率
        // Add angle increment for ramp-up
        hAux = wStart_Up_Freq / 65536;
        hAngle = (int16_t)(hAngle + (int32_t)(65536 / (SAMPLING_FREQ / hAux))); // 角度增量

        // Stat_Curr_a_b = GET_PHASE_CURRENTS();
        // Stat_Curr_alfa_beta = Clarke(Stat_Curr_a_b);
        // Stat_Curr_q_d = Park(Stat_Curr_alfa_beta, hAngle); // 启动励磁频率->Iq Id 和绝对角度

        // hAux = wStart_Up_I / 1024;
        // hTorque_Reference = hAux; // 启动电流->参考转矩
        // hFlux_Reference = 0;

        // /*loads the Torque Regulator output reference voltage Vqs*/
        // Stat_Volt_q_d.qV_Component1 = PID_Regulator(hTorque_Reference,
        //                                             Stat_Curr_q_d.qI_Component1, &PID_Torque_InitStructure);
        // /*loads the Flux Regulator output reference voltage Vds*/
        // Stat_Volt_q_d.qV_Component2 = PID_Regulator(hFlux_Reference,
        //                                             Stat_Curr_q_d.qI_Component2, &PID_Flux_InitStructure);

        // RevPark_Circle_Limitation();

        /*Performs the Reverse Park transformation,
        i.e transforms stator voltages Vqs and Vds into Valpha and Vbeta on a
        stationary reference frame*/

        // 开环
        foc_data_handler.elec_angle = hAngle * K_CON;
        foc_data_handler.uq = 15.0f;
        foc_data_handler.ud = 0.0f;

        // inv park ud,uq->u_alpha,u_beta
        foc_transform[PARK_INVERSE_TRANSFORM](&foc_data_handler);

        // SVPWM
        Foc_Svpwm(&foc_data_handler, PWM_FREQ, 23.0f);

        /* speed rpm  */
        if (HALL_SetRotorFreq_Dpp() > 40) // 2000rpm
            State = RUN;
        break;

    default:
        break;
    }
}
