#ifndef _USART2_H_
#define _USART2_H_

void Uart2_Init(u32 br_num);
void Uart2_IRQ(void);

void Uart2_Put_String(unsigned char *Str);
void Uart2_Put_Buf(unsigned char *DataToSend , u8 data_num);

#endif
