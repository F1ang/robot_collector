/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
 * File Name          : STM32F10x_MCconf.h
 * Author             : FOCSDK development tool ver. 1.0.0 by IMS Systems Lab
 * Creation date      : Thu Dec 01 10:57:43 2016
 * Description        : Motor Control Library configuration file.
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
#ifndef __STM32F10x_MCCONF_H
#define __STM32F10x_MCCONF_H

/********************   Current sampling technique definition   ***************/
/********************   电流采样技术定义    *********************/
/*   Define here the technique utilized for sampling the three-phase current  */

/* Current sensing by ICS (Isolated current sensors) */
/* 使用隔离电流传感器完成电流采样 */
// #define ICS_SENSORS

/* Current sensing by Three Shunt resistors */
/* 通过三相分流电阻进行完成电流采样 */
#define THREE_SHUNT

/* Current sensing by Single Shunt resistor */
/* 通过单相分流电阻进行完成电流采样 */
// #define SINGLE_SHUNT

/*******************  Position sensing technique definition  ******************/
/********************  位置传感器技术定义    *********************/
/* Define here the type of rotor position sensing utilized for both Field     */
/*                    Oriented Control and speed regulation                   */

/* Position sensing by quadrature encoder */
/* 增量编码器用于转子位置和速度测量 */
// #define ENCODER

/* Position sensing by Hall sensors */
/* 霍尔传感器用于转子位置和速度测量 */
#define HALL_SENSORS

/* Sensorless position sensing  */
/* 无传感器模式：FOC算法中状态观测器提供转子位置信息 */
// #define NO_SPEED_SENSORS
/* When in sensorless operation define here if you also want to acquire any */
/* position sensor information                                              */
/* 处理来自三个霍尔传感器的信息，通过DAC功能显示 */
/*（并与来自无传感器转子位置重建算法的信息相比） */
// #define VIEW_HALL_FEEDBACK
/* 处理来至增量式编码器的信息，将通过DAC的功能显示 */
/*（并与来自无传感器转子位置重建算法的信息相比） */
// #define VIEW_ENCODER_FEEDBACK

/* When in sensorless operation define here if you want to perform an       */
/* alignment before the ramp-up                                             */
/* 无传感器模式下，在电机启动前都要使驱动器对转子定位 */
// #define NO_SPEED_SENSORS_ALIGNMENT

/************************** FOC methods **************************************/
/********************   FOC控制方法    *********************/
/* Internal Permanent Magnet Motors Maximum-Torque-per-Ampere strategy */
/* 利用优化的 MTPA（最大转矩每安培）驱动来设计嵌入式永磁同步电动机（IPMSMs）*/
// #define IPMSM_MTPA

/* Flux weakening operations allowed */
/* 弱磁控制：用户需求使用电机转速超过额定速度 */
// #define FLUX_WEAKENING

/* Feed forward current regulation based on known motor parameters */
/* 前馈计算功能的标准PID电流调节 */
// #define FEED_FORWARD_CURRENT_REGULATION

/**************************   Brake technique   *******************************/
/********************   刹车技术定义    *********************/
/*      Define here the if you want to enable brake resistor management       */
/*          MANDATORY in case of operation in flux weakening region           */

/* Uncomment to enable brake resistor management feature */
/* 激活电阻制动软件运行 */
// #define BRAKE_RESISTOR

/********************    PIDs differential terms enabling  ********************/
/*        Define here if you want to enable PIDs differential terms           */

/* Uncomment to enable differential terms of PIDs */
/* 激活MC_PID_regulators库模块中的PID调节功能微分项 */
#define DIFFERENTIAL_TERM_ENABLED

/********************  PIDs gains tuning operations enabling  *****************/
/********************   PID增益整定选项使能    *********************/
/*  Define here if you want to tune currents (Id, Iq) PIDs, Luenberger State  */
/*                             Observer and PLL gains                         */

/* Uncomment to enable the generation of a square-wave shaped reference Iq */
/* 生成专门用于转矩和磁通PID增益调节的软件 */
/* 只在有传感器模式下有效 */
// #define FLUX_TORQUE_PIDs_TUNING

/* Uncomment to enable the tuning of Luenberger State Observer and PLL gains*/
/* 激活在液晶屏幕上生成用于观测器和PLL增益调节的可视界面 */
#define OBSERVER_GAIN_TUNING

/***********************   DAC functionality enabling  ************************/
/********************   DAC功能使能    *********************/
/*Uncomment to enable DAC functionality feature through TIM3 output channels*/
/* 激活DAC功能 */
// #define DAC_FUNCTIONALITY

/******************************************************************************/
/* Check-up of the configuration validity*/
#if ((defined(ICS_SENSORS)) && (defined(THREE_SHUNT)))
#error "Invalid configuration: Two current sampling techniques selected"
#endif

#if ((defined(ICS_SENSORS)) && (defined(SINGLE_SHUNT)))
#error "Invalid configuration: Two current sampling techniques selected"
#endif

#if ((defined(SINGLE_SHUNT)) && (defined(THREE_SHUNT)))
#error "Invalid configuration: Two current sampling techniques selected"
#endif

#if ((!defined(ICS_SENSORS)) && (!defined(THREE_SHUNT)) && (!defined(SINGLE_SHUNT)))
#error "Invalid setup: No sampling technique selected"
#endif

#if ((defined(ENCODER)) && (defined(HALL_SENSORS)) || (defined(ENCODER)) && (defined(NO_SPEED_SENSORS)) || (defined(NO_SPEED_SENSORS)) && (defined(HALL_SENSORS)))
#error "Invalid configuration: Two position sensing techniques selected"
#endif

#if ((!defined(ENCODER)) && (!defined(HALL_SENSORS)) && (!defined(NO_SPEED_SENSORS)))
#error "Invalid configuration: No position sensing technique selected"
#endif

#if ((defined VIEW_HALL_FEEDBACK) && (!defined NO_SPEED_SENSORS))
#error "Invalid configuration: VIEW_HALL_FEEDBACK supported only in\
                                                           sensorless operation"
#endif

#if ((defined VIEW_ENCODER_FEEDBACK) && (!defined NO_SPEED_SENSORS))
#error "Invalid configuration: VIEW_ENCODER_FEEDBACK supported only in\
                                                           sensorless operation"
#endif

#if ((defined FLUX_TORQUE_PIDs_TUNING) && (defined NO_SPEED_SENSORS))
#error "Invalid configuration: FLUX_TORQUE_PIDs_TUNING not supported in\
                                                           sensorless operation"
#endif

#if ((defined VIEW_HALL_FEEDBACK) && (defined VIEW_ENCODER_FEEDBACK))
#error "Invalid configuration: Two position sensing techniques selected"
#endif

#if ((!defined NO_SPEED_SENSORS) && (defined NO_SPEED_SENSORS_ALIGNMENT))
#warning "No speed sensors alignment has been disabled"
#undef NO_SPEED_SENSORS_ALIGNMENT
#endif

#endif /* __STM32F10x_MCCONF_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
