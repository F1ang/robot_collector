/*一、位带操作*/
//地址，地址指针，膨胀(位带区，位带别名区)--三要素   寄存器基址->别名区址->指针封装->操作指针--四步走
//sys.c  sys.h
// 4字节访问最高效
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

/*二、触摸屏*/
编程关键：键值及键值触发，状态机
1、电阻式：压力->x,y的ITO接触->分x,y两电阻->比例得x,y坐标 ，原理：x加电场，y测触点电压，利用电阻比例得x坐标。   
电容式：电容效应（非绝缘体)->x行发，y列收（电容量）->电容单元对应x,y(矩阵扫描)
2、xpt2046:  x-,x+,y-,y+信号->xpt2046处理->软件spi->interrupt_L/轮询-stm32->AD值->X,Y值（即物理转逻辑坐标）
时序spi：24 ,8周期DIN输入控制字，12/15/16周期转换，（手册）

校正屏幕：
已知坐标->触摸已知坐标->读取返回的坐标->计算转换系数->存FLASH->取FLASH->
if ( XPT2046_ReadAdc_Smooth_XY ( & strScreenCoordinate ) )//读取滤波后AD值
{    //在转换系数下进行变换（逻辑坐标）
  pDisplayCoordinate ->x = ( ( pTouchPara[LCD_SCAN_MODE].dX_X * strScreenCoordinate.x ) + ( pTouchPara[LCD_SCAN_MODE].dX_Y * strScreenCoordinate.y ) + pTouchPara[LCD_SCAN_MODE].dX );        
  pDisplayCoordinate ->y = ( ( pTouchPara[LCD_SCAN_MODE].dY_X * strScreenCoordinate.x ) + ( pTouchPara[LCD_SCAN_MODE].dY_Y * strScreenCoordinate.y ) + pTouchPara[LCD_SCAN_MODE].dY );
}
键与值：初始化按键，绑定键值（功能）---结构体函数，结构体数组实现

1)、绑定键值  2)、显示指定键值   3)、按下处理：按下的x,y->扫描键->标志     4)、松开处理
->在键区->执行键值
->在画框区->相应处理  

3、电容式TFTLCD
ott2001：I2C Salve address 0x59，时序及寄存器(手册)，   手势ID寄存器 ，坐标数据寄存器，
gt9147：I2C Salve address 0x5d/0x14（INT选择），0x814E-状态寄存器->坐标数据寄存器-0x8150~ , 0x8040-控制命令寄存器，配置寄存器，
ott2001初始化：复位->100 ms->释放复位->传感器控制寄存器最高位_1->使用
gt9147初始化：结束硬复位->i2c地址->100 ms ->软复位->更新配置->结束软复位->使用

4、电阻式tp
5、api:IC读取物理坐标->测试得校准参数（公式）->扫描触摸屏(状态,坐标寄存器)

6、流程
(1)选触摸屏->
->电阻->校准
->电容->id->ott2001/gt9147初始化，及Scan键值对应
(2)扫描坐标-> （.scan() ->  执行各自驱动IC,得触摸点物理坐标   注：电容则直接是逻辑坐标）
->电阻->校准系数系数->得逻辑坐标
->电容->校准系数为0->得逻辑坐标
(3)电阻->坐标区操作  电容->坐标区操作

7、注：地址要换算成读/写地址。  init 和scan公用（结构体函数，且初始化使键值对应）
关键：系数*AD值=逻辑x坐标，写命令与读数据，扫描-确认按下及获取x,y坐标（电阻屏需要校准系数，电容屏直接有比例因子得逻辑坐标）,状态机编程。
注：stm32只支持边沿触发，不支持电平触发。  spi_flash,spi与i2c可以硬件/软件，usart一般硬件。
8、ST7789:

9、显示屏：F4-8080/SPI等接口-ILI9341等显存(GRAM等)控制芯片-RGB接口(FPC、CLK+HSYNC+VSYNC+DE)-液晶面板
8080接口:RGB565=数据信号15pin,读信号,data/cmd信号,reset信号,写信号,cs信号,触摸屏控制信号(x-,x+,y-,y+)
FSMC:管理拓展的存储器,模拟8080时序

/*三、SD*/
1、command_control(48 bit)，SDIO：CLK上升沿有效，D/C，CRC，D0_L-busy，card_reg/data_tran，
->SDIO:SDIO适配器+AHB，clk cmd d/interrupt and dma,
->SDIO_CMD,SDIO_ARG命令索引和命令参数，

2、命令路径状态机/数据路径状态机：CPSM/DPSM，禁止则会回到IDLE； 命令发送并接收卡的响应/与SD数据交互
->命令初始化结构体：命令参数，命令号，响应类型，等待使能，CPSM(able or disable），只需设置命令索引和命令参数
->数据初始化结构体：数据传输超时，长度，块大小，传输方向，模式，DPSM
2022 .3 .23
3、cmd->sd w/r ，时序CLK(FOD-卡识别,FDD-数据传输),CMD,D0~D3。
SD命令格式（48 Bit）,命令类型（bc,bcr,ac,adtc）,响应及类型（长/短响应）.

卡识别模式->空闲，无效，准备，识别，待机
数据传输模式->待机，传输，发送，接收，编程，断开
响应类型->R1 R2 R3 R6 R7及帧格式
                 
（1）SD初始化：上电->CMD0软复位->CMD8区分SD卡及供电情况->ACMD55->ACMD41:再确认电压范围和是否属高容量卡->CMD2/CMD3获CID/RCA
->CMD9获CSD->CMD7获要操作SD卡。
（2）SD卡数据擦除，读取，写入操作
（3）SDIO与文件系统接口函数SD_ReadDisk(),SD_WriteDisk(),注：字节对齐实现

/*四、FATS*/
1、磁盘驱动器：6接口api，FATFS底层(diskio) 驱动代码，
2、SD卡加载文件系统管理文件， 关键点：SD读写，6接口api

2022. 3 .24
3、FATFS移植：
应用程序
（1）integer.h定义数据类型
（2)ffconf.h配置FATS
（3）diskio.c底层驱动编写，一般6接口api
初始化磁盘:disk_initialize()
获得磁盘状态:disk_status()
读扇区：disk_read()
写扇区：disk_write()
其他功能：disk_ioctl()
获得时间: get_fattime (void)
注、f_mount()注册工作区，才能使用后续API（即挂载SD卡）

fattester.c拓展成USMART接口，usmart_config.c添函数名及其查找串

/*五、USB*/
usb模块识别（刷脸机，桥梁）
1、PC<->MCU:共享数据缓冲区（可被USB访问）， 数据缓冲区大小-端点数和每个端点最大对的数据分组大小 。
2、移植->senddata_api:数据拷贝到端口1发送缓冲区，usb_api发送；receive_data_api，数据拷贝到端口3，usb_api实现，两者均使用回调调用usb_api。
->hw_config->usb虚拟usart

2022 .3 .23
3、端点：端点描述块 ，  usb模块识别有效端点，实现端点和数据缓冲区交互数据（得房卡/给脸部数据）   （刷脸酒店）
4、令牌分组，收发，握手分组
5、端点初始化
->IN令牌分组(发)：端点地址和收到的地址对应（刷脸匹配）->发车卡（指定他去客户出发区）,否则握手
->OUT令牌分组（收）：匹配（刷脸匹配）->发房卡（收到客户信息区），否则握手不处理->ack握手分组
SDIO,USB
6、usb读卡器：mass_mal->操作磁盘对象->memory读写实现->vu8 USB_STATUS_REG的使用
//bit0:表示电脑正在向SD卡写入数据
//bit1:表示电脑正从SD卡读出数据
//bit2:SD卡写数据错误标志位
//bit3:SD卡读数据错误标志位
//bit4:1,表示电脑有轮询操作(表明连接还保持着)

-->Usb Mass_Storag支持BOT传输相关，usb_bot.c，支持BOT的设备也支持endpoint，usb_endp.c，
-->Usb Mass_Storag可支持BOT,CBI传输协议
-->BOT又endpoint：SDIO/SPI等传输协议

/*六、GPIO复用与AFIO*/
1、GPIO映射
-->PA9默认GPIO，复用为串口TX，手册配置：
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

-->串口1的TX，RX重新映射 PB6，PB7：
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);//Remap全重映射

-->串口3的部分重映射：
GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);//PartialRemap部分
ps:区分端口复用和端口重映射

2、AFIO
-->AFRL（32 bit）,AFRH（32 bit）,  AFRL(AF0~15)：（pin）0~7，AFRH：8~15

-->pinx,由AFRL/ARRH选AFx(0~15)，datasheet-映射表

-->PA9由AFRH选AF7，映射为USART1_TX
/*PA9 指定为AF7，复用为USART1_TX */
GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
/* PA10 指定为AF7,复用为USART1_RX*/
GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 

注:外设复用于引脚
端口重映射->外设对应不同引脚   
端口复用->同一引脚对应不同外设

3、MODE  
output->推挽(Push-Pull,H/L,开漏(Open-Drain,H/高阻态,上拉电阻提供H,吸流强),复用(片上外设)推挽和开漏.
input->模拟,浮空(管脚悬空->状态不确定->由外部确认),上拉(灌电流),下拉(原L,动作则H,拉电流=输出电流=抗干扰)
GPIO复用AFIO寄存器,外设GPIO配置
高阻态:输出开路,状态->输入外部电路,总线占用,三态门   输入电平的不确定性  大电阻->极限就是悬空  状态的不确定性
OD:I/O端口的电平不一定是输出的电平.
PP:I/O端口的电平一定是输出的电平.
拉/灌电流--输出端  吸--输入端

4、EXTI
外部中断I/O映像中断线->NVIC中断矢量表

5、DMA
DMA请求,DMA请求通道映像(BUFF)->仲裁器,数据方向及指针变化,
BUFF=DMA通道x传输数量寄存器(DMA_CNDTRx)  DMA通道映射表,中断=传输过半/传输完成/传输错误  DMA与串口IDLE,RXNE与IDLE中断
DMA+IDLE/RXNE=>遍历判断    索引找确认帧头+帧尾及校验正确=>组成索引表

无DMA:内核通过DCode经过总线矩阵协调,从获取AHB存储的外设ADC采集的数据,
然后内核再通过DCode经过总线矩阵协调把数据存放到内存SRAM中.
双BUFF:获取现在正在访问的序号DMA_GetCurrentMemoryTarget().
DMA_DoubleBufferModeConfig(DMA1_Stream4, (uint32_t)Buffer1, DMA_Memory_0).

6、USART
发送器控制寄存器->发送数据寄存器(TRD)->发送移位寄存器  RXNE-RDR非空(可收),TXE-TRT为空(可发)

7、I2C
主机读/写从机地址(设备地址7bit),结构体初始化,TXE/RXNE可配置DMA请求和中断请求.
主机释放SDA总线,从机控制SDA总线,时序SCL/SDA.
读写API接口与I2C通信协议

8、SPI
时钟极性CPOL:SCK空闲电平(HL),时钟相位CPHA:采样边沿(SCK奇偶边沿)->MODE    时钟极性->时钟相位(奇偶边沿采样)
TXE=1发送缓冲区为空(可发),RXNE=1接收缓冲区非空(可收),配合移位寄存器+数据寄存器DR
注:发送/接收缓冲区<->数据移位寄存器,也有DMA,中断机制.
主机和从机-->SCK

9、TIM
内部时钟CK_INT->分频PSC->定时器时钟CK_CNT->计数CNT->重装ARR
输入捕获,输出比较,PWM,Encoder,Hall.
时钟源:CK_INT,外部时钟1 TIx->TIxFPx->TRGI,外部时钟2 ETR->ETRF,内部触发输入(ITRx).  控制器:编码器接口(2路),从模式(复位,计数),触发控制器(触发片内外设)
输入通道TIx->TIxFPx->捕获通道ICx->捕获寄存器  |  比较寄存器->OCxREF->输出通道OCx.
应用:1)PWM输入捕获(占2路,TI1FP1和TI2FP2->选触发TI1FP1及为H->TI1FP1周期,TI2FP2占空比),pwn捕获中断主要针对主捕获通道.
     2)PWM输出比较(cnt arr,中心对齐FOC-递增/递减计数)
     3)互补输出,死区插入,刹车配置,产生6步PWM(TIMx_ERG的COM事件,使互补输出达到6步pwm,可产生中断/DMA请求)
     4)encoder:TI1/TI2边沿计数->映射到TI1FP1/TI2FP2->向上/向下计数->计数脉冲
     5)hall:3相,接口定时器(捕获hall信号->产生时基),有刷电机
     接口定时器--捕获模式:TI1->TI1F_ED->捕获TRC->外部时钟1 TRGI  输出模式(pwm2):脉冲->TRGO->触发TIM1 COM事件->改变pwm属性
     HALL(A,B,C)XOR->接口定时器捕获模式->TI1.      TIM1:TRGI触发->COM事件->改变pwm属性
     6)DMA与中断机制,COM事件:软件TIMx_ERG/TRGI上升沿由硬件.
注:ARR->CNT->CCR,OCxREF->TGRO与内部触发ITRx(主从模式)->TRGI->COM事件


/*七、USMART调试组件*/
部门：外交usmat.c，处理usmat_str.c，监听usmart_config.c

usmart_config.c添函数名及其查找串

/*八、EXTIv and ADC*/
1、外部中断线line0对应PX0外部pin(line 0~15---pin 0~15).
2、边沿触发.
3、外部中断源EXTI15_10_IRQn.--void EXTI15_10_IRQHandler(void).

4、F4-pin,in/out,pp/od,up/down/no,speed.--DMA的数据流
5、ADC采样可被外部时钟调频率采样，ADC1_IN15(最多16PIN外部)->共0~19规则/插入转换通道和数目，DMA传输.

6、F4:启动文件-->中断矢量表(即中断入口函数)，stm32xxx.h-->中断矢量号(中断源)，系统文件-->时钟配置
startup_stm32fxx_xx.s->SystemInit->SetSysClock()->宏SYSCLK_FREQ_72MHz等RCC配置因子->SetSysClockToxx
注：
F4-->SYSTICK=168MHZ   AHB=168M   APB1=42M  APB2=84M=HCLK2
F1-->SYSTICK=72MHZ    AHB=72M=PLLCLK=HCLK=APB2=PCLK2   APB1=PCLK1=36M                     

7、Configures the ADC clock (ADCCLK).---F1 MAX=14 MHZ---ADCCLK=PCLK2/(分频因子2 4 6 8).    1/ADCCLK=最小采样周期(最快采样). 
转换时间=采样时间+12.5个周期.
F1:ADCCLK一般最高只能达12M, F4:ADCCLK=21M---->ADCCLK太高可能会出现BUG.
ADC_SampleTime_55Cycles5==采样时间为55.5个周期，最高为1.5个周期,周期=1/ADCCLK.
采样时间=5/21M=0.2381us=ADC_TwoSamplingDelay_5Cycles/ADCCLK=采样周期/ADCCLK.
独立ADC/双ADC(主从)->单通道或扫描多通道,规则/注入转换顺序,双ADC触发1外部1软件.

DAC:触发选择并开启->数据(DHRX)->生成波形.  DHRx->(DMA请求)->DORx,  
触发选择(即开关)-决定波形周期,DAC控制寄存器-控制波形数据集,数据保持寄存器-存数据集,数据输出寄存器-输出DA转换



/*九、NRF2401、ESP8266、ESP32、HC06、ILI9341(显示和触摸)、TFT、OV7725、MPU6050*/
//状态机编程
1、ESP:请求和响应，客户端和服务器端，IP和端口，服务器端监听端口，MQTT的发布和订阅，AT.
2、NRF:SPI
1)发送器-发送目标址和待发数据缓存址. 接收器-接收址配对发送器和发送目标址. 
2)发送和接收时序，发送端频率通道(0~127)，接收端和发送端数据通道(6).(接收模式,可接收6路不同通道的数据;6不同的地址,相同的频道)
3)发送端可共用相同的频道(频率通道128);RX_ADDR_PX数据通道的地址(发送端的RX_ADDR_P0有6个地址，和接收端的RX_ADDR_PX对应).
4)发送端和接收端--数据通道RX_ADDR_Px配置，发送端指定数据通道，接收端分配该数据通道的ID(0~5).
5)TX接收应答信号.   工作频率通道(125)->数据通道(6) 即RX_ADDR_PX(6)->TX 和 RX配置(通道X的发送和接收地址).
如:数据通道5 的发送端(TX5)及接收端(RX)地址.
TX5：TX_ADDR=0xB3B4B5B605----TX5：RX_ADDR_P0=0xB3B4B5B605----RX：RX_ADDR_P5=0xB3B4B5B605(TX5：RX_ADDR_P0== RX：RX_ADDR_P5)
即TX---RX可分别配置各自接收数据通道通信!!!.
3、HC06:AT/USART
//配置口和功能口，控制信号线
4、ILI9341:读写时序，显存(GRA)数据传输时序，即控制和传输时序。  读写寄存器实现功能。
5、OV7725:读写时序(I2C),AL422B(FIFO)像素输出VGA时序,即控制和传输时序。 读写寄存器实现功能。
6、MPU:读写时序(I2C)，原DATA->DMP运动处理器->MPL姿态融合、滤波->ROLL,YAW,PITCH.


/*十、RMUT*/
1、调试10min.
2、至少成功兑换2个矿石.
3、ready:1 min，start:3 min.
4、启动,资源岛,兑换站.
5、U-2/4,凹槽-1/5.
6、U放矿石3HZ白灯闪，兑换站识别红/蓝闪.
7、矿石内置场地交互模块卡。所有矿石的条形码图案相同，矿石内置的场地交互模块卡与条形码印刷平面平行距离为 5cm
8、200*200*200 mm.
9、条形码朝下，兑换站识别，3 s推入.
10、携带：工程机器人与矿石发生持续直接或间接接触，导致矿石产生明显位移.
注:工程机器人至少成功兑换 2 个矿石。
 

%s字符串,%d整型,%c字符 \r\n==0x0d0x0a=回车符+换行符
servo:50hz=20ms H占0.5-2.5ms (0.5=0,+0.5=+45°)


/*十一、选项字节*/
1、16字节=8*2反码，功能配置(选项)，读保护、看门狗、停机、待机、数据、写保护
2、16进制=4bit  OBL读取信息块->映射reg:FLASH_OBR、FLASH_WRPR  内存映射选项字节编程 


/* 十二、IAP+OTA */
1、w25q128
  65536页  1FLASH=256块,1块=16扇区,1扇区=4k=16页,1页=256byte  写1页,擦16页  (256 16 16 256)
2、bootloader
  (1)IAP控制表:控制表文件头、固件更新源、外部FLASH临时程序去读取地址(物理地址)及总大小(bytes)、内部FLASH正式程序区写入地址(物理地址)及总大小(bytes)、
  4字节边界、4字节对齐、内部FLASH单个扇区大小(bytes)、bin文件IAP_WR组包的答案小、128字节对齐、固件加/解密密钥、固件明文的CRC32、
  bin文件密文的校验和、初值0/重启完成后验证CRC不正确->重启再试的次数、IAP结构自身的CRC32   (加载IAP控制表-写入临时FLASH-数据CRC-MSP指针跳APP)

  (2)bin校验:1,大小  2,spi_en  3,16bytes密钥(FLASH_ID和CPU_ID异或)->bin前8byte解密(加密报文+密钥)  
  4,8byte铭文CRC32,往后数据按8byte进行解密 5,镜像搬到正式区(擦-解密报文(原文)-写)
  注:数据解密 decrypt(uint8_t *inout, uint8_t *key):in/out(加密/解密)  key(密钥) 密文/明文

  (3)reboot verify image:重启后验证内部FLASH CRC32,error需要反初始化IAP控制表

  (4)boot:reboot/erase_iapctrl->判断固件是否存在->获取实际app main()的地址与app地址(MSP指针)->对app main()的存储地址进行指针运算即可跳转main()
#define HW_REG(x) (*(volatile uint32_t*)(x))    
读出报文->密文校验和->前一包密文保存(为了后续包解密 ^ 前一包密文 得到原文)->解密文(原密文+固定指定密钥)->明文(原文)CRC32
->CRC32->BIN文件头、密文校验和、明文CRC32(4字节对齐)     
(综合:检查外部FLASH BIN文件格式===检查范围:BIN文件头,密文校验和,明文CRC32)

3、app
  n帧IAP数据
  (1)bin文件大小->IAP控制表初始化->硬件CRC->bin换算扇区大小->erase外部备分区   begin

  (2)固件长度、写入地址、数据长度及准确性判断->写入外部备份FLASH:CRC32校验和(4byte对齐)、密文校验(同上,8byte)->BIN文件头判断、防呆判断   write

  (3)最后1帧数据CRC32校验(FLASH数据 与 接收的数据)  format和crc

  (4)复位系统->boot->控制表通用

4、ota
  (1)can:1,首包(bin文件大小,组包的一帧大小)  n*128bytes(分16次),128byte=16次*8byte  以16次(128byte)作为一次心跳(进度条) 
  2,n = s_ack_index(进度条) 注:可存在不满16小包的n  主从机握手实现  usart_buf->can_buf

  (2)数据栈实现   发送队列帧--入:帧头++ 发:帧尾--   接收BUFF  


/* 十三、IMU */


af:tim3 
APB1-90M  APB2-180M
encoder:tim2-APB1 tim4-APB1 tim5-APB1 tim8-APB2
pwm:tim3-APB1 tim9-APB2
updata:tim7-APB1
dbus:usart1
debug:usart3


/* 单片机知识积累 */
1、堆heap:内存可以在堆中以不连续的块的形式分配(意味着碎片化，可能会降低运行效率),即指针可以随时访问改地址。
比如局部变量stack，指针访问(作用域为局部)；局部申请heap，未被释放，指针访问地址仍可作用(作用域全局)
stm32启动文件申请了512byte的heap
栈:现场保存(中断/函数回调)、局部变量存储
2、独立看门狗(预装载值,等效定时器,看门狗时钟-不太准)  窗口看门狗(预装载值,非窗口范围内喂狗则会复位,系统时钟-更准)

3、BOOT2APP：APP和BOOT在keil界面划分好IROM区  存储地址与运行地址
void JumpToApp( void )
{
	uint32_t  App_Start_Address = 0;
	void (*pFun)(void);       //定义一个函数指针.用于指向APP程序入口  
	
	App_Start_Address=*(__IO uint32_t*) ( 0x08004000+4 );	//获取复位中断向量地址
	__set_MSP(*(__IO uint32_t*) 0x08004000);	//设置SP.，堆栈栈顶地址复位
	pFun = (void (*)(void))App_Start_Address; //生成跳转函数.将复位中断向量地址做为函数指针
	(*pFun)();   //执行函数，实现跳转.不再返回.
}

4、启动方式:启动地址与中断向量表地址关联
(1)BOOTX引脚->主闪存(内部FLASH)   APP存储地址与启动地址:0x08000000
(2)系统存储器启动   启动地址:0x1FFF0000,存储ST的出厂bootloader(即ISP),ISP提供的串口下载的方式->程序下载完毕->恢复启动方式(1)
(3)SRAM启动  启动地址:0x20000000,中断向量表重定位到sram->启动区选sram,运行速度快
注：上电后CPU是从0地址开始执行->启动地址映射->0x08000000

5、flash死锁解决:修改BOOT模式重新刷写代码->BOOT0=1，BOOT1=0即可从系统存储器启动(ISP)->恢复
6、Jlink-Commander使用

7、对闪存FLASH的擦除与写入-API,bootloader->设置中断向量表偏移API=NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000)=0x08008000

8、中断向量表:中断入口地址,可重定向到ram区

















