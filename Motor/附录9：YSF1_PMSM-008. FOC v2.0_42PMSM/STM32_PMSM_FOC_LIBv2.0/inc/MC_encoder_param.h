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
/* ��·�������ź���TIMER2������������ */
//#define TIMER2_HANDLES_ENCODER
/* ��·�������ź���TIMER3������������ */
#define TIMER3_HANDLES_ENCODER
/* ��·�������ź���TIMER4������������ */
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
/* ���������ÿתһ�ܵ�ͨ��������������(ʵ�ʷֱ�����4�ı���) */
#define ENCODER_PPR           (u16)(1000)   // number of pulses per revolution

/* Define here the absolute value of the application minimum and maximum speed 
                                                                   in rpm unit*/
/* ����ת����С��еת��(ת��/��)����С����ʱ���ٶȲ�����Ӧ���в��߱��� */
/* ���ԺͰ�ȫ�ԣ������ٶȵ����趨ֵ����������ͻ��ۻ���Ϊ����ֹ�����ۼ� */
/* �����Ƶ���ٶȵ��㣬 �û����Խ���������Ϊ�� */
#define MINIMUM_MECHANICAL_SPEED_RPM  (u32)60   //rpm
/* ����ת������еת��(ת��/��)����������ʱ���ٶȲ�����Ӧ���в��߱��� */
/* ���ԺͰ�ȫ�ԣ������ٶȸ����趨ֵ����������ͻ��ۻ��� */
#define MAXIMUM_MECHANICAL_SPEED_RPM  (u32)10000 //rpm

/* Define here the number of consecutive error measurement to be detected 
   before going into FAULT state */
/* �ڲ�������ѶϢ֮ǰ�������ٶȲ������������ٶȴ������ */
#define MAXIMUM_ERROR_NUMBER (u8)3
/* Computation Parameter*/
//Number of averaged speed measurement
/* ������ת��ʹ�õĻ�������С��2 �ı������ڼ��㡣*/
#define SPEED_BUFFER_SIZE   12   // power of 2 required to ease computations

/*************************** Alignment settings *******************************/
/*************************** У׼���� **************************************/
/* ��������һ�����λ�ô����������Ǵų��������Ҫ�����λ����Ϣ�������� */
/* ĳ�ַ�ʽ����һ����Ƕ�λ�á������ͨ��У׼��λ��ɣ����ڵ����һ���� */
/* ��֮ǰ���У�Ҳ�ڴ����¼�����С�*/

//Alignemnt duration
/* ������λУ׼ʱ�䣨��λ��ms�� */
#define T_ALIGNMENT           (u16) 100    // Alignment time in ms

/* �涨�������� */
#define ALIGNMENT_ANGLE       (u16) 90 //Degrees [0..359] 
//  90?<-> Ia = I_ALIGNMENT, Ib = Ic =-I_ALIGNMENT/2) 

// With MB459 and ALIGNMENT_ANGLE equal to 90?
//final alignment phase current = (I_ALIGNMENT * 0.64)/(32767 * Rshunt) 
/* ����ο�Id��ֵ������ֵ */
#define I_ALIGNMENT           (u16) 5575

//Do not be modified
#define T_ALIGNMENT_PWM_STEPS     (u32) ((T_ALIGNMENT * SAMPLING_FREQ)/1000) 
#define ALIGNMENT_ANGLE_S16       (s16)((s32)(ALIGNMENT_ANGLE) * 65536/360)
#define MINIMUM_MECHANICAL_SPEED  (u16)(MINIMUM_MECHANICAL_SPEED_RPM/6)
#define MAXIMUM_MECHANICAL_SPEED  (u16)(MAXIMUM_MECHANICAL_SPEED_RPM/6)
#endif  /*__MC_ENCODER_PARAM_H*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
