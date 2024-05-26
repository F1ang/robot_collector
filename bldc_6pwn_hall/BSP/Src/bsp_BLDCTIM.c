#include "bsp_BLDCTIM.h" 


/**
  * 函数功能: 配置TIMx复用输出PWM时用到的I/O
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
static void BLDC_TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能定时器始终 */
	BLDC_TIM_APBxClock_FUN(BLDC_TIM_CLK, ENABLE);

  /* 使能定时器通道引脚GPIO时钟 */
  RCC_APB2PeriphClockCmd(BLDC_TIM_GPIO_CLK|RCC_APB2Periph_AFIO, ENABLE); 

  /* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(BLDC_TIM_CH1_PORT, &GPIO_InitStructure);

  /* 配置定时器通道2输出引脚模式 */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH2_PIN;
  GPIO_Init(BLDC_TIM_CH2_PORT, &GPIO_InitStructure);

  /* 配置定时器通道3输出引脚模式 */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH3_PIN;
  GPIO_Init(BLDC_TIM_CH3_PORT, &GPIO_InitStructure);
  
  /* 配置定时器互补通道1输出引脚模式 */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH1N_PIN;
  GPIO_Init(BLDC_TIM_CH1N_PORT, &GPIO_InitStructure);
  
  /* 配置定时器互补通道2输出引脚模式 */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH2N_PIN;
  GPIO_Init(BLDC_TIM_CH2N_PORT, &GPIO_InitStructure);
  
  /* 配置定时器互补通道3输出引脚模式 */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH3N_PIN;
  GPIO_Init(BLDC_TIM_CH3N_PORT, &GPIO_InitStructure);  
  
  /* Configuration: BKIN pin */
  GPIO_InitStructure.GPIO_Pin = BLDC_TIM_BKIN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(BLDC_TIM_BKIN_PORT, &GPIO_InitStructure);  
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
 */
static void BLDC_TIMx_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
  
  /* 定时器基本参数始终 */		 
  TIM_TimeBaseStructure.TIM_Period = BLDC_TIM_PERIOD; 
  /* 设置预分频：不预分频，即为72MHz */
  TIM_TimeBaseStructure.TIM_Prescaler = BLDC_TIM_PRESCALER;
  /* 设置时钟分频系数：不分频(这里用不到) */
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
  /* 向上计数模式 */
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  /* 重复计算器 */
  TIM_TimeBaseStructure.TIM_RepetitionCounter = BLDC_TIM_REPETITIONCOUNTER;
  TIM_TimeBaseInit(BLDC_TIMx, &TIM_TimeBaseStructure);

  /* 定时器输出通道1模式配置 */
  /* 模式配置：PWM模式1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  /* 输出状态设置：使能输出 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  /* 互补通道输出状态设置：使能输出 */
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  /* 设置跳变值，当计数器计数到这个值时，电平发生跳变 */
  TIM_OCInitStructure.TIM_Pulse = 0;
  /* 当定时器计数值小于CCR1_Val时为高电平 */
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity= TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
  /* 初始化定时器通道1输出PWM */
  TIM_OC1Init(BLDC_TIMx, &TIM_OCInitStructure);
  
  /* 定时器输出通道2模式配置 */
  /* 设置通道2的电平跳变值，输出另外一个占空比的PWM */
  TIM_OCInitStructure.TIM_Pulse = 0;
  /* 初始化定时器通道2输出PWM */
  TIM_OC2Init(BLDC_TIMx, &TIM_OCInitStructure);

  /* 定时器输出通道3模式配置 */
  /* 设置通道3的电平跳变值，输出另外一个占空比的PWM */
  TIM_OCInitStructure.TIM_Pulse = 0;
  /* 初始化定时器通道3输出PWM */
  TIM_OC3Init(BLDC_TIMx, &TIM_OCInitStructure);

  /* Automatic Output enable, Break, dead time and lock configuration*/
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
  TIM_BDTRInitStructure.TIM_DeadTime = 5;
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  TIM_BDTRConfig(BLDC_TIMx, &TIM_BDTRInitStructure);
  
  TIM_OC1PreloadConfig(BLDC_TIMx,TIM_OCPreload_Enable);
  TIM_OC2PreloadConfig(BLDC_TIMx,TIM_OCPreload_Enable);
  TIM_OC3PreloadConfig(BLDC_TIMx,TIM_OCPreload_Enable);
  
  /* 使能定时器重载寄存器ARR */
  TIM_ARRPreloadConfig(BLDC_TIMx, ENABLE);

  /* 使能定时器 */
  TIM_Cmd(BLDC_TIMx, ENABLE);
  
  /* TIM主输出使能 */
  TIM_CtrlPWMOutputs(BLDC_TIMx, ENABLE);
  
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
}

/**
  * 函数功能: TIMx 输出PWM信号初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：只要调用这个函数TIMx的四个通道就会有PWM信号输出
  */
void BLDC_TIMx_PWM_Init(void)
{
	BLDC_TIMx_GPIO_Config();
	BLDC_TIMx_Configuration();	
}


/**
  * 函数功能: 无刷驱动换相
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void BLDC_PHASE_CHANGE(uint8_t step)
{
  switch(step)
  {
    case 4: //B+ C-
      /* Next step: Step 2 Configuration -------------------------------------- */ 
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      /*  Channel2 configuration */    
      TIM_SetCompare2(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/100);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Enable);
      /*  Channel3 configuration */
      TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Enable);
      break;
    case 5: //B+ A-
      /* Next step: Step 3 Configuration -------------------------------------- */      
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Enable);
    
      /*  Channel2 configuration */
      TIM_SetCompare2(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/100);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Enable);
      /*  Channel3 configuration */
      break;
    case 1: //C+ A-
      /* Next step: Step 4 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
    
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Enable);
      
      /*  Channel2 configuration */ 
      /*  Channel3 configuration */
      TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/100);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Enable);

      break;
    case 3: //C+ B-
      /* Next step: Step 5 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);    
    
      /*  Channel1 configuration */      
      /*  Channel2 configuration */   
      TIM_SetCompare2(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Enable);
    
      /*  Channel3 configuration */      
      TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/100);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Enable);
    
      break;
    case 2: //A+ B-
      /* Next step: Step 6 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/100);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Enable);
      /*  Channel2 configuration */
      TIM_SetCompare2(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Enable);
      /*  Channel3 configuration */
    
      break;
    case 6: //A+ C-
      /* Next step: Step 1 Configuration -------------------------------------- */
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
      
      /*  Channel1 configuration */
      TIM_SetCompare1(BLDC_TIMx,BLDC_TIM_PERIOD*speed_duty/100);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Enable);
      /*  Channel2 configuration */      
      /*  Channel3 configuration */
      TIM_SetCompare3(BLDC_TIMx,BLDC_TIM_PERIOD);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Enable);
    
      break;
    default:
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
      TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
      TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
      break;
  }
}

/**
  * 函数功能: 系统滴答定时器回调函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 1ms运行该函数一次
  */
void HAL_SYSTICK_Callback(void)
{
  static uint16_t time_count=0;
  
  bldc_dev.stalling_count++;  
  if(bldc_dev.motor_state==RUN)
  {
    time_count++;    
    if(bldc_dev.stalling_count>2000) // 电机卡住超时
      bldc_dev.motor_state=STOP;
  }
  else
  {
    time_count=0;
  }
  if(time_count>50) // 50ms
  {
    int temp;
    int pid_result;
    // bldc_dev.step_counter记录霍尔传感器在50ms时间内产生的脉冲个数，而电机旋转一圈会有总共
    // (MOTOR_POLE_PAIR_NUM*6)个脉冲,使用 n=bldc_dev.step_counter/(MOTOR_POLE_PAIR_NUM*6) 为
    // 50ms内电机转动圈数，为换算为【转/分钟(rpm)】
    //  n/50 = x/(60*1000) --> x=bldc_dev.step_counter*50
    temp=bldc_dev.step_counter*50;
    pid_result=IncPIDCalc(temp);     // 计算增量
		
    pid_result =pid_result*10/25;    // *10/25为转速和占空比一个转换，转速（0~2500），占空比（0~1000）
		
    if((pid_result+speed_duty)<70)
      speed_duty =70;
    else if((pid_result+speed_duty)>1000)
      speed_duty =1000;
    else
      speed_duty +=pid_result;    
    time_count=0;
    bldc_dev.step_counter=0;
  }
}

void HALL_EXTI_Callback(void)
{
  __IO uint8_t uwStep = 0;
  uint16_t hall_read = (HALL_GPIO->IDR)&0x01c0; // 读取霍尔传感器信息
  
  if(bldc_dev.motor_state==STOP)return;
  
  uwStep=hall_read>>6;
  if(bldc_dev.motor_direction == CW) // 方向判断
    uwStep=7-uwStep;    

  BLDC_PHASE_CHANGE(uwStep); //驱动换相
}


/**
  * 函数功能: 定时器中断服务函数
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void HALL_TIMx_Callback(void)
{
  uint8_t pinstate=0;
  static int8_t pinstate0=0;

  if(bldc_dev.motor_state==STOP)return;

  if((GPIOC->IDR & GPIO_Pin_6) != (uint32_t)Bit_RESET)  //霍尔传感器状态获取
  {
    pinstate |= 0x01;
  }
  if((GPIOC->IDR & GPIO_Pin_7) != (uint32_t)Bit_RESET)  //霍尔传感器状态获取
  {
    pinstate |= 0x02;
  }
  if((GPIOC->IDR & GPIO_Pin_8) != (uint32_t)Bit_RESET)  //霍尔传感器状态获取
  {
    pinstate |= 0x04;
  }
  if(bldc_dev.motor_direction==CW) // 方向判断
    pinstate=7-pinstate;

  BLDC_PHASE_CHANGE(pinstate);//驱动换相  
  
  if( pinstate0!=pinstate) // 测试发现有时会连续出现两个相同的数据，这里滤掉重复的
  {    
    bldc_dev.step_counter++;    // 步进递增
    bldc_dev.stalling_count=0;  // 阻塞计数清零
  }
  pinstate0=pinstate;

}


