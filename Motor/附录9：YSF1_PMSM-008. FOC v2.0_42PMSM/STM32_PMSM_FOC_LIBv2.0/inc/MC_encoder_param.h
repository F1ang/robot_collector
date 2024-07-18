/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_encoder_param.h
* Author             : FOCSDK development tool ver. 1.0.0 by IMS Systems Lab
* Creation date      : Thu Dec 01 14:52:47 2016
* Description        : Contains the list of project specific parameters related
*                      to the encoder speed and position feedback.
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
#ifndef __MC_ENCODER_PARAM_H
#define __MC_ENCODER_PARAM_H

#include "STM32F10x_MCconf.h"

/* PERIPHERAL SET-UP ---------------------------------------------------------*/
#if ((defined ENCODER)||(defined VIEW_ENCODER_FEEDBACK))
/* Define here the 16-bit timer chosen to handle encoder feedback */
/*TIMER 2 is the mandatory selection when using STM32MC-KIT */
/* 两路传感器信号与TIMER2输入引脚连接 */
//#define TIMER2_HANDLES_ENCODER
/* 两路传感器信号与TIMER3输入引脚连接 */
#define TIMER3_HANDLES_ENCODER
/* 两路传感器信号与TIMER4输入引脚连接 */
//#define TIMER4_HANDLES_ENCODER
#endif  // ENCODER

#if defined(TIMER2_HANDLES_ENCODER)
#define ENCODER_TIMER         TIM2          // Encoder unit connected to TIM2
#elif defined(TIMER3_HANDLES_ENCODER)
#define ENCODER_TIMER         TIM3          // Encoder unit connected to TIM3
#else // TIMER4_HANDLES_ENCODER
#define ENCODER_TIMER         TIM4          // Encoder unit connected to TIM4
#endif

/*****************************  Encoder settings ******************************/
/* 定义编码器每转一周单通道产生的脉冲数(实际分辨率是4的倍数) */
#define ENCODER_PPR           (u16)(1000)   // number of pulses per revolution

/* Define here the absolute value of the application minimum and maximum speed 
                                                                   in rpm unit*/
/* 定义转子最小机械转速(转数/分)，当小于它时，速度测量在应用中不具备可 */
/* 行性和安全性；测量速度低于设定值错误计数器就会累积。为了制止这种累计 */
/* 并控制电机速度到零， 用户可以将参数设置为零 */
#define MINIMUM_MECHANICAL_SPEED_RPM  (u32)60   //rpm
/* 定义转子最大机械转速(转数/分)，当大于它时，速度测量在应用中不具备可 */
/* 行性和安全性；测量速度高于设定值错误计数器就会累积。 */
#define MAXIMUM_MECHANICAL_SPEED_RPM  (u32)10000 //rpm

/* Define here the number of consecutive error measurement to be detected 
   before going into FAULT state */
/* 在产生错误讯息之前，定义速度测量连续检测出速度错误次数 */
#define MAXIMUM_ERROR_NUMBER (u8)3
/* Computation Parameter*/
//Number of averaged speed measurement
/* 定义检测转速使用的缓冲区大小。2 的倍数便于计算。*/
#define SPEED_BUFFER_SIZE   12   // power of 2 required to ease computations

/*************************** Alignment settings *******************************/
/*************************** 校准设置 **************************************/
/* 正交编码一个相对位置传感器。考虑磁场定向控制要求绝对位置信息，必需以 */
/* 某种方式建立一个零角度位置。这可以通过校准相位完成，并在电机第一个启 */
/* 动之前进行，也在错误事件后进行。*/

//Alignemnt duration
/* 定义相位校准时间（单位：ms） */
#define T_ALIGNMENT           (u16) 100    // Alignment time in ms

/* 规定向量方向 */
#define ALIGNMENT_ANGLE       (u16) 90 //Degrees [0..359] 
//  90?<-> Ia = I_ALIGNMENT, Ib = Ic =-I_ALIGNMENT/2) 

// With MB459 and ALIGNMENT_ANGLE equal to 90?
//final alignment phase current = (I_ALIGNMENT * 0.64)/(32767 * Rshunt) 
/* 定义参考Id幅值的最终值 */
#define I_ALIGNMENT           (u16) 5575

//Do not be modified
#define T_ALIGNMENT_PWM_STEPS     (u32) ((T_ALIGNMENT * SAMPLING_FREQ)/1000) 
#define ALIGNMENT_ANGLE_S16       (s16)((s32)(ALIGNMENT_ANGLE) * 65536/360)
#define MINIMUM_MECHANICAL_SPEED  (u16)(MINIMUM_MECHANICAL_SPEED_RPM/6)
#define MAXIMUM_MECHANICAL_SPEED  (u16)(MAXIMUM_MECHANICAL_SPEED_RPM/6)
#endif  /*__MC_ENCODER_PARAM_H*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
