#include "bsp_BLDCTIM.h" 


/**
  * ��������: ����TIMx�������PWMʱ�õ���I/O
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void BLDC_TIMx_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ�ܶ�ʱ��ʼ�� */
	BLDC_TIM_APBxClock_FUN(BLDC_TIM_CLK, ENABLE);

  /* ʹ�ܶ�ʱ��ͨ������GPIOʱ�� */
  RCC_APB2PeriphClockCmd(BLDC_TIM_GPIO_CLK|RCC_APB2Periph_AFIO, ENABLE); 

  /* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(BLDC_TIM_CH1_PORT, &GPIO_InitStructure);

  /* ���ö�ʱ��ͨ��2�������ģʽ */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH2_PIN;
  GPIO_Init(BLDC_TIM_CH2_PORT, &GPIO_InitStructure);

  /* ���ö�ʱ��ͨ��3�������ģʽ */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH3_PIN;
  GPIO_Init(BLDC_TIM_CH3_PORT, &GPIO_InitStructure);
  
  /* ���ö�ʱ������ͨ��1�������ģʽ */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH1N_PIN;
  GPIO_Init(BLDC_TIM_CH1N_PORT, &GPIO_InitStructure);
  
  /* ���ö�ʱ������ͨ��2�������ģʽ */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH2N_PIN;
  GPIO_Init(BLDC_TIM_CH2N_PORT, &GPIO_InitStructure);
  
  /* ���ö�ʱ������ͨ��3�������ģʽ */
  GPIO_InitStructure.GPIO_Pin =  BLDC_TIM_CH3N_PIN;
  GPIO_Init(BLDC_TIM_CH3N_PORT, &GPIO_InitStructure);  
  
  /* Configuration: BKIN pin */
  GPIO_InitStructure.GPIO_Pin = BLDC_TIM_BKIN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(BLDC_TIM_BKIN_PORT, &GPIO_InitStructure);  
}

/**
  * ��������: ����TIMx�����PWM�źŵ�ģʽ�������ڡ����ԡ�ռ�ձ�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> TIMxCNT ���¼���
 *                    TIMx_CCR(��ƽ�����仯)
 * �ź�����=(TIMx_ARR +1 ) * ʱ������
 * ռ�ձ�=TIMx_CCR/(TIMx_ARR +1)
 */
static void BLDC_TIMx_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
  
  /* ��ʱ����������ʼ�� */		 
  TIM_TimeBaseStructure.TIM_Period = BLDC_TIM_PERIOD; 
  /* ����Ԥ��Ƶ����Ԥ��Ƶ����Ϊ72MHz */
  TIM_TimeBaseStructure.TIM_Prescaler = BLDC_TIM_PRESCALER;
  /* ����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���) */
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
  /* ���ϼ���ģʽ */
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  /* �ظ������� */
  TIM_TimeBaseStructure.TIM_RepetitionCounter = BLDC_TIM_REPETITIONCOUNTER;
  TIM_TimeBaseInit(BLDC_TIMx, &TIM_TimeBaseStructure);

  /* ��ʱ�����ͨ��1ģʽ���� */
  /* ģʽ���ã�PWMģʽ1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  /* ���״̬���ã�ʹ����� */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
  /* ����ͨ�����״̬���ã�ʹ����� */
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  /* ��������ֵ�������������������ֵʱ����ƽ�������� */
  TIM_OCInitStructure.TIM_Pulse = 0;
  /* ����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ */
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity= TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
  /* ��ʼ����ʱ��ͨ��1���PWM */
  TIM_OC1Init(BLDC_TIMx, &TIM_OCInitStructure);
  
  /* ��ʱ�����ͨ��2ģʽ���� */
  /* ����ͨ��2�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM */
  TIM_OCInitStructure.TIM_Pulse = 0;
  /* ��ʼ����ʱ��ͨ��2���PWM */
  TIM_OC2Init(BLDC_TIMx, &TIM_OCInitStructure);

  /* ��ʱ�����ͨ��3ģʽ���� */
  /* ����ͨ��3�ĵ�ƽ����ֵ���������һ��ռ�ձȵ�PWM */
  TIM_OCInitStructure.TIM_Pulse = 0;
  /* ��ʼ����ʱ��ͨ��3���PWM */
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
  
  /* ʹ�ܶ�ʱ�����ؼĴ���ARR */
  TIM_ARRPreloadConfig(BLDC_TIMx, ENABLE);

  /* ʹ�ܶ�ʱ�� */
  TIM_Cmd(BLDC_TIMx, ENABLE);
  
  /* TIM�����ʹ�� */
  TIM_CtrlPWMOutputs(BLDC_TIMx, ENABLE);
  
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
  TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
  TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
}

/**
  * ��������: TIMx ���PWM�źų�ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����ֻҪ�����������TIMx���ĸ�ͨ���ͻ���PWM�ź����
  */
void BLDC_TIMx_PWM_Init(void)
{
	BLDC_TIMx_GPIO_Config();
	BLDC_TIMx_Configuration();	
}


/**
  * ��������: ��ˢ��������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
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
  * ��������: ϵͳ�δ�ʱ���ص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: 1ms���иú���һ��
  */
void HAL_SYSTICK_Callback(void)
{
  static uint16_t time_count=0;
  
  bldc_dev.stalling_count++;  
  if(bldc_dev.motor_state==RUN)
  {
    time_count++;    
    if(bldc_dev.stalling_count>2000) // �����ס��ʱ
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
    // bldc_dev.step_counter��¼������������50msʱ���ڲ���������������������תһȦ�����ܹ�
    // (MOTOR_POLE_PAIR_NUM*6)������,ʹ�� n=bldc_dev.step_counter/(MOTOR_POLE_PAIR_NUM*6) Ϊ
    // 50ms�ڵ��ת��Ȧ����Ϊ����Ϊ��ת/����(rpm)��
    //  n/50 = x/(60*1000) --> x=bldc_dev.step_counter*50
    temp=bldc_dev.step_counter*50;
    pid_result=IncPIDCalc(temp);     // ��������
		
    pid_result =pid_result*10/25;    // *10/25Ϊת�ٺ�ռ�ձ�һ��ת����ת�٣�0~2500����ռ�ձȣ�0~1000��
		
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
  uint16_t hall_read = (HALL_GPIO->IDR)&0x01c0; // ��ȡ������������Ϣ
  
  if(bldc_dev.motor_state==STOP)return;
  
  uwStep=hall_read>>6;
  if(bldc_dev.motor_direction == CW) // �����ж�
    uwStep=7-uwStep;    

  BLDC_PHASE_CHANGE(uwStep); //��������
}


/**
  * ��������: ��ʱ���жϷ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void HALL_TIMx_Callback(void)
{
  uint8_t pinstate=0;
  static int8_t pinstate0=0;

  if(bldc_dev.motor_state==STOP)return;

  if((GPIOC->IDR & GPIO_Pin_6) != (uint32_t)Bit_RESET)  //����������״̬��ȡ
  {
    pinstate |= 0x01;
  }
  if((GPIOC->IDR & GPIO_Pin_7) != (uint32_t)Bit_RESET)  //����������״̬��ȡ
  {
    pinstate |= 0x02;
  }
  if((GPIOC->IDR & GPIO_Pin_8) != (uint32_t)Bit_RESET)  //����������״̬��ȡ
  {
    pinstate |= 0x04;
  }
  if(bldc_dev.motor_direction==CW) // �����ж�
    pinstate=7-pinstate;

  BLDC_PHASE_CHANGE(pinstate);//��������  
  
  if( pinstate0!=pinstate) // ���Է�����ʱ����������������ͬ�����ݣ������˵��ظ���
  {    
    bldc_dev.step_counter++;    // ��������
    bldc_dev.stalling_count=0;  // ������������
  }
  pinstate0=pinstate;

}


