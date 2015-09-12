#ifndef __eeprom_H__
#define __eeprom_H__
//#include <stc12c5a60s2.h>
#include <intrins.h>
#include "delay.h"

/*Declare SFR associated with the IAP 
sfr IAP_DATA    =   0xC2;           //Flash data register
sfr IAP_ADDRH   =   0xC3;           //Flash address HIGH
sfr IAP_ADDRL   =   0xC4;           //Flash address LOW
sfr IAP_CMD     =   0xC5;           //Flash command register
sfr IAP_TRIG    =   0xC6;           //Flash command trigger
sfr IAP_CONTR   =   0xC7;           //Flash control register

	for(j=0;j<5;j++)//5行
	  for(k=0;k<4;k++)//4列
	    IC_EEPROM[j][k]=IapReadByte(IAP_ADDRESS+j*4+k);//读出数据
														  */
/*Define ISP/IAP/EEPROM command*/
#define CMD_IDLE    0               //Stand-By
#define CMD_READ    1               //Byte-Read
#define CMD_PROGRAM 2               //Byte-Program
#define CMD_ERASE   3               //Sector-Erase

/*Define ISP/IAP/EEPROM operation const for IAP_CONTR*/
//#define ENABLE_IAP 0x80           //if SYSCLK<30MHz
//#define ENABLE_IAP 0x81           //if SYSCLK<24MHz
//#define ENABLE_IAP  0x82            //if SYSCLK<20MHz
#define ENABLE_IAP 0x83           //if SYSCLK<12MHz
//#define ENABLE_IAP 0x84           //if SYSCLK<6MHz
//#define ENABLE_IAP 0x85           //if SYSCLK<3MHz
//#define ENABLE_IAP 0x86           //if SYSCLK<2MHz
//#define ENABLE_IAP 0x87           //if SYSCLK<1MHz

//Start address for STC11/10xx EEPROM
#define IAP_ADDRESS 0x0100

void IapIdle();
uchar IapReadByte(uint addr);
void IapProgramByte(uint addr, uchar dat);
void IapEraseSector(uint addr);

#endif