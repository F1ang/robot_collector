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


char SDPath[4];                   /* SD卡逻辑设备路径 */
FATFS fs;													/* FatFs文件系统对象 */
FRESULT f_res;                    /* 文件操作结果 */

int main(void)
{	
	HAL_Init();                    	 	//初始化HAL库    
	Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口	
	LED_Init();							//初始化LED	
	KEY_Init();							//初始化按键
	
 	ILI9341_Init();             		//LCD 初始化
	XPT2046_Init();
	ILI9341_GramScan (6);	
	my_mem_init(SRAMIN);				//初始化内部内存池
	
	if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
  {
    //在SD卡挂载文件系统，文件系统挂载时会对SD卡初始化
    f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
		 if(f_res!=FR_OK)
    {
			LEDR(1);
      printf("！！SD卡挂载文件系统失败。(%d)\r\n",f_res);
    }
	}
	
	
	btim_timx_int_init(10-1,7200-1);    //初始化定时器
	
	lv_init(); /* lvgl 系统初始化 */
	lv_port_disp_init(); /* lvgl 显示接口初始化,放在 lv_init()的后面 */
	lv_port_indev_init(); /* lvgl 输入接口初始化,放在 lv_init()的后面 */

	//lv_demo_stress();
	//lv_demo_music();
	//lv_demo_widgets();
	lv_mainstart(); 
	while(1)
	{			
		lv_timer_handler(); /* LVGL 管理函数相当于 RTOS 触发任务调度函数 */
	}
}


