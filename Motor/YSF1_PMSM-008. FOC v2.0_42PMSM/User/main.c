/**
 ******************************************************************************
 * 文件名程: main.c
 * 作    者: 硬石嵌入式开发团队
 * 版    本: V1.0
 * 编写日期: 2015-10-04
 * 功    能: 使用标准库方法控制LED灯亮灭
 ******************************************************************************
 * 说明：
 * 本例程配套硬石stm32开发板YS-F1Pro使用。
 *
 * 淘宝：
 * 论坛：http://www.ing10bbs.com
 * 版权归硬石嵌入式开发团队所有，请勿商用。
 ******************************************************************************
 */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_MClib.h"
#include "MC_Globals.h"
#include "bsp_lcd.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/*******************************************************************************
 * Function Name  : GPIO_Configuration
 * Description    : Configures the TIM1 Pins.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_DeInit(GPIOA);
    GPIO_StructInit(&GPIO_InitStructure);

    /* Configure PC.06, PC.07, PC.08 and PC.09 as Output push-pull for debugging
       purposes */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11);
}

/**
 * 函数功能: 主函数.
 * 输入参数: 无
 * 返 回 值: 无
 * 说    明: 无
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); // NVIC3
    GPIO_Configuration();                           // PA8~PA10

#ifdef THREE_SHUNT
    SVPWM_3ShuntInit(); // 通过三相分流电阻进行完成电流采样
#elif defined ICS_SENSORS
    SVPWM_IcsInit();
#elif defined SINGLE_SHUNT
    SVPWM_1ShuntInit();
#endif

#ifdef ENCODER
    ENC_Init();
#ifdef OBSERVER_GAIN_TUNING
    STO_StateObserverInterface_Init();
    STO_Init();
#endif
#elif defined HALL_SENSORS
    HALL_HallTimerInit(); // HALL-TIM3、CC1与Update中断
#ifdef OBSERVER_GAIN_TUNING
    STO_StateObserverInterface_Init();
    STO_Init();
#endif
#elif defined NO_SPEED_SENSORS
    STO_StateObserverInterface_Init();
    STO_Init();
#ifdef VIEW_ENCODER_FEEDBACK
    ENC_Init();
#elif defined VIEW_HALL_FEEDBACK
    HALL_HallTimerInit();
#endif
#endif

#ifdef DAC_FUNCTIONALITY
    MCDAC_Init();
#endif

    TB_Init(); // 系统滴答定时器

    PID_Init(&PID_Torque_InitStructure, &PID_Flux_InitStructure, &PID_Speed_InitStructure);

#ifdef BRAKE_RESISTOR
    MCL_Brake_Init();
#endif

    KEYS_Init();

    /* TIM1 Counter Clock stopped when the core is halted */
    //  DBGMCU_Config(DBGMCU_TIM1_STOP, ENABLE);

    // Init Bus voltage and Temperature average
    MCL_Init_Arrays();

    BSP_LCD_Init();
    LCD_SetTextColor(Blue);
    LCD_SetBackColor(White);
    Display_Welcome_Message();

    while (1) {
        Display_LCD();
        MCL_ChkPowerStage();
        // User interface management
        KEYS_process(); //  State = INIT

        switch (State) {
        case IDLE: // Idle state
            break;

        case INIT:
            MCL_Init(); // 电机控制层初始化
            TB_Set_StartUp_Timeout(3000);
            State = START;
            break;

        case START:
            // passage to state RUN is performed by startup functions;
            break;

        case RUN: // motor running
#ifdef ENCODER
            if (ENC_ErrorOnFeedback() == TRUE) {
                MCL_SetFault(SPEED_FEEDBACK);
            }
#elif defined HALL_SENSORS
            if (HALL_IsTimedOut()) {
                MCL_SetFault(SPEED_FEEDBACK);
            }
            if (HALL_GetSpeed() == HALL_MAX_SPEED) {
                MCL_SetFault(SPEED_FEEDBACK);
            }
#elif defined NO_SPEED_SENSORS

#endif
            break;

        case STOP: // motor stopped
            // shutdown power
            /* Main PWM Output Disable */
            TIM_CtrlPWMOutputs(TIM1, DISABLE);

            State = WAIT;

#ifdef THREE_SHUNT
            SVPWM_3ShuntAdvCurrentReading(DISABLE);
#endif
#ifdef SINGLE_SHUNT
            SVPWM_1ShuntAdvCurrentReading(DISABLE);
#endif
            Stat_Volt_alfa_beta.qV_Component1 = Stat_Volt_alfa_beta.qV_Component2 = 0;

#ifdef ICS_SENSORS
            SVPWM_IcsCalcDutyCycles(Stat_Volt_alfa_beta);
#elif defined THREE_SHUNT
            SVPWM_3ShuntCalcDutyCycles(Stat_Volt_alfa_beta);
#endif
            TB_Set_Delay_500us(2000); // 1 sec delay
            break;

        case WAIT: // wait state
            if (TB_Delay_IsElapsed() == TRUE) {
#ifdef ENCODER
                if (ENC_Get_Mechanical_Speed() == 0) {
                    State = IDLE;
                }
#elif defined HALL_SENSORS
                if (HALL_IsTimedOut()) {
                    State = IDLE;
                }
#elif defined NO_SPEED_SENSORS
                STO_InitSpeedBuffer();
                State = IDLE;
#endif
            }
            break;

        case FAULT:
            if (MCL_ClearFault() == TRUE) {
                if ((wGlobal_Flags & SPEED_CONTROL) == SPEED_CONTROL) {
                    bMenu_index = CONTROL_MODE_MENU_1;
                } else {
                    bMenu_index = CONTROL_MODE_MENU_6;
                }
#if defined NO_SPEED_SENSORS
                STO_InitSpeedBuffer();
#endif
                State = IDLE;
                wGlobal_Flags |= FIRST_START;
            }
            break;

        default:
            break;
        }
    }
}

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
