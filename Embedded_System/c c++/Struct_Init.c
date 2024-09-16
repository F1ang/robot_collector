/*
 * @Description: 
 * @Author: YiPei_Fang
 * @Date: 2022-11-08 14:44:01
 */
#include "stdio.h"
#include "Struct_Init.h"

PID1 pid1 = PID1_Init(1,0,0,100, 50, 2000,10, 1, 1);

/* 中断注册函数 */
void system_register_irqhandler(unsigned int irq,system_irq_handler_t handler,void *userParam);

/* 中断服务函数表 */
static sys_irq_handle_t irqTable[160];
system_irq_handler_t CallBack();

void main(void)
{
    printf("%.2f %d\r\n",pid1.Kp,pid1.Max_out);

    system_register_irqhandler(20,CallBack(),NULL);

    return;
}

/*
* @description : 给指定的中断号注册中断服务函数
* @param - irq : 要注册的中断号
* @param - handler : 要注册的中断处理函数
* @param - usrParam : 中断服务处理函数参数
* @return : 无
*/
void system_register_irqhandler(unsigned int irq,system_irq_handler_t handler,void *userParam)
{
    irqTable[irq].irqHandler = handler;
    irqTable[irq].userParam = userParam;
}


system_irq_handler_t CallBack()
{
    printf("callback...\r\n");
}




//通过访问和修改联合体中的定义bitdata成员，可以间接的访问和修改定义的bytedata的值，这可以用在嵌入式的寄存器位操作上

/* 位操作 */
typedef struct
{
  unsigned char bit0:1;
  unsigned char bit1:1;
  unsigned char bit2:1;
  unsigned char bit3:1;
  unsigned char bit4:1;
  unsigned char bit5:1;
  unsigned char bit6:1;
  unsigned char bit7:1;
}bitValue;

typedef union                   /* 内存共享 */
{
  unsigned char bytedata;
  bitValue  bitdata;
}regValue;

int union_bit()
{
  regValue data;
  data.bytedata= 0x5A;
  printf("%d",data.bitdata.bit5);  //读取第6位
  data.bitdata.bit7 = 1;           //修改第8位
  return 0;
}

/* 发送与接收 */
typedef union
{
  unsigned char   data8[4];
  unsigned int  data32;
}dataType;

unsigned int sendData = 0x5A5AA5A5;
unsigned int receiveData;
dataType commSend;
void union_sen_rev(void)
{
    unsigned char commData[128];
    //数据复制
    commSend.data32 = sendData;
    //发送数据,字节复制,不需要再将commData.data32单独移位拆分
    commData[0]= commSend.data8[0];
    commData[1]= commSend.data8[1];
    commData[2]= commSend.data8[2];
    commData[3]= commSend.data8[3];

    //读取数据时,字节复制,不需要再将已经读取到的4个单字节数据拼接
    receiveData =  commSend.data32;
}


/* 
    大端存储就是高字节数据放在低地址。(大高低)
    小端存储就是高字节数据放在高地址。(小高高)

    地址指针函数类型lv_event_cb_t    参数(lv_event_t * e)   返回void
*/



