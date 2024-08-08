/*
 * @Welcome: Hacker
 * @Description: Excellent Day
 */
#include "bsp_key.h"

volatile u16 key_delay = 0;
Key_TypeDef key_sta;

/* KEY初始化 */
void Key_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);

  //PA.0-下拉输入 K1
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // K2
  GPIO_InitStructure.GPIO_Pin  =GPIO_Pin_13;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
 * @description: key scan
 * @param {u8} mode : 1支持连按
 * @return {*} K1:1 K2:2
 */
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if (mode) 
    key_up=1;  //支持连按		  

	if (key_up && (KEY1 == 1 || KEY2 == 1)) {
		if (key_delay >= 40) {
         key_delay = 0;
         key_up=0;

         if(KEY1 == 1)
         return 1;
         else if (KEY2 == 1)
         return 2;
      }
	}else if (KEY1 == 0 && KEY2 == 0)
    key_up = 1; 	    

 	return 0;// 无按键按下
}

/* 按键进程 */
/**
 * @description: K1:0:no key, 1:key pressed 2:key pressed     K2:0:stop, 1:up, 2:down 3:mode
 * @return {None}
 */
void Key_Process_Handler(void)
{
   key_sta.key_flag = KEY_Scan(0);

   if (key_sta.key_flag == 1) {
      key_sta.key_stop_up_down+=1;
   } 
   else if (key_sta.key_flag == 2) {
      if (key_sta.key_stop_up_down == 0) {
         State = STOP;
         hSpeed_Reference = 0;
      } 
      else if (key_sta.key_stop_up_down == 1)
         hSpeed_Reference += 200;
      else if (key_sta.key_stop_up_down == 2)
         hSpeed_Reference -= 200;
      else if(key_sta.key_stop_up_down == 3) {
         State = INIT;
      }
      else
         key_sta.key_stop_up_down = 0;
   }
}




