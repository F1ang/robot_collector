#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h" 
#include "touch.h"
#include "btim.h"

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "bsp_sdio_sdcard.h"

#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
//#include "lv_demo_stress.h"
//#include "lv_demo_music.h"
//#include "lv_demo_widgets.h"
#include "lv_mainstart.h"


char SDPath[4];                   /* SD���߼��豸·�� */
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FRESULT f_res;                    /* �ļ�������� */

int main(void)
{	
	HAL_Init();                    	 	//��ʼ��HAL��    
	Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
	
 	ILI9341_Init();             		//LCD ��ʼ��
	XPT2046_Init();
	ILI9341_GramScan (6);	
	my_mem_init(SRAMIN);				//��ʼ���ڲ��ڴ��
	
	if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
  {
    //��SD�������ļ�ϵͳ���ļ�ϵͳ����ʱ���SD����ʼ��
    f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
		 if(f_res!=FR_OK)
    {
			LEDR(1);
      printf("����SD�������ļ�ϵͳʧ�ܡ�(%d)\r\n",f_res);
    }
	}
	
	
	btim_timx_int_init(10-1,7200-1);    //��ʼ����ʱ��
	
	lv_init(); /* lvgl ϵͳ��ʼ�� */
	lv_port_disp_init(); /* lvgl ��ʾ�ӿڳ�ʼ��,���� lv_init()�ĺ��� */
	lv_port_indev_init(); /* lvgl ����ӿڳ�ʼ��,���� lv_init()�ĺ��� */

	//lv_demo_stress();
	//lv_demo_music();
	//lv_demo_widgets();
	lv_mainstart(); 
	while(1)
	{			
		lv_timer_handler(); /* LVGL �������൱�� RTOS ����������Ⱥ��� */
	}
}


