/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_PMSM_motor_param.h
* Author             : FOCSDK development tool ver. 1.0.0 by IMS Systems Lab
* Creation date      : Thu Dec 01 10:57:43 2016
* Description        : This file contains the PM motor parameters.
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
* 14/07/08 v2.0.1
* 28/08/08 v2.0.2
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTOR_PARAM_H
#define __MOTOR_PARAM_H

// Number of motor pair of poles
/* 电机磁极对的数目 */
#define	POLE_PAIR_NUM 	(u8) 4        /* Number of motor pole pairs */
/* 电机绕阻（相）电阻值，单位欧姆 */
#define RS               0.65            /* Stator resistance , ohm*/
/* 电机绕阻（相）电感值，单位亨利 */
#define LS               0.0006        /* Stator inductance , H */

// When using Id = 0, NOMINAL_CURRENT is utilized to saturate the output of the 
// PID for speed regulation (i.e. reference torque). 
// Whit MB459 board, the value must be calculated accordingly with formula:
// NOMINAL_CURRENT = (Nominal phase current (A, 0-to-peak)*32767* Rshunt) /0.64
/* 电机额定电流 */
#define NOMINAL_CURRENT             (s16)18367  //motor nominal current (0-pk)
/* 实际应用电机的最大速度（转数/分） */
#define MOTOR_MAX_SPEED_RPM         (u32)3000   //maximum speed required
/* 电机恒定电压Ke（相相 V/krpm RMS），单位伏特 */
#define MOTOR_VOLTAGE_CONSTANT      4.1   //Volts RMS ph-ph /kRPM
//Demagnetization current
/* 电机磁体未被磁化时最大参考电流Id* */
#define ID_DEMAG    -NOMINAL_CURRENT

#ifdef IPMSM_MTPA
//MTPA parameters, to be defined only for IPMSM and if MTPA control is chosen
#else
#define IQMAX NOMINAL_CURRENT
#endif

#ifdef FLUX_WEAKENING //弱磁运行的额外参数
// 在弱磁控制中定子电压保持恒定的幅值
#define FW_VOLTAGE_REF (s16)(985)   //Vs reference, tenth of a percent
// 弱磁模块中运行的 PI 调节器的比例增益
#define FW_KP_GAIN (s16)(3000)      //proportional gain of flux weakening ctrl
// 弱磁模块中运行的 PI 调节器的积分增益
#define FW_KI_GAIN (s16)(5000)      //integral gain of flux weakening ctrl
// 弱磁模块中运行的 PI 调节器的比例增益系数
#define FW_KPDIV ((u16)(32768))     //flux weak ctrl P gain scaling factor
// 弱磁模块中运行的 PI 调节器的积分增益系数
#define FW_KIDIV ((u16)(32768))     //flux weak ctrl I gain scaling factor
#endif

#ifdef FEED_FORWARD_CURRENT_REGULATION //用于前馈、高性能电流控制的额外参数
#define CONSTANT1_Q (s32)(68013)
#define CONSTANT1_D (s32)(68013)
#define CONSTANT2 (s32)(19769)
#endif

/*not to be modified*/
#define MAX_BEMF_VOLTAGE  (u16)((MOTOR_MAX_SPEED_RPM*\
                           MOTOR_VOLTAGE_CONSTANT*SQRT_2)/(1000*SQRT_3))

#define MOTOR_MAX_SPEED_HZ (s16)((MOTOR_MAX_SPEED_RPM*10)/60)

#define _0_1HZ_2_128DPP (u16)((POLE_PAIR_NUM*65536*128)/(SAMPLING_FREQ*10))

#endif /*__MC_PMSM_MOTOR_PARAM_H*/
/**************** (c) 2007  STMicroelectronics ********************************/
