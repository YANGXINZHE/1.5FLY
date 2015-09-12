#include "uart.h"


void UartInit(void)		//9600bps@30.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xF3;		//设定定时初值
	T2H = 0xFC;		//设定定时初值
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x10;		//启动定时器2
}
/*------------------------------------------------
                    发送一个字节
------------------------------------------------*/   
void SendByte(unsigned char dat)
{
 	SBUF = dat;
 	while(!TI);
      TI = 0;
}											  
void send_len(u8 *p ,u8 len)
{
   u8 i=0;
   for(i=0;i<len;i++)
   {
    SendByte(*(p+i));
   }
}										  
/*------------------------------------------------
                    发送一个字符串
------------------------------------------------*/
void SendStr(unsigned char *s)
{
 	while(*s!='\0')// \0 表示字符串结束标志，
                //通过检测是否字符串末尾
  	{
  		SendByte(*s);
  		s++;
 	}
} 