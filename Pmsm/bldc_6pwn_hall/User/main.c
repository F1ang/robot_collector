#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_BLDCTIM.h" 
#include "bsp_hall.h"
#include "bsp_key.h"
#include "bsp_pid.h"
#include "bsp_motor.h"

uint8_t key_cnt = 0;

int main(void)
{ 
  uint8_t key_count = 1; // 1,启动 2,加速 3,减速 4,反转 5,停下
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  SysTick_Init();
  
  Key_Init(); 
  //HALL_GPIO_Init();
  HALL_TIMx_Init();

  BLDC_TIMx_PWM_Init();
  
  IncPIDInit(); 

  /* 无限循环 */
  while (1)
  {    
		key_cnt = key_count;
		
    if(KEY_Scan(0) == 1) // 功能选择
    {
      key_count++;
      if(key_count > 5)
        key_count = 1;
    }
		
    if(KEY_Scan(0) == 2) // 功能执行
    {
      switch(key_count)
      {
        case 1: // 电机启动
          if(bldc_dev.motor_state == STOP)
          {
            bldc_dev.motor_state=RUN;
            bldc_dev.step_counter=0;
            bldc_dev.stalling_count=0;
            if((bldc_dev.motor_speed*10/25)>70)  
              speed_duty=bldc_dev.motor_speed*10/25;// *10/25为转速和占空比一个转换，转速（0~2500），占空比（0~1000）
            else
              speed_duty=70;
            NVIC_EnableIRQ(TIM3_IRQn);  
            HALL_TIMx_Callback();
          }          
          break;
        case 2: // 加速
          bldc_dev.motor_speed+=100;
          if(bldc_dev.motor_speed>MOTOR_MAX_SPEED)
            bldc_dev.motor_speed=MOTOR_MAX_SPEED;
          bldc_pid.SetPoint=bldc_dev.motor_speed; 
          break;
        case 3: // 减速
          bldc_dev.motor_speed-=50;
          if(bldc_dev.motor_speed<MOTOR_MIN_SPEED)
            bldc_dev.motor_speed=MOTOR_MIN_SPEED;
          bldc_pid.SetPoint=bldc_dev.motor_speed; 
          break;
        case 4: // 方向反转
          if(bldc_dev.motor_direction == CW)
            bldc_dev.motor_direction = CCW;
          else
            bldc_dev.motor_direction = CW;
          break;
        case 5: // 停机
            TIM_ClearITPendingBit (TIM3,TIM_IT_Trigger); 
            NVIC_DisableIRQ(TIM3_IRQn);
            TIM_CCxCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
            TIM_CCxCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
            TIM_CCxCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCx_Disable);
            TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
            HAL_Delay(5);
            TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Enable);
            TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Enable);
            TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Enable);
            while(bldc_dev.stalling_count<1600);
            TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_1,TIM_CCxN_Disable);
            TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_2,TIM_CCxN_Disable);
            TIM_CCxNCmd(BLDC_TIMx,TIM_Channel_3,TIM_CCxN_Disable);
            bldc_dev.motor_state=STOP;
          break;
      }
           
    }
  }
}

