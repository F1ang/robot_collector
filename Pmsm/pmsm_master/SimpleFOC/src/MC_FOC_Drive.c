/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_FOC_Drive.c
* Author             : IMS Systems Lab 
* Date First Issued  : 21/11/07
* Description        : This file provides all the PMSM FOC drive functions.
* 
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
* 14/07/08 v2.0.1
* 28/08/08 v2.0.2
* 04/09/08 v2.0.3
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_MClib.h"
#include "MC_Globals.h"
#include "MC_const.h"
#include "MC_FOC_Drive.h"
#include "MC_PMSM_motor_param.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FW_KDDIV        1
#define FW_KD_GAIN      0
#define FW_D_TERM_INIT  0
#define VOLTAGE_SAMPLING_BUFFER 128
/* Private macro -------------------------------------------------------------*/
#define SATURATION_TO_S16(a)    if (a > S16_MAX)              \
                                {                             \
                                  a = S16_MAX;                \
                                }                             \
                                else if (a < -S16_MAX)        \
                                {                             \
                                  a = -S16_MAX;               \
                                }                             \
/* Private functions ---------------------------------------------------------*/
/* Private variable ----------------------------------------------------------*/
static volatile Curr_Components Stat_Curr_q_d_ref;
static Curr_Components Stat_Curr_q_d_ref_ref;  // ->FOC_Model

#ifdef FEED_FORWARD_CURRENT_REGULATION
static Volt_Components Stat_Volt_q_d_3;
static Volt_Components Stat_Volt_q_d_2;
#endif

#ifdef FLUX_WEAKENING
static Volt_Components Stat_Volt_q_d_1;
s16 hFW_V_Ref = FW_VOLTAGE_REF;
s16 hFW_P_Gain = FW_KP_GAIN;
s16 hFW_I_Gain = FW_KI_GAIN;
s16 hVMagn = 0;
#endif

/*******************************************************************************
* Function Name : FOC_Init
* Description   : The purpose of this function is to initialize to proper values
*                 all the variables related to the field-oriented control
*                 algorithm. To be called once prior to every motor startup.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void FOC_Init (void)
{  
  // FOC的SVPWM输入(转矩环输出)
  Stat_Curr_q_d_ref_ref.qI_Component1 = 0;
  Stat_Curr_q_d_ref_ref.qI_Component2 = 0;  
  
  Stat_Curr_q_d_ref.qI_Component1 = 0;
  Stat_Curr_q_d_ref.qI_Component2 = 0;
}

/**
 * @description: FOC算法实现
 * @param {速度环输出 = Stat_Curr_q_d_ref；转矩环输出 = Stat_Curr_q_d_ref_ref}
 * @return {None}
 */
void FOC_Model(void)
{
#ifdef HALL_SENSORS
  HALL_IncElectricalAngle();                          // 每个FOC周期的小增量角度积分
  
  /**********STARTS THE VECTOR CONTROL ************************/  
 
  Stat_Curr_a_b = GET_PHASE_CURRENTS();               // 电流测量值Ia Ib
  
  Stat_Curr_alfa_beta = Clarke(Stat_Curr_a_b);        // 变换为alpha-beta坐标系
  
  Stat_Curr_q_d = Park(Stat_Curr_alfa_beta,GET_ELECTRICAL_ANGLE);  // 变换为q-d坐标系


  /*loads the Torque Regulator output reference voltage Vqs*/   
  Stat_Volt_q_d.qV_Component1 = PID_Regulator(Stat_Curr_q_d_ref_ref.qI_Component1, 
                        Stat_Curr_q_d.qI_Component1, &PID_Torque_InitStructure);    

  
  /*loads the Flux Regulator output reference voltage Vds*/
  Stat_Volt_q_d.qV_Component2 = PID_Regulator(Stat_Curr_q_d_ref_ref.qI_Component2, 
                          Stat_Curr_q_d.qI_Component2, &PID_Flux_InitStructure);  
  
  //circle limitation
  RevPark_Circle_Limitation(); // 圆限制，防止过调制
 
  /*Performs the Reverse Park transformation,
  i.e transforms stator voltages Vqs and Vds into Valpha and Vbeta on a 
  stationary reference frame*/

  Stat_Volt_alfa_beta = Rev_Park(Stat_Volt_q_d);

  /*Valpha and Vbeta finally drive the power stage*/ 
  CALC_SVPWM(Stat_Volt_alfa_beta); // 计算ccr1 ccr2 ccr3的导通时刻(作用时间)，ccr4的采样值
#elif defined NO_SPEED_SENSORS
  Stat_Curr_a_b = GET_PHASE_CURRENTS();
  Stat_Curr_alfa_beta = Clarke(Stat_Curr_a_b);
  Stat_Curr_q_d = Park(Stat_Curr_alfa_beta,GET_ELECTRICAL_ANGLE);  
  
  STO_Calc_Rotor_Angle(Stat_Volt_alfa_beta,Stat_Curr_alfa_beta,MCL_Get_BusVolt());  // bemf+pll->w theta
  /*loads the Torque Regulator output reference voltage Vqs*/   
  Stat_Volt_q_d.qV_Component1 = PID_Regulator(Stat_Curr_q_d_ref_ref.qI_Component1, 
                        Stat_Curr_q_d.qI_Component1, &PID_Torque_InitStructure);

  
  /*loads the Flux Regulator output reference voltage Vds*/
  Stat_Volt_q_d.qV_Component2 = PID_Regulator(Stat_Curr_q_d_ref_ref.qI_Component2, 
                          Stat_Curr_q_d.qI_Component2, &PID_Flux_InitStructure);  
    //circle limitation
  RevPark_Circle_Limitation();
 
  /*Performs the Reverse Park transformation,
  i.e transforms stator voltages Vqs and Vds into Valpha and Vbeta on a 
  stationary reference frame*/

  Stat_Volt_alfa_beta = Rev_Park(Stat_Volt_q_d);

  /*Valpha and Vbeta finally drive the power stage*/ 
  CALC_SVPWM(Stat_Volt_alfa_beta);                        
#endif

}

/*******************************************************************************
* Function Name   : FOC_CalcFluxTorqueRef
* Description     : This function provides current components Iqs* and Ids* to be
*                   used as reference values (by the FOC_Model function) when in
*                   speed control mode
* Input           : None.
* Output          : None.
* Return          : None.
*******************************************************************************/
void FOC_CalcFluxTorqueRef(void)  // 速度环
{
  Stat_Curr_q_d_ref.qI_Component1 = PID_Regulator(hSpeed_Reference,
                                    GET_SPEED_0_1HZ,&PID_Speed_InitStructure);  // 参考速度,转子速度0.1Hz

  Stat_Curr_q_d_ref.qI_Component2 = 0;

  Stat_Curr_q_d_ref_ref = Stat_Curr_q_d_ref;                  // ->FOC_Model
  
  // 磁通+力矩：速度环->力矩环
  hTorque_Reference = Stat_Curr_q_d_ref_ref.qI_Component1;
  hFlux_Reference = Stat_Curr_q_d_ref_ref.qI_Component2;  
}

/*******************************************************************************
* Function Name   : FOC_TorqueCntrl
* Description     : This function provides current components Iqs* and Ids* to be
*                   used as reference values (by the FOC_Model function) when in
*                   Torque control mode
* Input           : None.
* Output          : None.
* Return          : None.
*******************************************************************************/
void FOC_TorqueCtrl(void)       // 力矩环
{
  Stat_Curr_q_d_ref_ref.qI_Component1 = hTorque_Reference;
  Stat_Curr_q_d_ref_ref.qI_Component2 = hFlux_Reference;
}

/*******************************************************************************
* Function Name   : FOC_MTPAInterface_Init
* Description     : According to the motor parameters written in the MTPA
*                   section of MC_PMSM_motor_param.h, it initializes all the
*                   variables related to the MTPA trajectory generator to proper
*                   values (FOC_MTPA function). It has to be called at least
*                   once before the first motor startup.
* Input           : None.
* Output          : None.
* Return          : None.
*******************************************************************************/
#ifdef IPMSM_MTPA
void FOC_MTPAInterface_Init(void)
{
  MTPA_Const MTPA_InitStructure = {SEGDIV,ANGC,OFST};
  FOC_MTPA_Init(MTPA_InitStructure, ID_DEMAG);
}
#endif

/*******************************************************************************
* Function Name   : FOC_FluxRegulatorInterface_Init
* Description     : According to the motor parameters written in the
*                   fluxweakening section of MC_PMSM_motor_param.h,
*                   it initializes all the variables related to flux-weakening
*                   operations to proper values (FOC_Flux_Regulator function).
*                   It has to be called before every motor startup.
* Input           : None.
* Output          : None.
* Return          : None.
*******************************************************************************/
#ifdef FLUX_WEAKENING
void FOC_FluxRegulatorInterface_Init(void)
{
  PID_Struct_t PI_Stat_Volt_InitStructure;
  
  PI_Stat_Volt_InitStructure.hKp_Gain = hFW_P_Gain;
  PI_Stat_Volt_InitStructure.hKp_Divisor = FW_KPDIV;
  PI_Stat_Volt_InitStructure.hKi_Gain = hFW_I_Gain;
  PI_Stat_Volt_InitStructure.hKi_Divisor = FW_KIDIV; 
  //Lower Limit for Output limitation
  PI_Stat_Volt_InitStructure.hLower_Limit_Output = ID_DEMAG;
  //Upper Limit for Output limitation
  PI_Stat_Volt_InitStructure.hUpper_Limit_Output = 0;
  PI_Stat_Volt_InitStructure.wLower_Limit_Integral = 
                              PI_Stat_Volt_InitStructure.hLower_Limit_Output *
                              PI_Stat_Volt_InitStructure.hKi_Divisor;   //Lower Limit for Integral term limitation
  PI_Stat_Volt_InitStructure.wUpper_Limit_Integral = 0;   //Lower Limit for Integral term limitation
  PI_Stat_Volt_InitStructure.wIntegral = 0;
  
  PI_Stat_Volt_InitStructure.hKd_Gain = FW_KD_GAIN;
  PI_Stat_Volt_InitStructure.hKd_Divisor = FW_KDDIV;
  PI_Stat_Volt_InitStructure.wPreviousError = FW_D_TERM_INIT;
  
  FOC_FluxRegulator_Init(&PI_Stat_Volt_InitStructure,NOMINAL_CURRENT);  
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
