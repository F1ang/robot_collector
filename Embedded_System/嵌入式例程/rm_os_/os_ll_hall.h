typedef struct
{
	struct
	{//通道和拨码
		unsigned short ch0;
		unsigned short ch1;
		unsigned short ch2;
		unsigned short ch3;
		unsigned char s1; 
		unsigned char s2;
	}rc;
	
	struct
	{//鼠标
		unsigned short x;
		unsigned short y;
		unsigned short z;
		unsigned char press_l; 
		unsigned char press_r;
	}mouse;
	
	struct
	{//其他键鼠
		unsigned short v;
	}key;
}RC_Ctl_t;



