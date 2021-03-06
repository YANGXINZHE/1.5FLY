#include "led.h"

void Delay_ms_led(u16 nms)
{	
	uint16_t i,j;
	for(i=0;i<nms;i++)
		for(j=0;j<8500;j++);
} 

void Debug_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_Structure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	GPIO_Structure.GPIO_Pin =  GPIO_Pin_12;
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入

	GPIO_Init(GPIOB, &GPIO_Structure);
}
void LED_INIT(void)
{
	GPIO_InitTypeDef GPIO_Structure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	GPIO_Structure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_13 | GPIO_Pin_14;//led
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOB, &GPIO_Structure);

	GPIO_Structure.GPIO_Pin =  GPIO_Pin_4;//led
	GPIO_Init(GPIOA, &GPIO_Structure);
	
	GPIO_Structure.GPIO_Pin =  GPIO_Pin_3;  //蓝牙状态
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOB, &GPIO_Structure);	


	Debug_IO_Init();//DEBUG引脚初始化PA15
	GPIO_ResetBits(GPIOB, GPIO_Pin_9);
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);

}

void LED_FLASH(void)
{
	LED1_ON;
	Delay_ms_led(100);
	LED2_ON;
	Delay_ms_led(100);
	LED3_ON;
	Delay_ms_led(100);
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	Delay_ms_led(100);
}
void LED1_ONOFF(void)
{
	static uint8_t busy=0;
	static uint8_t led1_sta=1;
	static uint32_t time_temp;
	if(led1_sta)
	{
		if(!busy)
		{
			time_temp=TIM3_IRQCNT;
			busy=1;
		}
		else if((time_temp+150)<TIM3_IRQCNT)//200 on time
		{
			led1_sta=0;
			LED1_OFF;
			busy=0;
		}
	}
	else
	{
		if(!busy)
		{
			time_temp=TIM3_IRQCNT;
			busy=1;
		}
		else if((time_temp+150)<TIM3_IRQCNT)
		{
			led1_sta=1;
			LED1_ON;
			busy=0;
		}
	}
}
void LED2_ONOFF(void)
{
	static uint8_t busy=0;
	static uint8_t led2_sta=1;
	static uint32_t time_temp;
	if(led2_sta)
	{
		if(!busy)
		{
			time_temp=TIM3_IRQCNT;
			busy=1;
		}
		else if((time_temp+150)<TIM3_IRQCNT)
		{
			led2_sta=0;
			LED2_OFF;
			busy=0;
		}
	}
	else
	{
		if(!busy)
		{
			time_temp=TIM3_IRQCNT;
			busy=1;
		}
		else if((time_temp+150)<TIM3_IRQCNT)
		{
			led2_sta=1;
			LED2_ON;
			busy=0;
		}
	}
}
void LED3_ONOFF(void)
{
	static uint8_t busy=0;
	static uint8_t led3_sta=1;
	static uint32_t time_temp;
	if(led3_sta)
	{
		if(!busy)
		{
			time_temp=TIM3_IRQCNT;
			busy=1;
		}
		else if((time_temp+150)<TIM3_IRQCNT)
		{
			led3_sta=0;
			LED3_OFF;
			busy=0;
		}
	}
	else
	{
		if(!busy)
		{
			time_temp=TIM3_IRQCNT;
			busy=1;
		}
		else if((time_temp+150)<TIM3_IRQCNT)
		{
			led3_sta=1;
			LED3_ON;
			busy=0;
		}
	}
}

