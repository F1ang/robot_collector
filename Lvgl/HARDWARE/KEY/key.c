#include "key.h"
#include "delay.h"
#include "led.h"


//������ʼ������
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOAʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();           //����GPIOEʱ��
    
    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
    GPIO_Initure.Pull=GPIO_PULLDOWN;        //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
	GPIO_Initure.Pin=GPIO_PIN_13; //PC13
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
    GPIO_Initure.Pull=GPIO_PULLDOWN;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
}

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>WK_UP!!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;     //�����ɿ���־
    if(mode == 1)key_up = 1;    //֧������
    if(key_up && (KEY1 == 1|| KEY2 == 1))
    {
        delay_ms(10);
        key_up=0;
        if(KEY1 == 1)       return KEY1_PRES;
        else if(KEY2 == 1)  return KEY2_PRES;       
    }else if(KEY1 == 0 && KEY2 == 0)key_up=1;
    return 0;   //�ް�������
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



