/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_hall_prm.h
* Author             : FOCSDK development tool ver. 1.0.0 by IMS Systems Lab
* Creation date      : Thu Dec 01 10:57:43 2016
* Description        : Contains the list of project specific parameters related
*                      to the Hall sensors speed feedback.
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
#ifndef __HALL_PRM_H
#define __HALL_PRM_H
/* Includes ------------------------------------------------------------------*/
#include "STM32F10x_MCconf.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* APPLICATION SPECIFIC DEFINE -----------------------------------------------*/
/* Define here the 16-bit timer chosen to handle hall sensors feedback  */
/* Timer 2 is the mandatory selection when using STM32MC-KIT  */
/* 三路霍尔传感器信号与 TIMER2 输入引脚相连 */
//#define TIMER2_HANDLES_HALL
/* 三路霍尔传感器信号与 TIMER3 输入引脚相连 */
#define TIMER3_HANDLES_HALL
/* 三路霍尔传感器信号与 TIMER4 输入引脚相连 */
//#define TIMER4_HANDLES_HALL

/* HALL SENSORS PLACEMENT ----------------------------------------------------*/
/* 以角度位单位定义霍尔传感器之间电位移（物理位移×磁极对数）*/
#define	DEGREES_120 0
#define	DEGREES_60 1

/* Define here the mechanical position of the sensors with reference to an 
                                                             electrical cycle */ 
#define HALL_SENSORS_PLACEMENT DEGREES_120

/* Define here in degrees the electrical phase shift between the low to high
transition of signal H1 and the maximum of the Bemf induced on phase A */
/* 以角度定义 0°与 TIMx_CH1第一个上升沿之间的电角度偏移。*/
#define	HALL_PHASE_SHIFT (s16) 120

/* APPLICATION SPEED DOMAIN AND ERROR/RANGE CHECKING -------------------------*/

/* Define here the rotor mechanical frequency above which speed feedback is not 
realistic in the application: this allows discriminating glitches for instance 
*/
/* 定义转子最大机械转速（转数/分），当大于它时，速度反馈不可用：用于区别故障。*/
#define	HALL_MAX_SPEED_FDBK_RPM ((u32)30000)
/* Define here the returned value if measured speed is > MAX_SPEED_FDBK_RPM
It could be 0 or FFFF depending on upper layer software management */
/* 如果检测到速度大于HALL_MAX_SPEED_FDBK_RPM 时，函数HALL_GetSpeed */
/*（单位：0.1Hz）返回这个值。 */

#define HALL_MAX_SPEED               ((u16)5000) // Unit is 0.1Hz
/* 如果检测到速度大于HALL_MAX_SPEED_FDBK_RPM时，函数HALL_GetRotorFreq */

/* 返回这个值。单位：dpp。 */
// With digit-per-PWM unit (here 2*PI rad = 0xFFFF):
#define HALL_MAX_PSEUDO_SPEED        ((s16)-32768)

/* Define here the rotor mechanical frequency below which speed feedback is not 
realistic in the application: this allows to discriminate too low freq for 
instance */
/* 定义转子最小机械转速(rpm)，当小于它时，速度反馈不可用。 */
#define	HALL_MIN_SPEED_FDBK_RPM ((u16)60)
/* Max TIM prescaler ratio defining the lowest expected speed feedback */
/* -定义可以测量的最低速度（当计数=0xFFFF）*/
/* -当电机停止时，防止时钟分频器减小过度。（每个捕获中断以优化时钟分辨 */
/* 率时预分频器会自动调整） */

#define HALL_MAX_RATIO		((u16)800u)
/* Number of consecutive timer overflows without capture: this can indicate
that informations are lost or that speed is decreasing very sharply */
/* This is needed to implement hall sensors time-out. This duration depends on hall sensor
timer pre-scaler, which is variable; the time-out will be higher at low speed*/
/* 定时器连续溢出的最大次数 */
/* 当定时器出现两次以上溢出（意味霍尔传感器在连续有效边缘周期至少增大两倍），*/
/* 溢出数目将不再累加。这通常表明数据已经丢失（霍尔传感器超时）或速度的急剧 */
/* 下降。对应超时延迟根据选择定时器预分频器不同而不同，是一个变量；预分频器 */
/*频率越高（低速），定时器超时时间越长 */

#ifdef FLUX_TORQUE_PIDs_TUNING
#define HALL_MAX_OVERFLOWS       ((u16)4)
#else
#define HALL_MAX_OVERFLOWS       ((u16)2)
#endif

/* ROLLING AVERAGE DEPTH -----------------------------------------------------*/
/* 存储最新测量的速度的软件 FIFO 长度。在连续数据块堆栈之间有必要计算滚动平均值。*/
#ifdef FLUX_TORQUE_PIDs_TUNING
#define HALL_SPEED_FIFO_SIZE 	((u8)1)
#else
#define HALL_SPEED_FIFO_SIZE 	((u8)6)
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __HALL_PRM_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
