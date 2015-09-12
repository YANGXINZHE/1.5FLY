#include <STC15Fxxxx.H>
#include <intrins.h>
#include <NRF24L01.H>
#include <AD.H>		
#include "sysconfig.h"	

//*********************************初始化A/D转换*************************************************
void adc_init()
{
	P1ASF=0x7e;
	ADC_RES=0;	 
	ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
	delayms(10); //适当延时
}
//**********************************获得A/D转换的数据********************************************
u8 ReadADC(u8 ch)
{
	ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
	NOP(5);
	while (!(ADC_CONTR & ADC_FLAG));//等待转换完成
	ADC_CONTR &= ~ADC_FLAG; //关闭adc
	return ADC_RES;
}
u8 getADCResult(u8 ch)
{
 u8 i =0;
 u16 sum = 0;
 for(i=0;i<8;i++)
  sum += ReadADC(ch);
 return (u8)(sum>>3);
}



