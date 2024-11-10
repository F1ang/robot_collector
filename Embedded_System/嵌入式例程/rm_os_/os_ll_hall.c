/*一、USART及DMA相关*/
/*二、can相关*/
/*三、LL_API*/
/*四、裁判系统和UI*/
/*五、Freertos*/


/*一、USART及DMA相关*/ 

//1、clear flag
	USART3->SR;
	USART3->DR;
//2、RM——remote	 
RC_Ctl_t RC_Ctl;
uint8_t sbus_rx_buffer[18];
int TongDao0=0;
//RC数据解析
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *rx_buffer)
{
	RC_Ctl.rc.ch0 = (sbus_rx_buffer[0] | (sbus_rx_buffer[1] << 8))& 0x07ff;  //确保11位
	RC_Ctl.rc.ch1 = ((sbus_rx_buffer[1] >> 3) | (sbus_rx_buffer[2] << 5))& 0x07ff;
	RC_Ctl.rc.ch2 = ((sbus_rx_buffer[2] >> 6) | (sbus_rx_buffer[3] << 2) | (sbus_rx_buffer[4] << 10))& 0x07ff;
	RC_Ctl.rc.ch3 = ((sbus_rx_buffer[4] >> 1) | (sbus_rx_buffer[5] << 7))& 0x07ff;
	RC_Ctl.rc.s1  = ((sbus_rx_buffer[5] >> 4) & 0x000C) >> 2;
	RC_Ctl.rc.s2  = ((sbus_rx_buffer[5] >> 4) & 0x0003);
	
	RC_Ctl.mouse.x = sbus_rx_buffer[6] | (sbus_rx_buffer[7] << 8);
	RC_Ctl.mouse.y = sbus_rx_buffer[8] | (sbus_rx_buffer[9] << 8);
	RC_Ctl.mouse.z = sbus_rx_buffer[10] | (sbus_rx_buffer[11] << 8);
	RC_Ctl.mouse.press_l = sbus_rx_buffer[12];
	RC_Ctl.mouse.press_r = sbus_rx_buffer[13];
	RC_Ctl.key.v = sbus_rx_buffer[14] | (sbus_rx_buffer[15] << 8);
	
}
  while (1)
  {
	if(RC_Ctl.rc.s2 == 1)  // 开关向上
	{
			//自己想干嘛--串口调试
	}

//3、串口相关:LL和HAL  
#include <stdio.h>   
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart6, (uint8_t *)&ch, 1, 0xffff);
  return ch;
} 
int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart6, &ch, 1, 0xffff);
  return ch;
}

 while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
		 if(recv_end_flag == 1)  //接收完成标志
		{
			DMA_Usart_Send(rx_buffer, rx_len);
			rx_len = 0;//清除计数
			recv_end_flag = 0;//清除接收结束标志位
		    memset(rx_buffer,0,rx_len);//清接收缓存
		}
		HAL_UART_Receive_DMA(&huart1,rx_buffer,BUFFER_SIZE);//重新打开DMA接收
  } 
                              /*LL库*/
  int fputc(int ch, FILE *f)
{

	LL_USART_TransmitData8(USART1,ch);
	while(!LL_USART_IsActiveFlag_TC(USART1))
	{
	}
	return ch;
}
void USART_RxIdleCallback(void)
{
		uint8_t i;
		if(LL_USART_IsActiveFlag_IDLE(USART1))			
		{			
			//先清标志位			
			LL_USART_ClearFlag_IDLE(USART1); 											
			if(UartRxDmaBuf[2] == 0xEE)			
			{			
				NVIC_SystemReset();			
			}		
			//先停止UART流DMA，暂停接收		
			LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2); 			
			/* 3.搬移数据进行其他处理 */			
			for(i=0; i<8; i++)
			{			
				UartRxBuf[i] = UartRxDmaBuf[i];
			}								
			UartRxflag = 1; //标志已经成功接收到一包等待处理		
			LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 8);		
			/* 4.开启新的一次DMA接收 */	
			LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);							
		}
}

/*二、can相关*/
//过滤器实现 CAN_RX回调(操作系统这时得通知数据解析任务) CAN_send  迭代接收数据的句柄和BUFF
//1、can理论学习
DBUS：100000bits/s  CAN：1Mbps  裁判：115200bit/s  
1-数据位分解：Tq,四段划分8~25个Tq，采样点-->bit/s，即波特率(1码元=1bit).    注：32的位时序只三段，APB1，BaudRate=1/(N*Tq)
2-报文：数据，遥控，错误，过载，帧间隔 .    数据帧：标准(ID_11)及拓展(ID_29)--->区分仲裁段，控制段，数据段，CRC段，ACK段    
3-ID手动分配，CAN控制器据ID过滤报文，还与优先级有关.  数据段：0~8byte
4-3个发送邮箱，0~n个过滤器，2个3级深度接收邮箱，使用can2使必使能can1
5-收发邮箱：ID,IDE,RTR,DLC，DATA 
6-验收筛选器：内容定ID，0~27，模式及位宽  注：主0~n，则从n~27，掩码和标识符模式  ID-MASK->接收报文
7-CAN_InitTypeDef，CanTxMsg及CanRxMsg，CAN_FilterInitTypeDef
8、Init_struct:CAN_Prescaler--时间片Tq的长度，SJW，BS1，BS2
9、TxMsg:StdId,ExtId,IDE,RTR,DLC,Data[8],FMI--------RxMsg:同上
10、Filter：16bit，接收FIFO，FilterMode，FilterNumber，

(1)、CAN_InitTypeDef:CAN_Prescaler决定时间片Tq的长度，SJW，BS1，BS2.
(2)、TxMsg:StdId,ExtId,IDE,RTR,DLC,Data[8]填充.
(3)、Filter：16bit的ID过滤4组，接收FIFO，FilterMode，FilterNumber.
(4)、CAN_Prescaler=4->1个数据为时序有SJW=1Tq,BS8=8Tq，BS1=9Tq.
(5)、Tq->36m/4=9mhz, 18个Tq则9m/18=500kbps
void Can1_Send_4Msg(uint32_t id, int16_t data1, int16_t data2, int16_t data3, int16_t data4)
{
	static CAN_TxHeaderTypeDef can_tx_msg_format;
	static uint8_t can_tx_msg[8];
	static uint32_t send_mail_box;
	can_tx_msg_format.StdId = id;
	can_tx_msg_format.IDE = CAN_ID_STD;
	can_tx_msg_format.RTR = CAN_RTR_DATA;
	can_tx_msg_format.DLC = 0x08;
	can_tx_msg[0] = data1 >> 8;
	can_tx_msg[1] = data1;
	can_tx_msg[2] = data2 >> 8;
	can_tx_msg[3] = data2;
	can_tx_msg[4] = data3 >> 8;
	can_tx_msg[5] = data3;
	can_tx_msg[6] = data4 >> 8;
	can_tx_msg[7] = data4;
	///< use HAL function send
	HAL_CAN_AddTxMessage(&hcan1, &can_tx_msg_format, can_tx_msg, &send_mail_box);
}
	//CAN过滤器配置
	CAN1_FILTER_CONFIG(&hcan1);                                                     
	CAN2_FILTER_CONFIG(&hcan2);
	 //CAN中断使能：can1和can2公用回调
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);             
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);

1)当需要使用CAN发送报文时，先定义一个上面发送类型的结构体，然后把报文的内容按成员赋值到该结构体中，最后调用库函数CAN_Transmit把这
些内容写入到发送邮箱即可把报文发送出去。
2)接收报文时，通过检测标志位获知接收FIFO的状态，若收到报文，可调用库函数CAN_Receive把接收FIFO中的内容读取到预先定义的接收类型结构
体中，然后再访问该结构体即可利用报文.

3)标识符列表模式，它把要接收报文的ID列成一个表，要求报文ID与列表中的某
一个标识符完全相同才可以接收，可以理解为白名单管理。
4)掩码模式，它把可接收报文ID的某几位作为列表，这几位被称为掩码，可以把
它理解成关键字搜索，只要掩码(关键字)相同，就符合要求，报文就会被保存到
接收FIFO。

->如果使能了筛选器，且报文的ID与所有筛选器的配置都不匹配，CAN外设会
丢弃该报文，不存入接收FIFO。

//2、spi：用来IMU的通讯
//128psc，

//3、encoder
/************************************************************
*
*			encoder key detect is norm or fast
*
*************************************************************/
#define MSG_ENC_RIGHT    Set_Bit0
#define MSG_ENC_LEFT     Set_Bit1
#define CMP_VALUE    (500)
#define FAST_RANK_2  (200)
#define FAST_RANK_1  (30)
unsigned char enc_speed_grade;
unsigned char Enc_GetSpeed(void)
{
	return enc_speed_grade;
}
unsigned short int Encoder_Process(void)
{
	static unsigned char flag = 0; 
	unsigned short int enc_value;		
	if(LL_TIM_GetCounter(TIM4) == CMP_VALUE) //获取定时器的值
	{
		flag = 0;
		enc_speed_grade = ENC_SPEED_0;
		return 0;
	}
	if((LL_TIM_GetCounter(TIM4)) > CMP_VALUE)
	{
		enc_value = MSG_ENC_RIGHT;
		LL_TIM_SetCounter(TIM4, CMP_VALUE);

	}else if(LL_TIM_GetCounter(TIM4) < CMP_VALUE){

		enc_value = MSG_ENC_LEFT;
		LL_TIM_SetCounter(TIM4, CMP_VALUE);		
	}
	if(!flag)
	{
		flag = 1;
		Tim_ms = 0;
		return enc_value;
	}
	if(Tim_ms < FAST_RANK_2)
	{
		enc_speed_grade = ENC_SPEED_2;

		if(Tim_ms <= FAST_RANK_1)
		{
			enc_speed_grade = ENC_SPEED_1;
		}
		Tim_ms = 0;
	}else{	
		enc_speed_grade = ENC_SPEED_0;		
	}
	return enc_value;
}

/*三、LL_API*/
	
	/*
	* 1,LL库首先要确保外设的  pin复用、时钟配置、中断配置/功能开启、外设启用。  
	* 2,
	*/

  //pin复用
  LL_GPIO_SetAFPin_0_7(GPIOF, LL_GPIO_PIN_7, LL_GPIO_AF_5); //SPI5
  LL_GPIO_SetAFPin_8_15(GPIOF, LL_GPIO_PIN_8, LL_GPIO_AF_5);//SPI5

  //RX
  LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)(&USART1->DR));
  LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)rc_rx_buf0);
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 8);

  LL_DMA_DisableStream(DMA2,LL_DMA_STREAM_2);	//双缓冲模式，待定
  LL_DMA_ConfigAddresses  (DMA2,LL_DMA_STREAM_2,(uint32_t)(&USART1->DR),(uint32_t)rc_rx_buf1,LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetCurrentTargetMem  (DMA2,LL_DMA_STREAM_2,LL_DMA_CURRENTTARGETMEM0);
  LL_DMA_EnableDoubleBufferMode  (DMA2,LL_DMA_STREAM_2);	

  LL_USART_EnableIT_RXNE(USART1);   
  LL_USART_EnableIT_IDLE(USART1);   				//注：法一：IDLE+RXNE，法二：BUFF+IDLE ，方法三:BUFF+DMA(选)
	
  LL_USART_EnableDMAReq_RX(USART1);
  LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_2);

  //TX
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_0, (uint32_t)uart8_tx_buf);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_0, (uint32_t)(&UART8->DR));
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, 0);
  LL_DMA_EnableStream(DMA1,LL_DMA_STREAM_0);
  LL_USART_EnableDMAReq_TX(UART8);

  LL_DMA_ClearFlag_TC0(DMA1);
  LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_0);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, uart8_tx_length);
  LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
 

//tim-pwm
  LL_TIM_CC_EnableChannel(TIM1,LL_TIM_CHANNEL_CH1);
  LL_TIM_EnableCounter(TIM1);
  LL_TIM_EnableAllOutputs(TIM1);

  LL_TIM_OC_SetCompareCH1(TIM1,Plus);//占空比
  LL_mDelay(100);

void DMA2_Stream2_IRQHandler(void)
{
	if(LL_DMA_IsActiveFlag_TC2(DMA2))
	{
			LL_DMA_ClearFlag_TC2(DMA2);
		LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_2);//add
		
		Rc_Data_Update(); 
	
		//debug
		LED_RED_ON;
		LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, 18);	
		LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);		
	}
}

四、裁判系统
frame_header (5-byte) +   cmd_id (2-byte)  + data (n-byte)    + frame_tail (2-byte，CRC16，整包校验)  
128 byte  , so 128-9-ID*3 = 113 byte。
ID*3=数据端的内容ID,发送者ID,接收者ID.
1、
（1）frame_header:1-byte 2-byte 1-byte 1-byte
 SOF：0xA5，  data_length：2 byte   seq:1 byte 包序号  CRC8：1 byte
typedef __head struct
{
	uint8_t SOF_hll;
	uint16_t Data_length_hll;
	uint8_t Seq_hll;
	uint8_t CRC8_hll;
}head_frame;

（2）cmd_id:命令
机器人血量数据：0x0003 , 场地事件数据：0x0101 , 补给站动作标识：0x0102 ,
比赛机器人状态：0x0201 , 机器人 RFID 状态：0x0209 , 子弹剩余量：0x0208 ,
 实时功率热量数据：0x0202 ，图传遥控信息标识：0x0304
交互数据接收信息：0x0301-->ID*3+数据内容(113 byte)
机器人ID:2，工程(红)102，工程(蓝) ; 客户端(红)0x0102;客户端(蓝)0x0166

（3）data (n-byte)：
相对id的字节偏移及大小和数据（地址指针和结构体实现指针运算）
(注：在处理UI中，6 byte ID,113 byte data)

如：typedef __packed struct//先给id->ext_game_status_t
{
 uint8_t game_type : 4;			
 uint8_t game_progress : 4;		//4bit
 uint16_t stage_remain_time;	//1byte
uint64_t SyncTimeStamp;			//3byte
} ext_game_status_t;

如:
ext_game_status_t *game_status_t;
game_status_t=&(0x0201)
game_status_t->

（4）frame_tail (2-byte，CRC16，整包校验)  

2、
STM32F427:收->选->发（UI处理）
图传：一发			
发->服务器（监听到）->UI（客户端）

3、UI(车间通信)
机器人间交互数据--发:0x0301
128byte->ID*3+数据内容(113 byte).

(1)关键：内容ID/发送者ID/接收者ID:=ID*3 = 6 byte
typedef __packed struct
{
 uint16_t data_cmd_id;//关键：内容ID
 uint16_t sender_ID;
 uint16_t receiver_ID;
}ext_student_interactive_header_data_t;

(2)交互数据：内容ID：0x0200~0x02FF，已方
注：校验ID
typedef __pack struct
{
uint8_t data[]
} robot_interactive_data_t

(3)客户端删除图形 内容ID=0x0100
typedef __packed struct
{
uint8_t operate_tpye; 
uint8_t layer; 
} ext_client_custom _graphic_delete_t

(4)图形数据  内容数据段--max=113
typedef __packed struct
{ 
uint8_t graphic_name[3]; 
uint32_t operate_tpye:3; 
uint32_t graphic_tpye:3; 
uint32_t layer:4; 
uint32_t color:4; 
uint32_t start_angle:9;
uint32_t end_angle:9;
uint32_t width:10; 
uint32_t start_x:11; 
uint32_t start_y:11; 
uint32_t radius:10; 
uint32_t end_x:11; 
uint32_t end_y:11; 
} graphic_data_struct_t

(5)客户端绘制一个图形 内容ID=0x0101
typedef __packed struct
{
 graphic_data_struct_t grapic_data_struct;
} ext_client_custom_graphic_single_t;

(6)客户端绘制二个图形  内容ID=0x0102
typedef __packed struct
{
graphic_data_struct_t grapic_data_struct[2];
} ext_client_custom_graphic_double_t;

注:最多可绘制7个.

(7)客户端绘制字符  内容ID=0x0110
typedef __packed struct
{
graphic_data_struct_t grapic_data_struct;
uint8_t data[30];
} ext_client_custom_character_t;

机器人间交互数据-收:0x0302
typedef __pack struct
{
uint8_t data[]
} robot_interactive_data_t

注：最多为30

(8))客户端下发键鼠信息

5、
一、接收：
找帧头，校验帧头，校验整包数据，解析数据(依据id->地址指针配合结构体，完成解析)

二、UI发：
（1）填帧
get/set 客户端和机器人端ID，    
填帧头，填cmd_id，填操作ID（内容ID等），填数据段（即内容ID下struct），
整包校验，
包序号+1，
注：可以进行拷贝各帧段

（2）填图形数据（同为填）

（3）推送：1、图形：整合帧，校验，发送   2、字符：同上


五、比赛相关
1、MODE:S-绝对 S1-底盘 S2-云台
常规：V =[Vx,Vy,Wv]	分解得（注：系数1~20），
 V1=Vx∗kx−Vy∗ky−ωv∗kω
 V2=Vx∗kx+Vy∗ky−ωv∗kω
 V3=Vx∗kx−Vy∗ky+ωv∗kω
 V4=Vx∗kx+Vy∗ky+ωv∗kω
 	wheel_rpm[0] = (-mec->speed.vx - mec->speed.vy - mec->speed.vw * rotate_ratio_fr) * wheel_rpm_ratio;
  	wheel_rpm[1] = (mec->speed.vx - mec->speed.vy - mec->speed.vw * rotate_ratio_fl) * wheel_rpm_ratio;
 	wheel_rpm[2] = (mec->speed.vx + mec->speed.vy - mec->speed.vw * rotate_ratio_bl) * wheel_rpm_ratio;
  	wheel_rpm[3] = (-mec->speed.vx + mec->speed.vy - mec->speed.vw * rotate_ratio_br) * wheel_rpm_ratio;
	
小陀螺：底盘360°旋转的同时，依然正常的前后左右平动的。
（注：因为YAW轴电机和底盘电机相对固定，当指定云台角度，此为绝对角度，双轴pid使角度不变，即云台和YAW轴电机不动，此时底盘按圆转）
Vx1=Vx2∗cosθ−Vy2∗sinθ   Vy1=Vx2∗sinθ+Vy2∗cosθ（即可转的同时左右前后平动）->代入Vx=Vx1,Vy=Vy1
（θ=绝对角度，且正弦运算，大小恒定，方向为圆）

随动：  云台YAW轴电机,期望绝对角度->pid输出角度->期望速度->pid输出速度    
	 底盘和云台YAW轴的期望相对夹角->pid输出角度->期望速度->pid输出速度->V分解到底盘->      底盘动->云台动->不断校正夹角，直到为0.

//角度过零处理
	if((Mileage->mechanical_angle - Mileage->Last_mechanical_angle)> 4096)
			Mileage->round_cnt--;
	else if ((Mileage->mechanical_angle - Mileage->Last_mechanical_angle)< -4096)
			Mileage->round_cnt++;
	Mileage->total_angle = Mileage->round_cnt * 8192 + Mileage->mechanical_angle - Mileage->offest_angle;
	delta=Mileage->Last_mechanical_angle-Mileage->total_angle;

armor:装甲  RFID:无线射频识别  reserve:保留  Comparable:机器人

六、Freertos

1、替换------>#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
2、任务创建函数
BaseType_t xTaskCreate(
				TaskFunction_tp vTaskCode,   //函数指针
				const char * constpcName,    //任务描述
				unsigned short usStackDepth, //堆栈大小
				void *pvParameters,          //参数指针
				UBaseType_t uxPriority,      //任务优先级
				TaskHandle_t *pvCreatedTask  //回传句柄
				);
任务优先级,任务堆栈大小,保存任务句柄(保存任务堆栈),任务函数-----声明信号量句柄,创建信号量,使用信号量				
如:
	//创建遥控任务
	xTaskCreate((TaskFunction_t )Remoter_Task,             
							(const char*    )"remoter_task",
							(uint16_t       )256,
							(void*          )NULL,
							(UBaseType_t    )15,
							(TaskHandle_t*  )&RemoterTask_Handler);
声明队列句柄：osMessageQId myQueue01Handle;	//信息队列
创建消息队列： myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);+queue_size
使用队列：入队xQueueSend( xQueue, pvItemToQueue, xTicksToWait ); 出队
3、补充
1)数字越小,优先级越低
2)任务控制块:结构体管理任务属性(xTaskCreate()自动分配给任务,即有地址指针)---TCB->STACK图.
3)任务堆栈:进行上下文切换时的现场保护,即存储.且存于任务句柄地址指针.  
4)动态/静态创建任务->指定RAM分配给任务控制块和任务堆栈的方式.
5)ROM:stack（栈）:函数信息；Heap（堆）：动态分配的空间.
如:
osThreadId judge_taskHandle;//保存任务句柄(现场保护)
uint32_t judge_taskBuffer[ 256 ];//指定任务堆栈的RAM
osStaticThreadDef_t judge_taskControlBlock;//指定任务控制块的RAM  （静态队列、信号量等也为结构体管理任务属性)

5)任务切换:中断级ISR->SysTick中断来抢占CPU(也会进行PendSV任务切换)->ISR继续执行->PendSV执行上下文切换(防ISR被耽搁)   
任务级-->SVC->PendSV执行上下文切换  
6)内核控制函数(!):任务切换,开启任务调度等---可裁剪
7)任务属性/任务状态信息API(!):获取优先级等---可裁剪
8)申请动态内存pvPortMalloc()   释放vPortFree()
9)系统时钟节拍--systick--xTickCount
10)消息队列(动态/静态)-消息数和大小,静态:手动将RAM存储区分配给队列堆栈和队列控制块,队列与阻塞时间.   
   二值信号量-队列为1, 计数信号量-队列大于1.  
   优先级翻转->互斥信号量
   软件定时器:APP_api->TIM命令队列->TIM服务任务->回调函数,复位软件定时器API(打断此次定时进程,并从打断点复位),创建/启停API,ps:本质为相对立发送cmd,所以必涉及阻塞延时.
   事件标志组-事件标志位:事件行为是否发生,8/24组事件,自行分配RAM,操作事件标志位->事件标志组->等待相应事件标志位->同步事件.
11）任务通知:接收端-任务通知值更新方法->不覆写、覆写、bit、+1----队列(仅1个32位数据,长度1)、二值、事件组、计数信号量<-获取任务通知API配合实现. P313
发送任务通知端可操作原接收任务通知值,操作->发送任务通知API:带通知值是(否)覆盖,不带通知值覆盖(原接收任务接受值+1).
二值信号量->任务通知清零(接收api).  计数型信号量->(可利用)资源管理-1.  队列-发api带值且覆写,收api入前不清出清.    
事件标志组-发api带值且指定bit,收api入前不清出清(任务通知可等于事件组)  
保留原任务通知值,指接受任务的任务通知值记录下这次任务通知值,方便下次处理.

本质:任务通知更新接收任务的任务通知值(即任务控制块的一个32位成员变量).
static:任务/队列句柄-保存用于现场恢复->任务/队列堆栈-提供RAM,入出栈    任务/队列控制块-提供RAM,记录属性状态(保存相对应结构体).
堆栈和控制块:(ram的分配,分配的ram的控制-保存相对应结构体)
注：说明于RM--FreeRTOS.text


字节偏移量3，占位数大小.
typedef __packed struct
{ 
uint8_t graphic_name[3]; 
uint32_t operate_tpye:3; 
uint32_t graphic_tpye:3; 
uint32_t layer:4; 
uint32_t color:4; 
uint32_t start_angle:9;
uint32_t end_angle:9;//占满32bit

uint32_t width:10; 
uint32_t start_x:11; 
uint32_t start_y:11; //占满32bit

uint32_t radius:10; 
uint32_t end_x:11; 
uint32_t end_y:11; //占满32bit
} graphic_data_struct_t

七、DWT内核时钟
文件如bsp_dwt.c,bsp_dwt.h
