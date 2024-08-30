/**
  ******************************************************************************
  * 文件名程: bsp_lcd.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.0
  * 编写日期: 2015-10-04
  * 功    能: 液晶底层驱动函数实现
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "bsp_lcd.h"
#include "ascii.h"	
#include "bsp_touch.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
// 保存当前检查到的液晶模块ID
volatile u32 lcd_id=0;
u16 TextColor=BLACK,BackColor=BACKGROUND;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 用于 LCD 简单延时函数
  * 输入参数: nCount ：延时计数值
  * 返 回 值: 无
  * 说    明：无
  */	
static void LCD_DELAY( volatile u32 nCount )
{
  for ( ; nCount != 0; nCount -- );
}

/**
  * 函数功能: 初始化LCD的IO引脚
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */	
static void LCD_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
  /* 使能复用IO时钟：复用为fsmc功能 */
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO, ENABLE );	  
  
	/* 使能FSMC对应相应管脚时钟 */
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE );
  FSMC_LCD_CS_APBxClock_FUN ( FSMC_LCD_CS_CLK, ENABLE );
  FSMC_LCD_DC_APBxClock_FUN ( FSMC_LCD_DC_CLK, ENABLE );
  FSMC_LCD_BK_APBxClock_FUN ( FSMC_LCD_BK_CLK, ENABLE );
	
  /* 配置FSMC相对应的数据线,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10 */	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_8 | GPIO_Pin_9 | 
																GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init ( GPIOD, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
																GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
																GPIO_Pin_15;
	GPIO_Init ( GPIOE,  & GPIO_InitStructure ); 
	
	/* 配置FSMC相对应的控制线
	 * PD4-FSMC_NOE   :LCD-RD
	 * PD5-FSMC_NWE   :LCD-WR
	 * PG12-FSMC_NE4  :LCD-CS
	 * PF0-FSMC_A0    :LCD-DC
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
	GPIO_Init (GPIOD, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
	GPIO_Init (GPIOD, & GPIO_InitStructure );	
	  
	GPIO_InitStructure.GPIO_Pin = FSMC_LCD_CS_PIN; 
	GPIO_Init ( FSMC_LCD_CS_PORT, & GPIO_InitStructure );  
	
	GPIO_InitStructure.GPIO_Pin = FSMC_LCD_DC_PIN; 
	GPIO_Init ( FSMC_LCD_DC_PORT, & GPIO_InitStructure );	
	
  /* 配置LCD背光控制管脚BK*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 	
	GPIO_InitStructure.GPIO_Pin = FSMC_LCD_BK_PIN; 
	GPIO_Init ( FSMC_LCD_BK_PORT, & GPIO_InitStructure );
  /* 初始化时先不开背光 */
  GPIO_ResetBits(FSMC_LCD_BK_PORT,FSMC_LCD_BK_PIN);
}

/**
  * 函数功能: LCD  FSMC 模式配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：读写使用相同时间配置
  */
static void LCD_FSMC_Config ( void )
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  fsmc_lcd; 	
	
	/* 使能FSMC时钟*/
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );

	fsmc_lcd.FSMC_AddressSetupTime      = 0x02;	 //地址建立时间
	fsmc_lcd.FSMC_AddressHoldTime       = 0x00;	 //地址保持时间
	fsmc_lcd.FSMC_DataSetupTime         = 0x05;	 //数据建立时间
	fsmc_lcd.FSMC_BusTurnAroundDuration = 0x00;
	fsmc_lcd.FSMC_CLKDivision           = 0x00;
	fsmc_lcd.FSMC_DataLatency           = 0x00;
	fsmc_lcd.FSMC_AccessMode            = FSMC_AccessMode_B;	 //模式B比较适用于LCD
	
	FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_LCD_BACKx;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = & fsmc_lcd;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = & fsmc_lcd;  
	
	FSMC_NORSRAMInit ( & FSMC_NORSRAMInitStructure ); 
	
	/* 使能 FSMC_Bank1_NORSRAM4 */
	FSMC_NORSRAMCmd ( FSMC_LCD_BACKx, ENABLE );  
}

/**
  * 函数功能: 初始化LCD寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：需要配置哪些寄存器，需要设置什么值与液晶厂家生产环境密切相关，
  *           所以这些参数由厂家提供，不同厂家可能不同。也可以根据ILI9488芯片
  *           手册内容参考修改。
  */
static void ILI9488_REG_Config ( void )
{
  //************* Start Initial Sequence **********//
  /* PGAMCTRL (Positive Gamma Control) (E0h) */
  LCD_WRITE_CMD(0xE0);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x07);
  LCD_WRITE_DATA(0x10);
  LCD_WRITE_DATA(0x09);
  LCD_WRITE_DATA(0x17);
  LCD_WRITE_DATA(0x0B);
  LCD_WRITE_DATA(0x41);
  LCD_WRITE_DATA(0x89);
  LCD_WRITE_DATA(0x4B);
  LCD_WRITE_DATA(0x0A);
  LCD_WRITE_DATA(0x0C);
  LCD_WRITE_DATA(0x0E);
  LCD_WRITE_DATA(0x18);
  LCD_WRITE_DATA(0x1B);
  LCD_WRITE_DATA(0x0F);

  /* NGAMCTRL (Negative Gamma Control) (E1h)  */
  LCD_WRITE_CMD(0XE1);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x17);
  LCD_WRITE_DATA(0x1A);
  LCD_WRITE_DATA(0x04);
  LCD_WRITE_DATA(0x0E);
  LCD_WRITE_DATA(0x06);
  LCD_WRITE_DATA(0x2F);
  LCD_WRITE_DATA(0x45);
  LCD_WRITE_DATA(0x43);
  LCD_WRITE_DATA(0x02);
  LCD_WRITE_DATA(0x0A);
  LCD_WRITE_DATA(0x09);
  LCD_WRITE_DATA(0x32);
  LCD_WRITE_DATA(0x36);
  LCD_WRITE_DATA(0x0F);
  
  /* Adjust Control 3 (F7h)  */
  LCD_WRITE_CMD(0XF7);
  LCD_WRITE_DATA(0xA9);
  LCD_WRITE_DATA(0x51);
  LCD_WRITE_DATA(0x2C);
  LCD_WRITE_DATA(0x82);/* DSI write DCS command, use loose packet RGB 666 */

  /* Power Control 1 (C0h)  */
  LCD_WRITE_CMD(0xC0);
  LCD_WRITE_DATA(0x11);
  LCD_WRITE_DATA(0x09);

  /* Power Control 2 (C1h) */
  LCD_WRITE_CMD(0xC1);
  LCD_WRITE_DATA(0x41);

  /* VCOM Control (C5h)  */
  LCD_WRITE_CMD(0XC5);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x0A);
  LCD_WRITE_DATA(0x80);

  /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
  LCD_WRITE_CMD(0xB1);
  LCD_WRITE_DATA(0xB0);
  LCD_WRITE_DATA(0x11);

  /* Display Inversion Control (B4h) */
  LCD_WRITE_CMD(0xB4);
  LCD_WRITE_DATA(0x02);

  /* Display Function Control (B6h)  */
  LCD_WRITE_CMD(0xB6);
  LCD_WRITE_DATA(0x02);
  LCD_WRITE_DATA(0x22);

  /* Entry Mode Set (B7h)  */
  LCD_WRITE_CMD(0xB7);
  LCD_WRITE_DATA(0xc6);

  /* HS Lanes Control (BEh) */
  LCD_WRITE_CMD(0xBE);
  LCD_WRITE_DATA(0x00);
  LCD_WRITE_DATA(0x04);

  /* Set Image Function (E9h)  */
  LCD_WRITE_CMD(0xE9);
  LCD_WRITE_DATA(0x00);
 
  /* 设置屏幕方向和尺寸 */
  LCD_SetDirection(LCD_DIRECTION);
  
  /* Interface Pixel Format (3Ah) */
  LCD_WRITE_CMD(0x3A);
  LCD_WRITE_DATA(0x55);/* 0x55 : 16 bits/pixel  */

  /* Sleep Out (11h) */
  LCD_WRITE_CMD(0x11);
  LCD_DELAY(120*2000);
  /* Display On */
  LCD_WRITE_CMD(0x29);
}

/**
  * 函数功能: 读取液晶模组ID
  * 输入参数: 无
  * 返 回 值: 液晶模组的ID
  * 说    明：这是通过读取04H寄存器获取得到液晶模组ID，该ID值有液晶厂家编程，不同液晶
  *           厂家的液晶模组得到的ID值可能不同。这也可以分辨不同型号的液晶模组。
  */
static u32 LCD_ReadID(void)
{
	u16 buf[4];

	LCD_WRITE_CMD(0x04);  
	buf[0] = LCD_READ_DATA();        // 第一个读取数据无效
	buf[1] = LCD_READ_DATA()&0x00ff; // 只有低8位数据有效
	buf[2] = LCD_READ_DATA()&0x00ff; // 只有低8位数据有效
	buf[3] = LCD_READ_DATA()&0x00ff; // 只有低8位数据有效
	return (buf[1] << 16) + (buf[2] << 8) + buf[3];  
}

/**
  * 函数功能: 画板初始化，用于取色用
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void Palette_Init(void)
{  
  /* 整屏清为白色 */
  LCD_Clear(  0,  0,480, 320,BACKGROUND);
  LCD_Clear(  0,280, 68, 40, LIGHTBLUE);  
  LCD_DispString_EN(16,285,"SEL",LIGHTBLUE,BLACK,FONT_24);
  LCD_Clear( 68,280, 68, 40, RED );
  LCD_DispString_EN(93,285,"UP",RED,BLACK,FONT_24);
  LCD_Clear(136,280, 68, 40, GREEN  );
  LCD_DispString_EN(145,285,"DOWN",GREEN,BLACK,FONT_24);
  LCD_Clear(204,280, 68, 40, ORANGE);
  LCD_DispString_EN(214,285,"LEFT",ORANGE,BLACK,FONT_24);
  LCD_Clear(272,280, 68, 40, CYAN);
  LCD_DispString_EN(275,285,"RIGHT",CYAN,BLACK,FONT_24);
  LCD_Clear(340,280, 68, 40, YELLOW);
  LCD_DispString_EN(356,285,"KEY1",YELLOW,BLACK,FONT_24);
  LCD_Clear(408,280, 72, 40, MAGENTA);
  LCD_DispString_EN(424,285,"KEY2",MAGENTA,BLACK,FONT_24);
}

/**
  * 函数功能: 液晶模组初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
u32 BSP_LCD_Init(void)
{
	LCD_GPIO_Config();
	LCD_FSMC_Config();
  
  lcd_id=LCD_ReadID();
  if(lcd_id==0x548066 || lcd_id==0x8066)
  {
    ILI9488_REG_Config();
  }    
  /* 触摸屏IO初始化 */
  Touch_Init_GPIO();

  Palette_Init();
  
  /* 开背光 */
  LCD_BK_ON();  
  return lcd_id;
}

/**
  * 函数功能: 设置LCD的GRAM的扫描方向 
  * 输入参数: ucOption ：选择GRAM的扫描方向 
  *           可选值：1 :原点在屏幕左上角 X*Y=320*480
  *                   2 :原点在屏幕右上角 X*Y=480*320
  *                   3 :原点在屏幕右下角 X*Y=320*480
  *                   4 :原点在屏幕左下角 X*Y=480*320
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_SetDirection( u8 ucOption )
{	
/**
  * Memory Access Control (36h)
  * This command defines read/write scanning direction of the frame memory.
  *
  * These 3 bits control the direction from the MPU to memory write/read.
  *
  * Bit  Symbol  Name  Description
  * D7   MY  Row Address Order     -- 以X轴镜像
  * D6   MX  Column Address Order  -- 以Y轴镜像
  * D5   MV  Row/Column Exchange   -- X轴与Y轴交换
  * D4   ML  Vertical Refresh Order  LCD vertical refresh direction control. 
  *
  * D3   BGR RGB-BGR Order   Color selector switch control
  *      (0 = RGB color filter panel, 1 = BGR color filter panel )
  * D2   MH  Horizontal Refresh ORDER  LCD horizontal refreshing direction control.
  * D1   X   Reserved  Reserved
  * D0   X   Reserved  Reserved
  */
	switch ( ucOption )
	{
		case 1:
//   左上角->右下角 
//	(0,0)	___ x(320)
//	     |  
//	     |
//       |	y(480) 
			LCD_WRITE_CMD(0x36); 
			LCD_WRITE_DATA(0x08); 
      
			LCD_WRITE_CMD(0x2A); 
			LCD_WRITE_DATA(0x00);	/* x start */	
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);  /* x end */	
			LCD_WRITE_DATA(0x3F);

			LCD_WRITE_CMD(0x2B); 
			LCD_WRITE_DATA(0x00);	/* y start */  
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);	/* y end */   
			LCD_WRITE_DATA(0xDF);					
		  break;
		
		case 2:
//		右上角-> 左下角
//		y(320)___ (0,0)            
//		         |
//		         |
//             |x(480)    
			LCD_WRITE_CMD(0x36); 
			LCD_WRITE_DATA(0x68);	
			LCD_WRITE_CMD(0x2A); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0xDF);	

			LCD_WRITE_CMD(0x2B); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0x3F);				
		  break;
		
		case 3:
//		右下角->左上角
//		          |y(480)
//		          |           
//		x(320) ___|(0,0)		
			LCD_WRITE_CMD(0x36); 
			LCD_WRITE_DATA(0xC8);	
			LCD_WRITE_CMD(0x2A); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0x3F);	

			LCD_WRITE_CMD(0x2B); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0x3F);			  
		  break;

		case 4:
//		左下角->右上角
//		|x(480)
//		|
//		|___ y(320)					  
			LCD_WRITE_CMD(0x36); 
			LCD_WRITE_DATA(0xA8);	
    
			LCD_WRITE_CMD(0x2A); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0xDF);	

			LCD_WRITE_CMD(0x2B); 
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x00);
			LCD_WRITE_DATA(0x01);
			LCD_WRITE_DATA(0x3F);				
	    break;		
	}	
	/* 开始向GRAM写入数据 */
	LCD_WRITE_CMD (0x2C);	
}

/**
  * 函数功能: 在LCD显示器上开辟一个窗口
  * 输入参数: usX ：在特定扫描方向下窗口的起点X坐标
  *           usY ：在特定扫描方向下窗口的起点Y坐标
  *           usWidth ：窗口的宽度
  *           usHeight ：窗口的高度
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_OpenWindow(u16 usX, u16 usY, u16 usWidth, u16 usHeight)
{	
	LCD_WRITE_CMD(0x2A ); 				       /* 设置X坐标 */
	LCD_WRITE_DATA(usX>>8);	             /* 设置起始点：先高8位 */
	LCD_WRITE_DATA(usX&0xff);	           /* 然后低8位 */
	LCD_WRITE_DATA((usX+usWidth-1)>>8);  /* 设置结束点：先高8位 */
	LCD_WRITE_DATA((usX+usWidth-1)&0xff);/* 然后低8位 */

	LCD_WRITE_CMD(0x2B); 			           /* 设置Y坐标*/
	LCD_WRITE_DATA(usY>>8);              /* 设置起始点：先高8位 */
	LCD_WRITE_DATA(usY&0xff);            /* 然后低8位 */
	LCD_WRITE_DATA((usY+usHeight-1)>>8); /* 设置结束点：先高8位 */
	LCD_WRITE_DATA((usY+usHeight-1)&0xff);/* 然后低8位 */
}

/**
  * 函数功能: 设定LCD的光标坐标
  * 输入参数: usX ：在特定扫描方向下窗口的起点X坐标
  *           usY ：在特定扫描方向下窗口的起点Y坐标
  * 返 回 值: 无
  * 说    明：无
  */
static void LCD_SetCursor ( u16 usX, u16 usY )	
{
	LCD_OpenWindow ( usX, usY, 1, 1 );
}

/**
  * 函数功能: 在LCD显示器上以某一颜色填充像素点
  * 输入参数: ulAmout_Point ：要填充颜色的像素点的总数目
  *           usColor ：颜色
  * 返 回 值: 无
  * 说    明：无
  */
#if defined ( __CC_ARM )  // 使用Keil编译环境
static __inline void LCD_FillColor ( u32 ulAmout_Point, u16 usColor )
{
	u32 i = 0;	
	
	/* 开始向GRAM写入数据 */
	LCD_WRITE_CMD ( 0x2C );	
	
	for ( i = 0; i < ulAmout_Point; i ++ )
		LCD_WRITE_DATA ( usColor );	
}
#elif defined ( __ICCARM__ ) // 使用IAR编译环境
#pragma inline
static void LCD_FillColor ( u32 ulAmout_Point, u16 usColor )
{
	u32 i = 0;	
	
	/* 开始向GRAM写入数据 */
	LCD_WRITE_CMD ( 0x2C );	
	
	for ( i = 0; i < ulAmout_Point; i ++ )
		LCD_WRITE_DATA ( usColor );	
}
#endif

/**
  * 函数功能: 对LCD显示器的某一窗口以某种颜色进行清屏
  * 输入参数: usX ：在特定扫描方向下窗口的起点X坐标
  *           usY ：在特定扫描方向下窗口的起点Y坐标
  *           usWidth ：窗口的宽度
  *           usHeight ：窗口的高度
  *           usColor ：颜色
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_Clear(u16 usX,u16 usY,u16 usWidth,u16 usHeight,u16 usColor)
{	 
#if 0   /* 优化代码执行速度 */
  u32 i;
	u32 n,m;
  /* 在LCD显示器上开辟一个窗口 */
  LCD_OpenWindow(usX,usY,usWidth,usHeight); 
  /* 开始向GRAM写入数据 */
  LCD_WRITE_CMD(0x2C);
  
  m=usWidth * usHeight;
  n=m/8;
  m=m-8*n;
	for(i=0;i<n;i++)
	{
		LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
    
    LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
    LCD_WRITE_DATA(usColor);	
	}
  for(i=0;i<m;i++)
	{
		LCD_WRITE_DATA(usColor);	
	}
#else
  /* 在LCD显示器上开辟一个窗口 */
  LCD_OpenWindow(usX,usY,usWidth,usHeight);
  /* 在LCD显示器上以某一颜色填充像素点 */
	LCD_FillColor(usWidth*usHeight,usColor);	
#endif	
}

/**
  * 函数功能: 对LCD显示器的某一点以某种颜色进行填充
  * 输入参数: usX ：在特定扫描方向下窗口的起点X坐标
  *           usY ：在特定扫描方向下窗口的起点Y坐标
  *           usColor ：颜色
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_SetPointPixel(u16 usX,u16 usY,u16 usColor)	
{	
	if((usX<LCD_DEFAULT_WIDTH)&&(usY<LCD_DEFAULT_HEIGTH))
  {
		LCD_OpenWindow(usX,usY,1,1);
		LCD_FillColor(1,usColor);
	}
}

/**
  * 函数功能: 对LCD显示器的某一点以某种颜色进行填充
  * 输入参数: 无
  * 返 回 值: u16:像素数据RGB565
  * 说    明：无
  */
static u16 LCD_Read_PixelData ( void )	
{	
	u16 usR=0, usG=0, usB=0 ;
	
	LCD_WRITE_CMD ( 0x2E );   /* 读数据 */
	usR = LCD_READ_DATA (); 	/*FIRST READ OUT DUMMY DATA*/
	
	usR = LCD_READ_DATA ();  	/*READ OUT RED DATA  */
	usB = LCD_READ_DATA ();  	/*READ OUT BLUE DATA*/
	usG = LCD_READ_DATA ();  	/*READ OUT GREEN DATA*/	
	
  return (((usR>>11)<<11) | ((usG>>10)<<5) | (usB>>11));
	
}

/**
  * 函数功能: 获取 LCD 显示器上某一个坐标点的像素数据
  * 输入参数: usX ：在特定扫描方向下窗口的起点X坐标
  *           usY ：在特定扫描方向下窗口的起点Y坐标
  * 返 回 值: u16:像素数据RGB565
  * 说    明：无
  */
u16 LCD_GetPointPixel ( u16 usX, u16 usY )
{ 
	u16 usPixelData;
	
	LCD_SetCursor ( usX, usY );
	
	usPixelData = LCD_Read_PixelData ();
	
	return usPixelData;
	
}

/**
  * 函数功能: 在 LCD 显示器上使用 Bresenham 算法画线段
  * 输入参数: usX1 ：在特定扫描方向下窗口的起点X坐标
  *           usY1 ：在特定扫描方向下窗口的起点Y坐标
  *           usX2 ：在特定扫描方向下线段的另一个端点X坐标
  *           usY2 ：在特定扫描方向下线段的另一个端点Y坐标
  *           usColor ：线段的颜色
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_DrawLine(u16 usX1,u16 usY1,u16 usX2,u16 usY2,u16 usColor)
{
	u16 us; 
	u16 usX_Current, usY_Current;
	s32 lError_X=0,lError_Y=0,lDelta_X,lDelta_Y,lDistance; 
	s32 lIncrease_X, lIncrease_Y;
	
	lDelta_X=usX2-usX1; //计算坐标增量 
	lDelta_Y=usY2-usY1; 
	usX_Current = usX1; 
	usY_Current = usY1; 
	
	if(lDelta_X>0)
  {
    lIncrease_X=1; //设置单步方向 
	}
  else if(lDelta_X==0)
  {
		lIncrease_X=0;//垂直线 
	}
	else 
  { 
    lIncrease_X=-1;
    lDelta_X=-lDelta_X;
  }
  
	if(lDelta_Y>0)
  {
		lIncrease_Y=1;
	}
	else if(lDelta_Y==0)
  {
		lIncrease_Y=0;//水平线 
	}
	else
  {
    lIncrease_Y=-1;
    lDelta_Y=-lDelta_Y;
  }
	
	if(lDelta_X>lDelta_Y)
  {
		lDistance=lDelta_X; //选取基本增量坐标轴 
	}
	else
  {
		lDistance=lDelta_Y; 
  }
	
	for(us=0;us<=lDistance+1;us++)//画线输出 
	{
		LCD_SetPointPixel(usX_Current,usY_Current,usColor);//画点 
		lError_X+=lDelta_X; 
		lError_Y+=lDelta_Y;
		if(lError_X>lDistance)
		{ 
			lError_X-=lDistance; 
			usX_Current+=lIncrease_X; 
		}
		if(lError_Y>lDistance) 
		{ 
			lError_Y-=lDistance; 
			usY_Current+=lIncrease_Y; 
		}		
	}
}

/**
  * 函数功能: 在LCD显示器上画一个矩形
  * 输入参数: usX_Start ：在特定扫描方向下窗口的起点X坐标
  *           usY_Start ：在特定扫描方向下窗口的起点Y坐标
  *           usWidth：矩形的宽度（单位：像素）
  *           usHeight：矩形的高度（单位：像素）
  *           usColor ：矩形的颜色
  *           ucFilled ：选择是否填充该矩形
  *             可选值：0：空心矩形
  *                     1：实心矩形
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_DrawRectangle ( u16 usX_Start, u16 usY_Start, u16 usWidth, u16 usHeight, u16 usColor, u8 ucFilled )
{
	if(ucFilled)
  {
	  LCD_Clear ( usX_Start, usY_Start, usWidth, usHeight, usColor);
  }
	else
	{
		LCD_DrawLine ( usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start, usColor );
		LCD_DrawLine ( usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1, usColor );
		LCD_DrawLine ( usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1, usColor );
		LCD_DrawLine ( usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1, usColor );		
	}
}

/**
  * 函数功能: 在 LCD 显示器上使用 Bresenham 算法画圆
  * 输入参数: usX_Center ：在特定扫描方向下圆心的X坐标
  *           usY_Center ：在特定扫描方向下圆心的Y坐标
  *           usRadius：圆的半径（单位：像素）
  *           usColor ：圆的颜色
  *           ucFilled ：选择是否填充该圆
  *             可选值：0：空心圆
  *                     1：实心圆
  * 返 回 值: 无
  * 说    明：无
  */
void LCD_DrawCircle(u16 usX_Center,u16 usY_Center,u16 usRadius,u16 usColor,u8 ucFilled)
{
	s16 sCurrentX, sCurrentY;
	s16 sError;
  
	sCurrentX=0;
  sCurrentY=usRadius;	
	sError=3-(usRadius<<1);   //判断下个点位置的标志
	
	while(sCurrentX<=sCurrentY)
	{
		s16 sCountY;		
		if(ucFilled)
    {			
			for(sCountY=sCurrentX;sCountY<=sCurrentY;sCountY++)
			{                      
				LCD_SetPointPixel(usX_Center+sCurrentX,usY_Center+sCountY,usColor);             //1，研究对象 
				LCD_SetPointPixel(usX_Center-sCurrentX,usY_Center+sCountY,usColor);             //2       
				LCD_SetPointPixel(usX_Center-sCountY,  usY_Center+sCurrentX,usColor);           //3
				LCD_SetPointPixel(usX_Center-sCountY,  usY_Center-sCurrentX,usColor);           //4
				LCD_SetPointPixel(usX_Center-sCurrentX,usY_Center-sCountY,usColor);             //5    
        LCD_SetPointPixel(usX_Center+sCurrentX,usY_Center-sCountY,usColor);             //6
				LCD_SetPointPixel(usX_Center+sCountY,  usY_Center-sCurrentX,usColor);           //7 	
        LCD_SetPointPixel(usX_Center+sCountY,  usY_Center+sCurrentX,usColor);           //0				
			}
    }		
		else
		{          
			LCD_SetPointPixel(usX_Center+sCurrentX,usY_Center+sCurrentY,usColor);             //1，研究对象
			LCD_SetPointPixel(usX_Center-sCurrentX,usY_Center+sCurrentY,usColor);             //2      
			LCD_SetPointPixel(usX_Center-sCurrentY,usY_Center+sCurrentX,usColor);             //3
			LCD_SetPointPixel(usX_Center-sCurrentY,usY_Center-sCurrentX,usColor);             //4
			LCD_SetPointPixel(usX_Center-sCurrentX,usY_Center-sCurrentY,usColor);             //5       
			LCD_SetPointPixel(usX_Center+sCurrentX,usY_Center-sCurrentY,usColor);             //6
			LCD_SetPointPixel(usX_Center+sCurrentY,usY_Center-sCurrentX,usColor);             //7 
			LCD_SetPointPixel(usX_Center+sCurrentY,usY_Center+sCurrentX,usColor);             //0
    }			
		sCurrentX ++;		
		if(sError<0) 
    {
			sError+=(4*sCurrentX+6);	  
		}
		else
		{
			sError +=(10+4*(sCurrentX-sCurrentY));   
			sCurrentY--;
		} 
	}
}

/**
  * 函数功能: 在 LCD 显示器上显示一个英文字符
  * 输入参数: usX：在特定扫描方向下字符的起始X坐标
  *           usY ：在特定扫描方向下该点的起始Y坐标
  *           cChar ：要显示的英文字符
  *           usColor_Background ：选择英文字符的背景色
  *           usColor_Foreground ：选择英文字符的前景色
  *           font：字体选择
  *           可选值：FONT_16 ：16号字体
  *                   FONT_24 ：24号字体 
  * 返 回 值: 无
  * 说    明：该函数必须与ascii.h内容对应使用
  */
void LCD_DispChar_EN( u16 usX, u16 usY, const char cChar, u16 usColor_Background, u16 usColor_Foreground,FONT_Typdef font)
{
	u8 ucTemp, ucRelativePositon, ucPage, ucColumn;
  
  /* 检查输入参数是否合法 */
  assert_param(IS_FONT(font));
  
	ucRelativePositon = cChar - ' ';
  
	if(font==FONT_16)
  {
    LCD_OpenWindow(usX,usY,8,16);
    LCD_WRITE_CMD(0x2C);
    
    for(ucPage=0;ucPage<16;ucPage++)
    {
      ucTemp=ucAscii_1608[ucRelativePositon][ucPage];		
      for(ucColumn=0;ucColumn<8;ucColumn++)
      {
        if(ucTemp&0x01)
          LCD_WRITE_DATA(usColor_Foreground);			
        else
          LCD_WRITE_DATA(usColor_Background);								
        ucTemp >>= 1;					
      }
    }    
  }
  else
  {
    LCD_OpenWindow(usX,usY,12,24);
    LCD_WRITE_CMD(0x2C);
    
    for(ucPage=0;ucPage<48;ucPage++)
    {
      ucTemp=ucAscii_2412[ucRelativePositon][ucPage];		
      for(ucColumn=0;ucColumn<8;ucColumn++)
      {
        if(ucTemp&0x01)
          LCD_WRITE_DATA(usColor_Foreground);			
        else
          LCD_WRITE_DATA(usColor_Background);								
        ucTemp >>= 1;					
      }	
      ucPage++;
      ucTemp=ucAscii_2412[ucRelativePositon][ucPage];
      /* 只显示前面4个位，与什么8位总共12位 */
      for(ucColumn=0;ucColumn<4;ucColumn++)
      {
        if(ucTemp&0x01)
          LCD_WRITE_DATA(usColor_Foreground);			
        else
          LCD_WRITE_DATA(usColor_Background);								
        ucTemp >>= 1;					
      }	
    }
  }	
}

/**
  * 函数功能: 在 LCD 显示器上显示英文字符串
  * 输入参数: usX：在特定扫描方向下字符的起始X坐标
  *           usY ：在特定扫描方向下该点的起始Y坐标
  *           pStr ：要显示的英文字符串的首地址
  *           usColor_Background ：选择英文字符的背景色
  *           usColor_Foreground ：选择英文字符的前景色
  *           font：字体选择
  *           可选值：FONT_16 ：16号字体
  *                   FONT_24 ：24号字体 
  * 返 回 值: 无
  * 说    明：该函数必须与ascii.h内容对应使用
  */
void LCD_DispString_EN ( u16 usX, u16 usY, const char * pstr, u16 usColor_Background, u16 usColor_Foreground,FONT_Typdef font)
{
  /* 检查输入参数是否合法 */
  assert_param(IS_FONT(font));
  
	while(*pstr != '\0')
	{
    if(font==FONT_16)
    {
      if((usX+8)>LCD_DEFAULT_WIDTH)
      {
        usX = 0;
        usY += 16;
      }      
      if((usY+16)>LCD_DEFAULT_HEIGTH)
      {
        usX=0;
        usY=0;
      }      
      LCD_DispChar_EN(usX,usY,*pstr,usColor_Background,usColor_Foreground,FONT_16);
      pstr++;      
      usX+=8;
    }
    else
    {
      if((usX+12)>LCD_DEFAULT_WIDTH)
      {
        usX=0;
        usY+=24;
      }      
      if((usY+24)>LCD_DEFAULT_HEIGTH)
      {
        usX=0;
        usY=0;
      }      
      LCD_DispChar_EN(usX,usY,*pstr,usColor_Background,usColor_Foreground,FONT_24);
      pstr++;      
      usX+=12;
    }
	}
}


void LCD_SetTextColor(volatile u16 Color)
{
  TextColor = Color;
}

void LCD_SetBackColor(volatile u16 Color)
{
  BackColor = Color;
}

#define SCREEN_OFFSET     100

void LCD_DrawRect(u16 usX_Start,u16 usY_Start, u16 usWidth,u16 usHeight)
{
  usX_Start +=SCREEN_OFFSET;
  LCD_DrawLine ( usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start, TextColor );
	LCD_DrawLine ( usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1, TextColor );
	LCD_DrawLine ( usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1, TextColor );
	LCD_DrawLine ( usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1, TextColor );
}

void LCD_ClearLine(volatile u8 Line)
{
  LCD_Clear(SCREEN_OFFSET,Line, 480-SCREEN_OFFSET, 24, BackColor);
}

void LCD_DisplayChar(u8 Line, u16 Column, u8 Ascii)
{
  LCD_DispChar_EN(Column+SCREEN_OFFSET,Line,Ascii,BackColor,TextColor,FONT_24);
}

void LCD_DisplayStringLine(volatile u16 Line, volatile u8 *pstr)
{
  LCD_DispString_EN(SCREEN_OFFSET,Line,(const char *)pstr,BackColor,TextColor,FONT_24);
}



/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
