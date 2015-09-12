#ifndef __uart_H
#define __uart_H

#include "sysconfig.h"

void UartInit(void);		//9600bps@30.000MHz
void SendByte(unsigned char dat);
void send_len(u8 *p ,u8 len);	

#endif