/**
  ******************************************************************************
  * �ļ�����: bsp_lcd.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2015-10-04
  * ��    ��: Һ���ײ���������ʵ��
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F1Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "bsp_lcd.h"
#include "ascii.h"	
#include "bsp_touch.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
// ���浱ǰ��鵽��Һ��ģ��ID
volatile u32 lcd_id=0;
u16 TextColor=BLACK,BackColor=BACKGROUND;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ���� LCD ����ʱ����
  * �������: nCount ����ʱ����ֵ
  * �� �� ֵ: ��
  * ˵    ������
  */	
static void LCD_DELAY( volatile u32 nCount )
{
  for ( ; nCount != 0; nCount -- );
}

/**
  * ��������: ��ʼ��LCD��IO����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */	
static void LCD_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
  /* ʹ�ܸ���IOʱ�ӣ�����Ϊfsmc���� */
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_AFIO, ENABLE );	  
  
	/* ʹ��FSMC��Ӧ��Ӧ�ܽ�ʱ�� */
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE );
  FSMC_LCD_CS_APBxClock_FUN ( FSMC_LCD_CS_CLK, ENABLE );
  FSMC_LCD_DC_APBxClock_FUN ( FSMC_LCD_DC_CLK, ENABLE );
  FSMC_LCD_BK_APBxClock_FUN ( FSMC_LCD_BK_CLK, ENABLE );
	
  /* ����FSMC���Ӧ��������,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10 */	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_8 | GPIO_Pin_9 | 
																GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init ( GPIOD, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
																GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
																GPIO_Pin_15;
	GPIO_Init ( GPIOE,  & GPIO_InitStructure ); 
	
	/* ����FSMC���Ӧ�Ŀ�����
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
	
  /* ����LCD������ƹܽ�BK*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 	
	GPIO_InitStructure.GPIO_Pin = FSMC_LCD_BK_PIN; 
	GPIO_Init ( FSMC_LCD_BK_PORT, & GPIO_InitStructure );
  /* ��ʼ��ʱ�Ȳ������� */
  GPIO_ResetBits(FSMC_LCD_BK_PORT,FSMC_LCD_BK_PIN);
}

/**
  * ��������: LCD  FSMC ģʽ����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������дʹ����ͬʱ������
  */
static void LCD_FSMC_Config ( void )
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  fsmc_lcd; 	
	
	/* ʹ��FSMCʱ��*/
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );

	fsmc_lcd.FSMC_AddressSetupTime      = 0x02;	 //��ַ����ʱ��
	fsmc_lcd.FSMC_AddressHoldTime       = 0x00;	 //��ַ����ʱ��
	fsmc_lcd.FSMC_DataSetupTime         = 0x05;	 //���ݽ���ʱ��
	fsmc_lcd.FSMC_BusTurnAroundDuration = 0x00;
	fsmc_lcd.FSMC_CLKDivision           = 0x00;
	fsmc_lcd.FSMC_DataLatency           = 0x00;
	fsmc_lcd.FSMC_AccessMode            = FSMC_AccessMode_B;	 //ģʽB�Ƚ�������LCD
	
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
	
	/* ʹ�� FSMC_Bank1_NORSRAM4 */
	FSMC_NORSRAMCmd ( FSMC_LCD_BACKx, ENABLE );  
}

/**
  * ��������: ��ʼ��LCD�Ĵ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������Ҫ������Щ�Ĵ�������Ҫ����ʲôֵ��Һ��������������������أ�
  *           ������Щ�����ɳ����ṩ����ͬ���ҿ��ܲ�ͬ��Ҳ���Ը���ILI9488оƬ
  *           �ֲ����ݲο��޸ġ�
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
 
  /* ������Ļ����ͳߴ� */
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
  * ��������: ��ȡҺ��ģ��ID
  * �������: ��
  * �� �� ֵ: Һ��ģ���ID
  * ˵    ��������ͨ����ȡ04H�Ĵ�����ȡ�õ�Һ��ģ��ID����IDֵ��Һ�����ұ�̣���ͬҺ��
  *           ���ҵ�Һ��ģ��õ���IDֵ���ܲ�ͬ����Ҳ���Էֱ治ͬ�ͺŵ�Һ��ģ�顣
  */
static u32 LCD_ReadID(void)
{
	u16 buf[4];

	LCD_WRITE_CMD(0x04);  
	buf[0] = LCD_READ_DATA();        // ��һ����ȡ������Ч
	buf[1] = LCD_READ_DATA()&0x00ff; // ֻ�е�8λ������Ч
	buf[2] = LCD_READ_DATA()&0x00ff; // ֻ�е�8λ������Ч
	buf[3] = LCD_READ_DATA()&0x00ff; // ֻ�е�8λ������Ч
	return (buf[1] << 16) + (buf[2] << 8) + buf[3];  
}

/**
  * ��������: �����ʼ��������ȡɫ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void Palette_Init(void)
{  
  /* ������Ϊ��ɫ */
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
  * ��������: Һ��ģ���ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
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
  /* ������IO��ʼ�� */
  Touch_Init_GPIO();

  Palette_Init();
  
  /* ������ */
  LCD_BK_ON();  
  return lcd_id;
}

/**
  * ��������: ����LCD��GRAM��ɨ�跽�� 
  * �������: ucOption ��ѡ��GRAM��ɨ�跽�� 
  *           ��ѡֵ��1 :ԭ������Ļ���Ͻ� X*Y=320*480
  *                   2 :ԭ������Ļ���Ͻ� X*Y=480*320
  *                   3 :ԭ������Ļ���½� X*Y=320*480
  *                   4 :ԭ������Ļ���½� X*Y=480*320
  * �� �� ֵ: ��
  * ˵    ������
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
  * D7   MY  Row Address Order     -- ��X�᾵��
  * D6   MX  Column Address Order  -- ��Y�᾵��
  * D5   MV  Row/Column Exchange   -- X����Y�ύ��
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
//   ���Ͻ�->���½� 
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
//		���Ͻ�-> ���½�
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
//		���½�->���Ͻ�
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
//		���½�->���Ͻ�
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
	/* ��ʼ��GRAMд������ */
	LCD_WRITE_CMD (0x2C);	
}

/**
  * ��������: ��LCD��ʾ���Ͽ���һ������
  * �������: usX �����ض�ɨ�跽���´��ڵ����X����
  *           usY �����ض�ɨ�跽���´��ڵ����Y����
  *           usWidth �����ڵĿ��
  *           usHeight �����ڵĸ߶�
  * �� �� ֵ: ��
  * ˵    ������
  */
void LCD_OpenWindow(u16 usX, u16 usY, u16 usWidth, u16 usHeight)
{	
	LCD_WRITE_CMD(0x2A ); 				       /* ����X���� */
	LCD_WRITE_DATA(usX>>8);	             /* ������ʼ�㣺�ȸ�8λ */
	LCD_WRITE_DATA(usX&0xff);	           /* Ȼ���8λ */
	LCD_WRITE_DATA((usX+usWidth-1)>>8);  /* ���ý����㣺�ȸ�8λ */
	LCD_WRITE_DATA((usX+usWidth-1)&0xff);/* Ȼ���8λ */

	LCD_WRITE_CMD(0x2B); 			           /* ����Y����*/
	LCD_WRITE_DATA(usY>>8);              /* ������ʼ�㣺�ȸ�8λ */
	LCD_WRITE_DATA(usY&0xff);            /* Ȼ���8λ */
	LCD_WRITE_DATA((usY+usHeight-1)>>8); /* ���ý����㣺�ȸ�8λ */
	LCD_WRITE_DATA((usY+usHeight-1)&0xff);/* Ȼ���8λ */
}

/**
  * ��������: �趨LCD�Ĺ������
  * �������: usX �����ض�ɨ�跽���´��ڵ����X����
  *           usY �����ض�ɨ�跽���´��ڵ����Y����
  * �� �� ֵ: ��
  * ˵    ������
  */
static void LCD_SetCursor ( u16 usX, u16 usY )	
{
	LCD_OpenWindow ( usX, usY, 1, 1 );
}

/**
  * ��������: ��LCD��ʾ������ĳһ��ɫ������ص�
  * �������: ulAmout_Point ��Ҫ�����ɫ�����ص������Ŀ
  *           usColor ����ɫ
  * �� �� ֵ: ��
  * ˵    ������
  */
#if defined ( __CC_ARM )  // ʹ��Keil���뻷��
static __inline void LCD_FillColor ( u32 ulAmout_Point, u16 usColor )
{
	u32 i = 0;	
	
	/* ��ʼ��GRAMд������ */
	LCD_WRITE_CMD ( 0x2C );	
	
	for ( i = 0; i < ulAmout_Point; i ++ )
		LCD_WRITE_DATA ( usColor );	
}
#elif defined ( __ICCARM__ ) // ʹ��IAR���뻷��
#pragma inline
static void LCD_FillColor ( u32 ulAmout_Point, u16 usColor )
{
	u32 i = 0;	
	
	/* ��ʼ��GRAMд������ */
	LCD_WRITE_CMD ( 0x2C );	
	
	for ( i = 0; i < ulAmout_Point; i ++ )
		LCD_WRITE_DATA ( usColor );	
}
#endif

/**
  * ��������: ��LCD��ʾ����ĳһ������ĳ����ɫ��������
  * �������: usX �����ض�ɨ�跽���´��ڵ����X����
  *           usY �����ض�ɨ�跽���´��ڵ����Y����
  *           usWidth �����ڵĿ��
  *           usHeight �����ڵĸ߶�
  *           usColor ����ɫ
  * �� �� ֵ: ��
  * ˵    ������
  */
void LCD_Clear(u16 usX,u16 usY,u16 usWidth,u16 usHeight,u16 usColor)
{	 
#if 0   /* �Ż�����ִ���ٶ� */
  u32 i;
	u32 n,m;
  /* ��LCD��ʾ���Ͽ���һ������ */
  LCD_OpenWindow(usX,usY,usWidth,usHeight); 
  /* ��ʼ��GRAMд������ */
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
  /* ��LCD��ʾ���Ͽ���һ������ */
  LCD_OpenWindow(usX,usY,usWidth,usHeight);
  /* ��LCD��ʾ������ĳһ��ɫ������ص� */
	LCD_FillColor(usWidth*usHeight,usColor);	
#endif	
}

/**
  * ��������: ��LCD��ʾ����ĳһ����ĳ����ɫ�������
  * �������: usX �����ض�ɨ�跽���´��ڵ����X����
  *           usY �����ض�ɨ�跽���´��ڵ����Y����
  *           usColor ����ɫ
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: ��LCD��ʾ����ĳһ����ĳ����ɫ�������
  * �������: ��
  * �� �� ֵ: u16:��������RGB565
  * ˵    ������
  */
static u16 LCD_Read_PixelData ( void )	
{	
	u16 usR=0, usG=0, usB=0 ;
	
	LCD_WRITE_CMD ( 0x2E );   /* ������ */
	usR = LCD_READ_DATA (); 	/*FIRST READ OUT DUMMY DATA*/
	
	usR = LCD_READ_DATA ();  	/*READ OUT RED DATA  */
	usB = LCD_READ_DATA ();  	/*READ OUT BLUE DATA*/
	usG = LCD_READ_DATA ();  	/*READ OUT GREEN DATA*/	
	
  return (((usR>>11)<<11) | ((usG>>10)<<5) | (usB>>11));
	
}

/**
  * ��������: ��ȡ LCD ��ʾ����ĳһ����������������
  * �������: usX �����ض�ɨ�跽���´��ڵ����X����
  *           usY �����ض�ɨ�跽���´��ڵ����Y����
  * �� �� ֵ: u16:��������RGB565
  * ˵    ������
  */
u16 LCD_GetPointPixel ( u16 usX, u16 usY )
{ 
	u16 usPixelData;
	
	LCD_SetCursor ( usX, usY );
	
	usPixelData = LCD_Read_PixelData ();
	
	return usPixelData;
	
}

/**
  * ��������: �� LCD ��ʾ����ʹ�� Bresenham �㷨���߶�
  * �������: usX1 �����ض�ɨ�跽���´��ڵ����X����
  *           usY1 �����ض�ɨ�跽���´��ڵ����Y����
  *           usX2 �����ض�ɨ�跽�����߶ε���һ���˵�X����
  *           usY2 �����ض�ɨ�跽�����߶ε���һ���˵�Y����
  *           usColor ���߶ε���ɫ
  * �� �� ֵ: ��
  * ˵    ������
  */
void LCD_DrawLine(u16 usX1,u16 usY1,u16 usX2,u16 usY2,u16 usColor)
{
	u16 us; 
	u16 usX_Current, usY_Current;
	s32 lError_X=0,lError_Y=0,lDelta_X,lDelta_Y,lDistance; 
	s32 lIncrease_X, lIncrease_Y;
	
	lDelta_X=usX2-usX1; //������������ 
	lDelta_Y=usY2-usY1; 
	usX_Current = usX1; 
	usY_Current = usY1; 
	
	if(lDelta_X>0)
  {
    lIncrease_X=1; //���õ������� 
	}
  else if(lDelta_X==0)
  {
		lIncrease_X=0;//��ֱ�� 
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
		lIncrease_Y=0;//ˮƽ�� 
	}
	else
  {
    lIncrease_Y=-1;
    lDelta_Y=-lDelta_Y;
  }
	
	if(lDelta_X>lDelta_Y)
  {
		lDistance=lDelta_X; //ѡȡ�������������� 
	}
	else
  {
		lDistance=lDelta_Y; 
  }
	
	for(us=0;us<=lDistance+1;us++)//������� 
	{
		LCD_SetPointPixel(usX_Current,usY_Current,usColor);//���� 
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
  * ��������: ��LCD��ʾ���ϻ�һ������
  * �������: usX_Start �����ض�ɨ�跽���´��ڵ����X����
  *           usY_Start �����ض�ɨ�跽���´��ڵ����Y����
  *           usWidth�����εĿ�ȣ���λ�����أ�
  *           usHeight�����εĸ߶ȣ���λ�����أ�
  *           usColor �����ε���ɫ
  *           ucFilled ��ѡ���Ƿ����þ���
  *             ��ѡֵ��0�����ľ���
  *                     1��ʵ�ľ���
  * �� �� ֵ: ��
  * ˵    ������
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
  * ��������: �� LCD ��ʾ����ʹ�� Bresenham �㷨��Բ
  * �������: usX_Center �����ض�ɨ�跽����Բ�ĵ�X����
  *           usY_Center �����ض�ɨ�跽����Բ�ĵ�Y����
  *           usRadius��Բ�İ뾶����λ�����أ�
  *           usColor ��Բ����ɫ
  *           ucFilled ��ѡ���Ƿ�����Բ
  *             ��ѡֵ��0������Բ
  *                     1��ʵ��Բ
  * �� �� ֵ: ��
  * ˵    ������
  */
void LCD_DrawCircle(u16 usX_Center,u16 usY_Center,u16 usRadius,u16 usColor,u8 ucFilled)
{
	s16 sCurrentX, sCurrentY;
	s16 sError;
  
	sCurrentX=0;
  sCurrentY=usRadius;	
	sError=3-(usRadius<<1);   //�ж��¸���λ�õı�־
	
	while(sCurrentX<=sCurrentY)
	{
		s16 sCountY;		
		if(ucFilled)
    {			
			for(sCountY=sCurrentX;sCountY<=sCurrentY;sCountY++)
			{                      
				LCD_SetPointPixel(usX_Center+sCurrentX,usY_Center+sCountY,usColor);             //1���о����� 
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
			LCD_SetPointPixel(usX_Center+sCurrentX,usY_Center+sCurrentY,usColor);             //1���о�����
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
  * ��������: �� LCD ��ʾ������ʾһ��Ӣ���ַ�
  * �������: usX�����ض�ɨ�跽�����ַ�����ʼX����
  *           usY �����ض�ɨ�跽���¸õ����ʼY����
  *           cChar ��Ҫ��ʾ��Ӣ���ַ�
  *           usColor_Background ��ѡ��Ӣ���ַ��ı���ɫ
  *           usColor_Foreground ��ѡ��Ӣ���ַ���ǰ��ɫ
  *           font������ѡ��
  *           ��ѡֵ��FONT_16 ��16������
  *                   FONT_24 ��24������ 
  * �� �� ֵ: ��
  * ˵    �����ú���������ascii.h���ݶ�Ӧʹ��
  */
void LCD_DispChar_EN( u16 usX, u16 usY, const char cChar, u16 usColor_Background, u16 usColor_Foreground,FONT_Typdef font)
{
	u8 ucTemp, ucRelativePositon, ucPage, ucColumn;
  
  /* �����������Ƿ�Ϸ� */
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
      /* ֻ��ʾǰ��4��λ����ʲô8λ�ܹ�12λ */
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
  * ��������: �� LCD ��ʾ������ʾӢ���ַ���
  * �������: usX�����ض�ɨ�跽�����ַ�����ʼX����
  *           usY �����ض�ɨ�跽���¸õ����ʼY����
  *           pStr ��Ҫ��ʾ��Ӣ���ַ������׵�ַ
  *           usColor_Background ��ѡ��Ӣ���ַ��ı���ɫ
  *           usColor_Foreground ��ѡ��Ӣ���ַ���ǰ��ɫ
  *           font������ѡ��
  *           ��ѡֵ��FONT_16 ��16������
  *                   FONT_24 ��24������ 
  * �� �� ֵ: ��
  * ˵    �����ú���������ascii.h���ݶ�Ӧʹ��
  */
void LCD_DispString_EN ( u16 usX, u16 usY, const char * pstr, u16 usColor_Background, u16 usColor_Foreground,FONT_Typdef font)
{
  /* �����������Ƿ�Ϸ� */
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



/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
