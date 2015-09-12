#ifndef _SYSCONFIG_H_
#define _SYSCONFIG_H_

#include <STC15Fxxxx.H>
#include "delay.h"
#include "uart.h"
#include "EEPROM.H"


#define LIMIT(a, x, b)  if((x)<(a))(x)=(a);else if((x)>(b))(x)=(b);

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
//*****************LED*********************
#define RLED1 P20
#define RLED2 P44
#define RLED3 P43
#define RLED4 P42
#define RLED5 P41
#define GLED  P37
//*****************KEY*********************
#define ROLLAdd P06//ROLL+
#define ROLLCut P10//ROLL-
#define PITAdd  P07//PIT+
#define PITCut  P05//PIT-

#define APL     P21 //无头
#define RC3D    P22 //体感遥控
#define KEYROLL P23//一键翻滚
#define RETURN  P04//一键返回
//*****************ADC*********************
#define YAW_CH  1//P15 ADC5
#define THR_CH  5//P11 ADC1
#define PIT_CH  3//P13 ADC3
#define ROLL_CH 4//P14 ADC4
#define BAT_CH  2//P12 ADC2
//*****************IIC*********************
#define SCL P00
#define SDA P01
#define MPUINT P02
//*****************OLED*********************
#define OLEDRST P54
//****************HMC5883*********************
#define HMCINT P03
//****************24L01*********************
#define	CE	  P46
#define	CSN	  P45
#define	SCK	  P27
#define MOSI  P26
#define MISO  P25
#define	IRQ	  P24
extern unsigned char idata TxBuf[24];
extern unsigned char idata RxBuf[24];
extern unsigned char idata  AngleOffsetBuf[10];

#define RC_FACTOR   3.13//4.31//1.17//2.35//1.56//3.96
#define RC_FUN_MIN	1200
#define RC_FUN_MAX	1800
#define RC_MIN	900
#define RC_MAX	2100

#define LEDON  0
#define LEDOFF 1

typedef struct{
				float rol;
				float pit;
				float yaw;}T_float_angle;
typedef struct{
				float X;
				float Y;
				float Z;}T_float_xyz;
typedef struct{
				int X;
				int Y;
				int Z;}T_int16_xyz;
typedef struct {
				u16 ROLL;
				u16 PITCH;
				u16 THROTTLE;
				u16 YAW;
				u16 AUX1;
				u16 AUX2;
				u16 AUX3;
				u16 AUX4;
				u16 AUX5;
				u16 AUX6;}T_RC_Data;
typedef struct{
				u8	ARMED;
				u8 OneRoll;}T_RC_Control;//ARMED = 1 ->?? 
typedef struct{
				u8 YAW;
				u8 THR;
				u8 PIT;
				u8 ROLL;
				u16 BAT; }ADCResult;

extern ADCResult  GetADC; 
extern ADCResult  OFFSETADC; 
extern bit BT_LOW;
extern bit DEBUG_EN;
extern u8 FLAG_ATT;
extern T_int16_xyz 		Acc,Gyr;	//???????????
extern T_float_angle 	Att_Angle;	//ATT?????????
extern u32				sys_cnt;
extern T_RC_Data 		Rc_D;		//??????
extern T_RC_Control		Rc_C;		//??????
				
extern  bit BT_low_state;
#endif
