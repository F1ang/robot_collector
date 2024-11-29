/*
 * @Welcome: Hacker
 * @Author: Ӳʯ�Ƽ�
 */
#include "bsp_hall.h"

/**
  * ��������: �������������ų�ʼ��.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����ʹ�ú궨�巽������������źţ����������ֲ��ֻҪ���޸�bsp_HALL.h
  *           �ļ���غ궨��Ϳ��Է����޸����š�
  */
void HALL_GPIO_Init(void)
{
   /* ����IOӲ����ʼ���ṹ����� */
  GPIO_InitTypeDef GPIO_InitStructure;
  /* �����ⲿ�ж��߳�ʼ���ṹ����� */
  EXTI_InitTypeDef EXTI_InitStructure;
  /* ����Ƕ�������жϿ�������ʼ���ṹ����� */
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ϊ����IO�����жϹ�����Ҫʹ�ܸ��ù���ʱ�� */
  RCC_APB2PeriphClockCmd(HALL_RCC_CLOCKGPIO | HALL_RCC_CLOCKAFIO,ENABLE);
  
  /* �趨����IO��� */
  GPIO_InitStructure.GPIO_Pin = HALL_U_GPIO_PIN;  
  /* �趨����IOΪ����ģʽ */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  /* ��ʼ��KEY2��Ӧ����IO */
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = HALL_V_GPIO_PIN; 
  
  GPIO_InitStructure.GPIO_Pin = HALL_W_GPIO_PIN; 
  GPIO_Init(HALL_GPIO, &GPIO_InitStructure);
  
  /* ѡ����Ϊ�ж�����Դ */
  GPIO_EXTILineConfig(HALL_GPIO_PORTSOURCE,HALL_U_GPIO_PINSOURCE);
  GPIO_EXTILineConfig(HALL_GPIO_PORTSOURCE,HALL_V_GPIO_PINSOURCE);
  GPIO_EXTILineConfig(HALL_GPIO_PORTSOURCE,HALL_W_GPIO_PINSOURCE);
  
  /* ��Ӧ���ж��� */
  EXTI_InitStructure.EXTI_Line=HALL_U_EXITLINE;
  /* �ⲿ�ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  /* �½��ش�����ʽ */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  /* ʹ���ж� */
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  /* ����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ��� */
  EXTI_Init(&EXTI_InitStructure);
  
  EXTI_InitStructure.EXTI_Line=HALL_V_EXITLINE;
  EXTI_Init(&EXTI_InitStructure);
  
  EXTI_InitStructure.EXTI_Line=HALL_W_EXITLINE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* ʹ���ⲿ�ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannel = HALL_EXTI_IRQn;
  /* ������ռʽ���ȼ�Ϊ1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  /* ���������ȼ�Ϊ1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  /* ʹ���ⲿ�ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
  /* ��ʼ������Ƕ�������жϿ����� */
  NVIC_Init(&NVIC_InitStructure); 
}


/**
  * ��������: �������������ų�ʼ��.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����ʹ�ú궨�巽������������źţ����������ֲ��ֻҪ���޸�bsp_HALL.h
  *           �ļ���غ궨��Ϳ��Է����޸����š�
  */
static void HALL_TIMx_GPIO_Init(void)
{
   /* ����IOӲ����ʼ���ṹ����� */
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Ϊ����IO�����жϹ�����Ҫʹ�ܸ��ù���ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);
  
  /* �趨����IO��� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  
  /* �趨����IOΪ����ģʽ */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  /* ��ʼ��KEY2��Ӧ����IO */
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  /* ��ʱ��ͨ��������ӳ�� */
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
}

/**
  * ��������: ����Ƕ�������жϿ�����NVIC
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void HALL_TIMx_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* ����TIMΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	/* ������ռʽ���ȼ�Ϊ0 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	/* ���������ȼ�Ϊ0 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	/* ʹ���ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
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
 *    _______    ______     _____      ____       ___        __         _
 * |_|       |__|      |___|     |____|    |_____|   |______|  |_______| |________|
 *
 */
static void HALL_TIMx_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef        TIM_ICInitStructure;

  /* ʹ�ܶ�ʱ��ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  /* ��ʱ����������ʼ�� */		 
  /* ��ʱ����: HALL_TIM_Period+1 */
  TIM_TimeBaseStructure.TIM_Period = 0xFFFF; 
  /* ����Ԥ��Ƶ��HALL_TIM_Prescaler,�������Ƶ�ʣ�72MHz/(HALL_TIM_Prescaler+1)/(HALL_TIM_Period+1) */
  TIM_TimeBaseStructure.TIM_Prescaler = 71;
  /* ����ʱ�ӷ�Ƶϵ��������Ƶ(�����ò���) */
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
  /* ���ϼ���ģʽ */
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* ��ʼ��TIM5���벶����� */
  /* CC1S=01 	ѡ������� IC1ӳ�䵽TI1�� */
	TIM_ICInitStructure.TIM_Channel     = TIM_Channel_1;
  /* �����ز��� */
	TIM_ICInitStructure.TIM_ICPolarity  = TIM_ICPolarity_BothEdge;	
  /* ӳ�䵽TI1�� */
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC; 
  /* ���������Ƶ,����Ƶ  */
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	           
  /* IC1F=0000 ���������˲��� ���˲� */
	TIM_ICInitStructure.TIM_ICFilter    = 0x08;                         
	TIM_ICInit(TIM3, &TIM_ICInitStructure);  
  
  /* ����NVIC */
  HALL_TIMx_NVIC_Configuration();
  
  TIM_SelectHallSensor(TIM3,ENABLE);                          //ʹ��TIMx�Ļ����������ӿ�
  TIM_SelectInputTrigger(TIM3, TIM_TS_TI1F_ED);               	//���봥��Դѡ��     
  
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);             	//��ģʽѡ��
  TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);	//����ģʽѡ��    
   /* ��������ж� ,����CC1IE�����ж�	 */
  TIM_ITConfig(TIM3, TIM_IT_Trigger, ENABLE);
  /* ʹ�ܶ�ʱ�� */
  TIM_Cmd(TIM3, ENABLE);
  TIM_ClearITPendingBit (TIM3,TIM_IT_Trigger);
}

/**
  * ��������: TIMxͨ��1���벶���ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void HALL_TIMx_Init(void)
{
	HALL_TIMx_GPIO_Init();
	HALL_TIMx_Configuration();	
}




