/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_State_Observer_param.h
* Author             : FOCSDK development tool ver. 1.0.0 by IMS Systems Lab
* Creation date      : Thu Dec 01 10:57:43 2016
* Description        : Contains the PMSM State Observer related parameters
*                      (module MC_State_Observer_Interface.c)
*
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
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
#ifndef __MC_STATE_OBSERVER_PARAM_H
#define __MC_STATE_OBSERVER_PARAM_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/******************* 状态观测器参数 **************************/
/* 定义等于 ADC 转化的电流值位 32767（最大 16 位），单位安培。 */
/* 如果电流是通过分流电阻测得，则：MAX_CURRENT=3.3/(2*Rshunt*Av) */
/* Rshunt是分流电阻值（欧姆）; Av是运算放大器的增益 */
#define MAX_CURRENT 5.6             /* max current value, Amps */

// Values showed on LCD display must be here multiplied by 10 
/* K1（32 位值）是状态观测器矢量增益参数，当电机运行时，可通过LCD */
/* 接口调节初始值，并评估结果。这样，K1在LCD上显示的值小了10倍。 */
#define K1 (s32)(-20813)             /* State Observer Gain 1 */
// Values showed on LCD display must be here multiplied by 100 
/* K2（32 位值）是状态观测器矢量增益参数，当电机运行时，可通过LCD */
/* 接口调节初始值，并评估结果。这样，K1在LCD上显示的值小了100倍。 */
#define K2 (s32)(74071)             /* State Observer Gain 2 */

/* PLL相位检测增益 */
#define PLL_KP_GAIN (s16)(532*MOTOR_MAX_SPEED_RPM*POLE_PAIR_NUM/SAMPLING_FREQ)
/* PLL循环过滤增益 */
#define PLL_KI_GAIN (s16)(1506742*POLE_PAIR_NUM/SAMPLING_FREQ * MOTOR_MAX_SPEED_RPM/SAMPLING_FREQ)

                                  
/******************* START-UP PARAMETERS ***************************************/
/******************* 启动参数 **********************/
/*
              Speed /|\
FINAL_START_UP_SPEED |              /
                     |            /
                     |          /          
                     |        /	      
                     |      /          
                     |    / 
                     |  / 
                     |/_______________________________________\                       
                     0          FREQ_START_UP_DURATION      t /               */

/* 定义启动允许的总时间，单位毫秒 */
#define FREQ_START_UP_DURATION    (u16) 1500 //in msec
/* 定义旋转磁通的速度，启动的时间的最后的速度，单位转数/分（这个参数 */
/* 设定的是频率线性斜坡的斜率） */
#define FINAL_START_UP_SPEED      (u16) 2500 //Rotor mechanical speed (rpm)

/*
                 |I|/|\
                     |
      FINAL_I_STARTUP|       __________________    
                     |     /          
                     |    /	      
                     |   /          
                     |  / 
                     | / 
      FIRST_I_STARTUP|/ 
                     |_______________________________________________\                      
                     0 I_START_UP_DURATION  FREQ_START_UP_DURATION t /        */
                                                                              
// With MB459 phase current = (X_I_START_UP * 0.64)/(32767 * Rshunt)
/* 三相电流系统的初始幅值 */
#define FIRST_I_STARTUP           (u16) 3521
/* 三相电流系统的最终幅值。这幅值的选择应该使产生的磁场力矩与实际应用负载相匹配。 */
#define FINAL_I_STARTUP           (u16) 4108
/* 从幅值的初始到最后，线性电流幅值增长允许时间，单位毫秒 */
#define I_START_UP_DURATION       (u16) 350 //in msec

// Alignment settings 
/* 转子校准或预定位设置 */
#ifdef NO_SPEED_SENSORS_ALIGNMENT

//Alignemnt duration
/* 校准相位需要的持续时间，单位毫秒 */
#define SLESS_T_ALIGNMENT           (u16) 700    // Alignment time in ms
/* 指定矢量的方向 */
#define SLESS_ALIGNMENT_ANGLE       (u16) 90 //Degrees [0..359]  
//  90? <-> Ia = SLESS_I_ALIGNMENT, Ib = Ic =-SLESS_I_ALIGNMENT/2) 

// With SLESS_ALIGNMENT_ANGLE equal to 90? final alignment 
// phase current = (SLESS_I_ALIGNMENT * 1.65/ Av)/(32767 * Rshunt)  
// being Av the voltage gain between Rshunt and A/D input
/* 定义参考 Id 的最终值大小 */
#define SLESS_I_ALIGNMENT           (u16) 11443

#endif

/**************************** STATISTIC PARAMETERS ****************************/
/**************************** 统计参数 *************************************/

//Threshold for the speed measurement variance.
/* 参数设定速度测量方差的门槛：方差是离平均值的偏离程度 */
/* 根 据 公 式 ：σ^2 >= μ^2*VARIANCE _THRESHOLD ，当方差观测速度方差大于平均 */
/* 值的百分比，就认为无传感算法是不可，σ和μ分别是方差和观测速度平均值（方差  */
/* VARIANCE_THRESHOLD是0.0625即平均值的百分比为 25%）。VARIANCE_THRESHOLD的参  */
/* 数越小，故障检测算法就越严格（即安全级别越高），反之亦然。*/
#define VARIANCE_THRESHOLD        0.0625  //Percentage of mean value

// Number of consecutive tests on speed variance to be passed before start-up is
// validated. Checked every PWM period
/* 在成功启动之前，速度检测方差比 VARIANCE_THRESHOLD阀值低的连续次数。在这情况下，*/
/* 主要状态从启动转化到运行 */
#define NB_CONSECUTIVE_TESTS      (u16) 60
// Number of consecutive tests on speed variance before the variable containing
// speed reliability change status. Checked every SPEED_SAMPLING_TIME
/* 转子速度/位置检测方法声明不可靠前，速度检测方差比VARIANCE_THRESHOLD 阀值高的 */
/* 连续次数。在这情况下，主要状态从运转化到故障 */
#define RELIABILITY_HYSTERESYS    (u8)  3
//Minimum Rotor speed to validate the start-up
/* 启动程序终止时的正常运行最小速度以（当转子速度/位置是可靠的），单位转数/分*/
#define MINIMUM_SPEED_RPM             (u16) 580

/* 观测器方程系数，以使电机绕组和电感产生作用 */
#define F1 (s16)(16384)
#define F2 (s16)(8192)

//The parameters below shouldn't be modified
/*max phase voltage, 0-peak Volts*/
#define MAX_VOLTAGE (s16)((3.3/2)/BUS_ADC_CONV_RATIO) 

#define C1 (s32)((F1*RS)/(LS*SAMPLING_FREQ))
#define C2 (s32)((F1*K1)/SAMPLING_FREQ)
#define C3 (s32)((F1*MAX_BEMF_VOLTAGE)/(LS*MAX_CURRENT*SAMPLING_FREQ))
#define C4 (s32)((((K2*MAX_CURRENT)/(MAX_BEMF_VOLTAGE))*F2)/(SAMPLING_FREQ))
#define C5 (s32)((F1*MAX_VOLTAGE)/(LS*MAX_CURRENT*SAMPLING_FREQ))

#define MOTOR_MAX_SPEED_DPP (s32)((1.2*MOTOR_MAX_SPEED_RPM*65536*POLE_PAIR_NUM)\
                                                            /(SAMPLING_FREQ*60))

#define FREQ_STARTUP_PWM_STEPS (u32) ((FREQ_START_UP_DURATION * SAMPLING_FREQ)\
                                                                          /1000) 
#define FREQ_INC (u16) ((FINAL_START_UP_SPEED*POLE_PAIR_NUM*65536/60)\
                                                        /FREQ_STARTUP_PWM_STEPS)
#define I_STARTUP_PWM_STEPS (u32) ((I_START_UP_DURATION * SAMPLING_FREQ)/1000) 
#define I_INC (u16)((FINAL_I_STARTUP -FIRST_I_STARTUP)*1024/I_STARTUP_PWM_STEPS)
#define PERCENTAGE_FACTOR    (u16)(VARIANCE_THRESHOLD*128)      
#define MINIMUM_SPEED        (u16) (MINIMUM_SPEED_RPM/6)
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MC_STATE_OBSERVER_PARAM_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
