#include "delay.h"

void Delay1ms()		//@30.000MHz
{
	unsigned char i, j;

	i = 25;
	j = 43;
	do
	{
		while (--j);
	} while (--i);
}
void delayms(u16 n)
{
 while(n--)
  Delay1ms();
}

void Delay1us()		//@30.000MHz
{
	unsigned char i;

	i = 2;
	while (--i);
}
void delayus(u16 n)
{ 
  while (n--) Delay1us();
}

