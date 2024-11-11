Plane:
/*一、线程实现*/
/*二、理论学习*/
/*三、路径规划*/
/*四、Remote相关*/


一、线程实现
1、sbus协议
2、ppm协议
3、kalaman

二、理论学习
1、姿态运动学：扭矩与反扭矩，  螺旋桨：7045，螺距4.5英寸，直径7.0英寸，
2、电机：有刷电机，电磁铁与三相绕组（电刷->ABC），  无刷电机，电磁铁是固定，无刷电调->ABC->永磁转子 ，2212，定子直径22mm，定子厚度12mm ，KV：动力（反比）和转速  ，
3、电调：电压调节器，电调可以通过接收PWM信号来将输入的电源转为不同的电压，并输出到电机，PWM控制频率越高，其周期越短，控制间隔也就越短，电调和电机响应速度也就越快。
4、有刷电调：以通过内部电路来改变输出电流的方向，从而达到电机不同方向的转动。
5、无刷电调：将直流电源转为三相交流电，并可以通过改变输出电压，从而改变无刷电机转动的速度。与有刷电调不同的是，无刷电调不能改变电机的转动方向，要改变无刷电机转动方向只需要将电机的三根电源线的任意两根反接即可。
6、电机安全保护，最大最小形成校准，
7、电池：LiPo，c：放电倍率，定放电电流大小，mA：储能大小  ，负载的电流消耗（即功率），
8、
pwm：需要在接收机上接上全部pwm输出通道，每一个通道就要接一组线，解析程序需要根据每一个通道的pwm高电平时长计算通道数值。
ppm：按固定周期发送所有通道pwm脉宽的数据格式，一组接线，一个周期内发送所有通道的pwm值，解析程序需要自行区分每一个通道的pwm时长。
sbus：每11个bit位表示一个通道数值的协议，串口通信，但是sbus的接收机通常是反向电平，连接到无人机时需要接电平反向器，大部分支持sbus的飞行控制板已经集成了反向器，直接将以旧换新机连接到飞行控制器即可。
xbus：常规通信协议，支持18个通道，数据包较大，串口通信有两种模式，可以在遥控器的配置选项中配置。接收机无需做特殊配置
9、机甲：十字或×型
10、飞行模式：油门大小，里程模式，外部控制模式（树莓派等拓展）
11、
Storage：dataman-调用dm_write和dm_read函数来读写飞行任务，param-飞行参数，Message Bus-uORB（黑板，uORB可以由多个发送者发布，也可以被多个接收者接收。），
12、uORB：orb_publish()->orb_copy()与orb_check，
13、设备驱动节点->上层应用程序(文件)->register_driver()函数向操作系统注册一个设备节点,应用级驱动->api
14、进程：操作系统执行程序的基本单元，进程控制块PCB，  线程：进程本身也可以“并行”执行多个任务，引入一个进程中可以创建多个可并行的调度单元，即为线程
一个进程中的多个线程都会共享这个进程中的内存资源。  现场：上下文保存和现场恢复，
15、遥控器与SBUS：100K，8bit，再input_rc.msg消息发布到uORB总线上为其它进程所使用。
16、pwm,ppm:一路管脚传输多路pwm
17、
命令响应和状态切换：如commander takeoff： 起飞，   commnader：相关参数，变量，消息uOBR，线程->获取及处理
Arming state:init,standby,armed,standby_error,reboot,in_air_restore
18、外部通信总线Mavlink：数据在飞控和地面站交互数据（消息）
19、姿态控制：角速度测量-传感器及卡尔曼滤波得最优值 ，位置控制：速度测量-传感器及卡尔曼滤波   注：双闭环控制--系统对那个环敏感则置内环
20、混控：不同机型 ，加计：校准--零偏、标度因数 ，陀螺仪：零偏 ， 磁罗盘：GPS->差表->磁偏角 ，  水平仪：飞控和机身安装   yaw需要GPS或磁罗盘
21、kalaman：
MEMS传感器，会使姿态偏差大->系统的噪声误差满足高斯分布
k-1最优+测量值-融合->k预测值+测量值-融合->k最优
速度计->预测值->融合GPS->最优 （加速度计和陀螺仪，输入值  磁罗盘、GPS和气压计，测量值）
5公式
22、不同传感器的坐标系--->本地坐标系， 速度位置估计， 6个自由度，分别为在x、y、z这3个坐标轴上的平移和转动运动方式 ，
23、单片机->树莓派->单片机，即接口API，路径规划：树莓派

三、路径规划
代价，花销
1、A*算法
-->把启发式方法（heuristic approaches）如BFS，和常规方法如Dijsktra算法结合在一起的算法。
-->把Dijkstra算法（靠近初始点的结点）和BFS算法（靠近目标点的结点）的信息块结合起来。
-->f(n) = g(n) + h(n),  g(n)表示从初始结点到任意结点n的代价，h(n)表示从结点n到目标点的启发式评估代价


四、Remote相关
sbus:
一帧：start-byte 22data_byte~16chanel(11bit*16=8*22) flag_byte end_byte
IDLE+DMA:  100000k,9bit,1bit  
IDLE+RXNE: 100000k,8bit,2bit 
0~2047   ch1=11bit=8bit+3bit 
14ms---25byte   4ms间隔一帧 
//RXNE+TIM（IDLE）：
/*
 * 串口3,printf 发函数
 * 确保一次发送数据不超过USART3_MAX_SEND_LEN字节
 */
void u3_printf( char* fmt, ... )
{
	va_list ap;
	va_start( ap, fmt );
	vsprintf( (char *) USART3_TX_BUF, fmt, ap );
	va_end( ap );
	while ( DMA_GetCurrDataCounter( DMA1_Channel2 ) != 0 )
		;                                                                       /* 等待通道2传输完成 */
	UART_DMA_Enable( DMA1_Channel2, strlen( (const char *) USART3_TX_BUF ) );       /* 通过dma发送出去 */
}
/*串口收：详见DIY:esp+oled
通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到任何数据,则表示此次接收完毕.
[15]:0,没有接收到数据;1,接收到了一批数据.
[14:0]:接收到的数据长度
*/
vu16 USART3_RX_STA=0;//!!
USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	//开启中断 
TIM7_Int_Init(1000-1,7200-1);					//10ms中断,先关，再在usart接收中断开启
void USART3_IRQHandler(void)
{
	u8 res;	      
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{	 
		res =USART_ReceiveData(USART3);
		//接收没完成
		if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(USART3_RX_STA<USART3_MAX_RECV_LEN)	//还可以接收数据
			{
				TIM_SetCounter(TIM7,0);				         	
				if(USART3_RX_STA==0) 				
				{
					TIM_Cmd(TIM7,ENABLE);
				}
				USART3_RX_BUF[USART3_RX_STA++]=res;		
			}
			//接收完成
			else 
			{
				USART3_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
	}  				 											 
}
//IDLE+RXNE:
void Sbus_Data_Count(uint8_t *buf)
{
	CH[ 0] = ((int16_t)buf[ 2] >> 0 | ((int16_t)buf[ 3] << 8 )) & 0x07FF;
	CH[ 1] = ((int16_t)buf[ 3] >> 3 | ((int16_t)buf[ 4] << 5 )) & 0x07FF;
	CH[ 2] = ((int16_t)buf[ 4] >> 6 | ((int16_t)buf[ 5] << 2 )  | (int16_t)buf[ 6] << 10 ) & 0x07FF;
	CH[ 3] = ((int16_t)buf[ 6] >> 1 | ((int16_t)buf[ 7] << 7 )) & 0x07FF;
	CH[ 4] = ((int16_t)buf[ 7] >> 4 | ((int16_t)buf[ 8] << 4 )) & 0x07FF;
	CH[ 5] = ((int16_t)buf[ 8] >> 7 | ((int16_t)buf[ 9] << 1 )  | (int16_t)buf[10] <<  9 ) & 0x07FF;
	CH[ 6] = ((int16_t)buf[10] >> 2 | ((int16_t)buf[11] << 6 )) & 0x07FF;
	CH[ 7] = ((int16_t)buf[11] >> 5 | ((int16_t)buf[12] << 3 )) & 0x07FF;
	
	CH[ 8] = ((int16_t)buf[13] << 0 | ((int16_t)buf[14] << 8 )) & 0x07FF;
	CH[ 9] = ((int16_t)buf[14] >> 3 | ((int16_t)buf[15] << 5 )) & 0x07FF;
	CH[10] = ((int16_t)buf[15] >> 6 | ((int16_t)buf[16] << 2 )  | (int16_t)buf[17] << 10 ) & 0x07FF;
	CH[11] = ((int16_t)buf[17] >> 1 | ((int16_t)buf[18] << 7 )) & 0x07FF;
	CH[12] = ((int16_t)buf[18] >> 4 | ((int16_t)buf[19] << 4 )) & 0x07FF;
	CH[13] = ((int16_t)buf[19] >> 7 | ((int16_t)buf[20] << 1 )  | (int16_t)buf[21] <<  9 ) & 0x07FF;
	CH[14] = ((int16_t)buf[21] >> 2 | ((int16_t)buf[22] << 6 )) & 0x07FF;
	CH[15] = ((int16_t)buf[22] >> 5 | ((int16_t)buf[23] << 3 )) & 0x07FF;
}

//IDLE+DMA:
void Sbus_Data_Count(uint8_t *buf)
{
	CH[ 0] = ((int16_t)buf[ 1] >> 0 | ((int16_t)buf[ 2] << 8 )) & 0x07FF;
	CH[ 1] = ((int16_t)buf[ 2] >> 3 | ((int16_t)buf[ 3] << 5 )) & 0x07FF;
	CH[ 2] = ((int16_t)buf[ 3] >> 6 | ((int16_t)buf[ 4] << 2 )  | (int16_t)buf[ 5] << 10 ) & 0x07FF;
	CH[ 3] = ((int16_t)buf[ 5] >> 1 | ((int16_t)buf[ 6] << 7 )) & 0x07FF;
	CH[ 4] = ((int16_t)buf[ 6] >> 4 | ((int16_t)buf[ 7] << 4 )) & 0x07FF;
	CH[ 5] = ((int16_t)buf[ 7] >> 7 | ((int16_t)buf[ 8] << 1 )  | (int16_t)buf[9] <<  9 ) & 0x07FF;
	CH[ 6] = ((int16_t)buf[ 9] >> 2 | ((int16_t)buf[10] << 6 )) & 0x07FF;
	CH[ 7] = ((int16_t)buf[10] >> 5 | ((int16_t)buf[11] << 3 )) & 0x07FF;
	
	CH[ 8] = ((int16_t)buf[12] << 0 | ((int16_t)buf[13] << 8 )) & 0x07FF;
	CH[ 9] = ((int16_t)buf[13] >> 3 | ((int16_t)buf[14] << 5 )) & 0x07FF;
	CH[10] = ((int16_t)buf[14] >> 6 | ((int16_t)buf[15] << 2 )  | (int16_t)buf[17] << 10 ) & 0x07FF;
	CH[11] = ((int16_t)buf[16] >> 1 | ((int16_t)buf[17] << 7 )) & 0x07FF;
	CH[12] = ((int16_t)buf[17] >> 4 | ((int16_t)buf[18] << 4 )) & 0x07FF;
	CH[13] = ((int16_t)buf[18] >> 7 | ((int16_t)buf[19] << 1 )  | (int16_t)buf[21] <<  9 ) & 0x07FF;
	CH[14] = ((int16_t)buf[20] >> 2 | ((int16_t)buf[21] << 6 )) & 0x07FF;
	CH[15] = ((int16_t)buf[21] >> 5 | ((int16_t)buf[22] << 3 )) & 0x07FF;
}

void USART2_Handler(void)
{
	uint8_t data_len; 
	if(USART_GetITStatus(USART2,USART_IT_IDLE))
	{
         USART2->SR;//	USART_ClearFlag(USART3,USART_IT_IDLE);
         USART2->DR;
		 DMA_Cmd(DMA1_Channel6, DISABLE);
         Sbus_Data_Count(uint8_t *buf);
         DMA_SetCurrDataCounter(DMA1_Channel6,DMA_UART2_BUF_SIZE);
		 DMA_Cmd(DMA1_Channel6, ENABLE);
	}		

}

//使用
u16 sbus_to_pwm(u16 sbus_value)
{
    float pwm;
    pwm = (float)SBUS_TARGET_MIN + (float)(sbus_value - SBUS_RANGE_MIN) * SBUS_SCALE_FACTOR;
    //                1000                                   300              1000/1400
    if (pwm > 2000) pwm = 2000;
    if (pwm < 1000) pwm = 1000;
    return (u16)pwm;
}



