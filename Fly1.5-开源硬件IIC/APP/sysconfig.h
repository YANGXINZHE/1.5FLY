#ifndef _SYSCONFIG_H_
#define _SYSCONFIG_H_
#include <stdbool.h>
#define CLI()      __set_PRIMASK(1)  
#define SEI()      __set_PRIMASK(0)

#define LIMIT(a, x, b)  if((x)<(a))(x)=(a);else if((x)>(b))(x)=(b);

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

#define NVIC_UART_P	5
#define NVIC_UART_S	1
#define NVIC_TIM3_P	2
#define NVIC_TIM3_S	1
#define NVIC_TIM4_P	0
#define NVIC_TIM4_S	1

#define DATA_TRANSFER_USE_USART
//#define DATA_TRANSFER_USE_SPI_NRF

//#define CONTROL_USE_RC  //遥控器
#define CONTROL_USE_DATATRANSFER  //数据传送

#define USE_IIC1

//#define USE_USART1  //使用UART1做调试用

#include "usart1.h"
#include "usart2.h"

#include "i2cdev.h"
#include "i2croutines.h"
#include "stm32f10x.h"

#define RC_FUN_MIN	1280
#define RC_FUN_MAX	1750
#define RC_MIN	900
#define RC_MAX	2100

typedef struct{
				float rol;
				float pit;
				float yaw;}T_float_angle;
typedef struct{
				float X;
				float Y;
				float Z;}T_float_xyz;
typedef struct{
				int16_t X;
				int16_t Y;
				int16_t Z;}T_int16_xyz;
typedef struct int16_rcget{
				int16_t ROLL;
				int16_t PITCH;
				int16_t THROTTLE;
				int16_t YAW;
				int16_t AUX1;
				int16_t AUX2;
				int16_t AUX3;
				int16_t AUX4;
				int16_t AUX5;
				int16_t AUX6;}T_RC_Data;
typedef struct{
				u8	ARMED;
				u8 OneRoll;}T_RC_Control;//ARMED = 1 ->?? 
#define TRUE true
#define FALSE false
#define Debug_IO  PBin(12)
extern bool BT_LOW;
extern bool DEBUG_EN;
extern u8 FLAG_ATT;
extern T_int16_xyz 		Acc,Gyr;	//???????????
extern T_float_angle 	Att_Angle;	//ATT?????????
extern vs32				Alt;
extern T_RC_Data 		Rc_D;		//??????
extern T_RC_Control		Rc_C;		//??????
extern u32				Debug_cnt;
				
extern u8 TxBuffer[256];
extern u8 TxCounter;
extern u8 count;
extern  u8 RxBuffer[50];
extern  u8 RxState ;	
extern  u16 link_num ;
				
extern  bool BT_low_state;
#endif
