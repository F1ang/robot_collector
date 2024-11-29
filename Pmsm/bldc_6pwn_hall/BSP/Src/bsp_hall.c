/*
 * @Welcome: Hacker
 * @Author: 硬石科技
 */
#include "bsp_hall.h"

/**
  * 函数功能: 霍尔传感器引脚初始化.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：使用宏定义方法代替具体引脚号，方便程序移植，只要简单修改bsp_HALL.h
  *           文件相关宏定义就可以方便修改引脚。
  */
void HALL_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStructure;
  /* 定义外部中断线初始化结构体变量 */
  EXTI_InitTypeDef EXTI_InitStructure;
  /* 定义嵌套向量中断控制器初始化结构体变量 */
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 为启用IO引脚中断功能需要使能复用功能时钟 */
  RCC_APB2PeriphClockCmd(HALL_RCC_CLOCKGPIO | HALL_RCC_CLOCKAFIO,ENABLE);
  
  /* 设定引脚IO编号 */
  GPIO_InitStructure.GPIO_Pin = HALL_U_GPIO_PIN;  
  /* 设定引脚IO为输入模式 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  /* 初始化KEY2对应引脚IO */
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = HALL_V_GPIO_PIN; 
  
  GPIO_InitStructure.GPIO_Pin = HALL_W_GPIO_PIN; 
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  
  /* 选择作为中断输入源 */
  GPIO_EXTILineConfig(HALL_GPIO_PORTSOURCE,HALL_U_GPIO_PINSOURCE);
  GPIO_EXTILineConfig(HALL_GPIO_PORTSOURCE,HALL_V_GPIO_PINSOURCE);
  GPIO_EXTILineConfig(HALL_GPIO_PORTSOURCE,HALL_W_GPIO_PINSOURCE);
  
  /* 对应的中断线 */
  EXTI_InitStructure.EXTI_Line=HALL_U_EXITLINE;
  /* 外部中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  /* 下降沿触发方式 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  /* 使能中断 */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  /* 根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器 */
  EXTI_Init(&EXTI_InitStructure);
  
  EXTI_InitStructure.EXTI_Line=HALL_V_EXITLINE;
  EXTI_Init(&EXTI_InitStructure);
  
  EXTI_InitStructure.EXTI_Line=HALL_W_EXITLINE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* 使能外部中断通道 */
  NVIC_InitStructure.NVIC_IRQChannel = HALL_EXTI_IRQn;
  /* 设置抢占式优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  /* 设置子优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  /* 使能外部中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  /* 初始化配置嵌套向量中断控制器 */
  NVIC_Init(&NVIC_InitStructure); 
}


/**
  * 函数功能: 霍尔传感器引脚初始化.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：使用宏定义方法代替具体引脚号，方便程序移植，只要简单修改bsp_HALL.h
  *           文件相关宏定义就可以方便修改引脚。
  */
static void HALL_TIMx_GPIO_Init(void)
{
   /* 定义IO硬件初始化结构体变量 */
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* 为启用IO引脚中断功能需要使能复用功能时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
  
  /* 设定引脚IO编号 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  
  /* 设定引脚IO为输入模式 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  /* 初始化KEY2对应引脚IO */
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* 定时器通道引脚重映射 */
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
}

/**
  * 函数功能: 配置嵌套向量中断控制器NVIC
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void HALL_TIMx_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置TIM为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	/* 设置抢占式优先级为0 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	/* 设置子优先级为0 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	/* 使能中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * 函数功能: 配置TIMx输出的PWM信号的模式，如周期、极性、占空比
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> TIMxCNT 重新计数
 *                    TIMx_CCR(电平发生变化)
 * 信号周期=(TIMx_ARR +1 ) * 时钟周期
 * 占空比=TIMx_CCR/(TIMx_ARR +1)
 *    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 *
 */
static void HALL_TIMx_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef        TIM_ICInitStructure;

  /* 使能定时器时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  /* 定时器基本参数始终 */		 
  /* 定时周期: HALL_TIM_Period+1 */
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 
  /* 设置预分频：HALL_TIM_Prescaler,输出脉冲频率：72MHz/(HALL_TIM_Prescaler+1)/(HALL_TIM_Period+1) */
  TIM_TimeBaseStructure.TIM_Prescaler = 71;
  /* 设置时钟分频系数：不分频(这里用不到) */
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
  /* 向上计数模式 */
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* 初始化TIM5输入捕获参数 */
  /* CC1S=01 	选择输入端 IC1映射到TI1上 */
	TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
  /* 上升沿捕获 */
	TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_BothEdge;	
  /* 映射到TI1上 */
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC; 
  /* 配置输入分频,不分频  */
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	           
  /* IC1F=0000 配置输入滤波器 不滤波 */
	TIM_ICInitStructure.TIM_ICFilter    = 0x08;                         
	TIM_ICInit(TIM3, &TIM_ICInitStructure);  
  
  /* 配置NVIC */
  HALL_TIMx_NVIC_Configuration();
  
  TIM_SelectHallSensor(TIM3,ENABLE);                          //使能TIMx的霍尔传感器接口
  TIM_SelectInputTrigger(TIM3, TIM_TS_TI1F_ED);               	//输入触发源选择     
  
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);             	//从模式选择
  TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);	//主从模式选择    
   /* 允许更新中断 ,允许CC1IE捕获中断	 */
  TIM_ITConfig(TIM3, TIM_IT_Trigger, ENABLE);
  /* 使能定时器 */
  TIM_Cmd(TIM3, ENABLE);
  TIM_ClearITPendingBit (TIM3,TIM_IT_Trigger);
}

/**
  * 函数功能: TIMx通道1输入捕获初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void HALL_TIMx_Init(void)
{
	HALL_TIMx_GPIO_Init();
	HALL_TIMx_Configuration();	
}




