#include "key.h"
#include "delay.h"
#include "led.h"


//按键初始化函数
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOA时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();           //开启GPIOE时钟
    
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //下拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
	GPIO_Initure.Pin=GPIO_PIN_13; //PC13
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLDOWN;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;     //按键松开标志
    if(mode == 1)key_up = 1;    //支持连按
    if(key_up && (KEY1 == 1|| KEY2 == 1))
    {
        delay_ms(10);
        key_up=0;
        if(KEY1 == 1)       return KEY1_PRES;
        else if(KEY2 == 1)  return KEY2_PRES;       
    }else if(KEY1 == 0 && KEY2 == 0)key_up=1;
    return 0;   //无按键按下
}



void TestSelf(void)
{
    switch (KEY_Scan(0))
    {
    case 1:
        LEDR(0);LEDG(1);LEDB(0);
        break;

    case 2:
        LEDR(0);LEDG(0);LEDB(1);
        break;

    default:
        break;
    }
}



