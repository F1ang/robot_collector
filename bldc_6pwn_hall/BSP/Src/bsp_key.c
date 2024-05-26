#include "bsp_key.h"

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

	if (key_up && (KEY1 == 1 || KEY2 == 1)){
		HAL_Delay(30);//去抖动 
		key_up=0;

		if(KEY1 == 1)
      return 1;
		else if (KEY2 == 1)
      return 2;
	}else if (KEY1 == 0 && KEY2 == 0)
    key_up = 1; 	    

 	return 0;// 无按键按下
}





