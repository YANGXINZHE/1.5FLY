//**********************************遥控器程序RC1.0*************************
//          本程序（左手油门）
//左手上下为油门，左右为旋转
//右手上下为俯仰，左右为横滚
//MCU工作频率28MHZ！！！
#include <rtx51tny.h>
#include <STC15Fxxxx.H>

#include <NRF24L01.H>
#include <AD.H>
#include "sysconfig.h"

ADCResult  GetADC;
ADCResult  OFFSETADC;

unsigned char idata  AngleOffsetBuf[10];
int AngleOffset[2];//AngleOffset[2]={rol,pit}
bit key_flag =1;

T_RC_Data  Rc_D;
u8 TX_FLAG = 0;
sbit  IN_OFF=P3^0;//串口接收端

/***************************************************************
** 函数名称: STC_ISP
** 功能描述: STC12C5A60S2免断电烧录程序
** 输　入:   
** 输　出:   
** 全局变量: 
***************************************************************/
 void STC_ISP()
 {
    
	IN_OFF=1; //——2

    if(!IN_OFF){IAP_CONTR=0x60;} //判断串口是否有数据过来
  
 } 
u8 FQCheck()
{	
 u8 FQ=0;

	CE=0;
	NRF24L01_Write_Buf(0x20+0x10,(u8*)ip1,5);//D′TX?úμ?μ??·
	NRF24L01_Write_Buf(0X20+0X0A,(u8*)ip1,5);//D′RX1?óê?μ??·
	CE=1;
 	while(1)//一直接收
 	{	STC_ISP();	          //ISP 下载不用冷启动 本函数不用
		NRF24L01_RX_Mode(FQ);
		if(NRF24L01_RxPacket(RxBuf))
		{	
		   // send_len(RxBuf,24) ;
			if(RxBuf[0]==0xAA && RxBuf[1]==0xFF)
			{		    
				FQ = RxBuf[2];//对应51822返回的频率 
				if(FQ != RxBuf[3])
					 	FQ = RxBuf[4];
		 		else if(FQ != RxBuf[5])
			        	FQ = RxBuf[6];
				//if(FQ-2 == temp) 
					break;
			}//RLED3 =LEDON;delayms(500);	  	
		}
		else 
		   FQ +=2;//对频

		if(FQ >=60) FQ = 0;
		RLED3 =LEDON;
		delayms(100);
		RLED3 =LEDOFF;
		delayms(100);
	}
	TX_FLAG = 1;
	SendByte(FQ);
	return FQ;
}
void sys_init()
{		u8 i=0;
        //M1M0 =00--->Standard,	01--->push-pull
        //M1M0 =10--->pure input,11--->open drain
		P1M1=0xFE;  //P1设为高阻模式
		P1M0=0x40;
		
		P0M1=0X00;  //其他I/O口设置为准双向，弱上拉模式
		P0M0=0XE0;
		
		P2M1=0X00;
		P2M0=0X00;
		
		P3M1=0X00;
		P3M0=0X00;
		
		P4M1=0X00;
		P4M0=0X00;
		
		P5M1=0X00;
		P5M0=0X00;

		adc_init();       //初始化AD检测模块

		UartInit();		//9600bps@30.000MHz
		//init_NRF24L01();  //初始化无线模块

		for(i=-0;i<2;i++)
		  AngleOffset[i]=IapReadByte(IAP_ADDRESS+i);//读出数据
	
		OFFSETADC.YAW = getADCResult(YAW_CH)-128;  //记录上电时摇杆的数据作为中位修正，因为摇杆中位要为128即256/2
		delayms(10);
		OFFSETADC.PIT = getADCResult(PIT_CH)-128;
		delayms(10);
		OFFSETADC.ROLL = getADCResult(ROLL_CH)-128;        
		delayms(10);

		while(NRF24L01_Check())
		{
		  RLED3 =LEDON;
		  delayms(1000);
		  RLED3 =LEDOFF;
		  delayms(1000);		  
		}
		NRF24L01_TX_Mode(FQCheck());
	//	NRF24L01_TX_Mode(24);
		CE=0;
		NRF24L01_Write_Buf(0x20+0x10,(u8*)ip1,5);//D′TX?úμ?μ??·
		NRF24L01_Write_Buf(0X20+0X0A,(u8*)ip1,5);//D′RX1?óê?μ??·
		CE=1;
		delayms(10);
}
void Send_OffsetData(u8 *p)//OFFSET 角度微调
{
	u8 cnt=0,sum=0,i=0;
	*(p+(cnt++))=0xAA;	//0
	*(p+(cnt++))=0xAF;	//1
	*(p+(cnt++))=0X16;	//2	 //	//OFFSET  角度微调
	*(p+(cnt++))=0;		//3
	*(p+(cnt++))=BYTE0(AngleOffset[0]);//4
	*(p+(cnt++))=BYTE1(AngleOffset[0]);//5
	*(p+(cnt++))=BYTE0(AngleOffset[1]); //6
	*(p+(cnt++))=BYTE1(AngleOffset[1]); //7

	*(p+3) = cnt-4;

	for(i=0;i<cnt;i++)
		sum += *(p+i);
	
	*(p+(cnt++))=sum;
//	send_len(p,cnt);

}
void Send_RCData(u8 *p)
{
	u8 cnt=0,sum=0,i=0;
	*(p+(cnt++))=0xAA;	//0
	*(p+(cnt++))=0xAF;	//1
	*(p+(cnt++))=0x03;	//2
	*(p+(cnt++))=0;		//3
	*(p+(cnt++))=BYTE0(Rc_D.THROTTLE);//4
	*(p+(cnt++))=BYTE1(Rc_D.THROTTLE);//5
	*(p+(cnt++))=BYTE0(Rc_D.YAW); //6
	*(p+(cnt++))=BYTE1(Rc_D.YAW); //7
	*(p+(cnt++))=BYTE0(Rc_D.ROLL);//8
	*(p+(cnt++))=BYTE1(Rc_D.ROLL);//9
	*(p+(cnt++))=BYTE0(Rc_D.PITCH);//10
	*(p+(cnt++))=BYTE1(Rc_D.PITCH);//11
	*(p+(cnt++))=BYTE0(Rc_D.AUX1);//12
	*(p+(cnt++))=BYTE1(Rc_D.AUX1);//13
	*(p+(cnt++))=BYTE0(Rc_D.AUX2);//14
	*(p+(cnt++))=BYTE1(Rc_D.AUX2);//15
	*(p+(cnt++))=BYTE0(Rc_D.AUX3);//16
	*(p+(cnt++))=BYTE1(Rc_D.AUX3);//17
	*(p+(cnt++))=BYTE0(Rc_D.AUX4);//18
	*(p+(cnt++))=BYTE1(Rc_D.AUX4);//19

	*(p+3) = cnt-4;

	for(i=0;i<cnt;i++)
		sum += *(p+i);
	
	*(p+(cnt++))=sum;
//	send_len(p,cnt);

}
void AD() _task_ 0 //获取AD采样数据
{
 	sys_init();   //初始化
	os_create_task (1);  //启动进程1
	os_create_task (2);  //启动进程2
  while(1)
  {
 	GetADC.THR  = getADCResult(THR_CH);   //读取4个摇杆通道每个通道的8位数据，取值范围0-255
    delayms(1);
	GetADC.BAT  = (u16)((getADCResult(BAT_CH)*4.96*100)/256);//电池电压检测通道  
    delayms(1);
	GetADC.PIT  = getADCResult(PIT_CH);
	delayms(1);
	GetADC.ROLL = getADCResult(ROLL_CH);
	delayms(1);
	GetADC.YAW  = getADCResult(YAW_CH);
//	 SendByte(BYTE0(GetADC.YAW)) ;
//	 SendByte(BYTE1(GetADC.YAW)) ;
//	 SendByte(0x11) ;
	if(GetADC.BAT <= 310)//低于3.1V亮红灯
	{
	  RLED1 = LEDON;
	  GLED = LEDOFF;
	}
	else
	{
	  RLED1 = LEDOFF;
	  GLED = LEDON;
	}
	if(!APL)//遥控器重启
	{
	  delayms(10);
	  if(!APL)
	      IAP_CONTR = 0x20;           //软件复位,系统重新从用户代码区开始运行程序	
	}
	os_wait(K_IVL,3,0);//延时3个节拍 
  }

}
void RCDATASEND()  _task_ 1	 //遥控数据
{	
	int temp=0;
	u8 i=0;
  	while(1)
	{
		temp = GetADC.PIT - OFFSETADC.PIT;//PIT  用上电记录的数据对采样数据进行修正，保证摇杆中位时数据为128
		LIMIT(0,temp,255);
		Rc_D.PITCH = (u16)(RC_FACTOR*temp)+1100;	//3.92

		temp = GetADC.ROLL - OFFSETADC.ROLL;//ROLL
		LIMIT(0,temp,255);
		Rc_D.ROLL = (u16)(RC_FACTOR*temp)+1100;

		temp = GetADC.YAW ;//- OFFSETADC.YAW; //YAW
		LIMIT(0,temp,255);
		Rc_D.YAW = (u16)(RC_FACTOR*temp)+1100;
		LIMIT(1190,Rc_D.YAW,1810);

	//	 SendByte(BYTE0(Rc_D.YAW)) ;
	//	 SendByte(BYTE1(Rc_D.YAW)) ;
	//	 SendByte(0x11) ;

		Rc_D.THROTTLE = (u16)(3.5*GetADC.THR)+1000; //THR
		LIMIT(1000,Rc_D.THROTTLE,2000); //油门通道不需处理，直接发送AD检测的8位数据即可，根据AD采样原理易知读取的AD采样数据不可能为负也不可能大于255

		if(Rc_D.THROTTLE <=1200)//油门过小
		{
		//	Rc_D.PITCH = 1500;
		   // Rc_D.ROLL = 1500;
		   // Rc_D.YAW = 1500; 
			RLED2 = LEDOFF;
		}  
		else
		{
		  RLED2 = LEDON;
		}
		Send_RCData(TxBuf);//数据准备
	//	if(TX_FLAG == 1)
		{
			if(NRF24L01_TxPacket(TxBuf)==TX_OK)//发射数据	
			{			
				RLED3 =LEDOFF;
			}
			else 
			{
	          RLED3 =LEDON;
			}
		}

//***********************************************************************************************
	 if(!ROLLAdd)//rol+
	 {
	  delayms(5);
	  if(!ROLLAdd)//rol+
	  {
	   if(AngleOffset[0]<2500)
	   {
	   	 AngleOffset[0] +=500;
	   }
	   key_flag = 1;
	  } 
	 }
	 else if(!ROLLCut)//rol-
	 {
	  delayms(5);
	  if(!ROLLCut)//rol-
	  {
	   if(AngleOffset[0]>-2500)
	   {
	   	 AngleOffset[0] -=500;
	   }
	   key_flag = 1;
	  } 
	 }
	 else if(!PITAdd)//pit+
	 {
	  delayms(5);
	  if(!PITAdd)//pit+
	  {
	   if(AngleOffset[1]<2500)
	   {
	   	 AngleOffset[1] +=500;
	   }
	   key_flag = 1;
	  } 
	 }
	 else if(!PITCut)//pit-
	 {
	  delayms(5);
	  if(!PITCut)//pit-
	  {
	   if(AngleOffset[1]>-2500)
	   {
	   	 AngleOffset[1] -=500;
	   }
	   key_flag = 1;
	  } 
	 }
	 if(key_flag==1)
	 {	  
	     RLED4 =LEDON;   
		 IapEraseSector(IAP_ADDRESS);//清除数据
		 for(i=0;i<2;i++)
		   IapProgramByte(IAP_ADDRESS+i,AngleOffset[i]);//写入数据
		 Send_OffsetData(AngleOffsetBuf);//数据准备
		 NRF24L01_TxPacket(AngleOffsetBuf);//发射数据
		 key_flag = 0;		 	
		// os_wait(K_IVL,2,0); //给一定延时让数据发送完成 
		 RLED4 =LEDOFF;  
	 } 

//*******************************************************************************************************************
		STC_ISP();	          //ISP 下载不用冷启动 本函数不用
		os_wait(K_IVL,2,0); //给一定延时让数据发送完成    
	}
}

//void OFFSETSEND()  _task_ 2	 //遥控数据
//{
// u8 i = 0;
// while(1)
// {
//	 if(!ROLLAdd)//rol+
//	 {
//	  delayms(5);
//	  if(!ROLLAdd)//rol+
//	  {
//	   if(AngleOffset[0]<2500)
//	   {
//	   	 AngleOffset[0] +=500;
//	   }
//	   key_flag = 1;
//	  } 
//	 }
//	 else if(!ROLLCut)//rol-
//	 {
//	  delayms(5);
//	  if(!ROLLCut)//rol-
//	  {
//	   if(AngleOffset[0]>-2500)
//	   {
//	   	 AngleOffset[0] -=500;
//	   }
//	   key_flag = 1;
//	  } 
//	 }
//	 else if(!PITAdd)//pit+
//	 {
//	  delayms(5);
//	  if(!PITAdd)//pit+
//	  {
//	   if(AngleOffset[1]<2500)
//	   {
//	   	 AngleOffset[1] +=500;
//	   }
//	   key_flag = 1;
//	  } 
//	 }
//	 else if(!PITCut)//pit-
//	 {
//	  delayms(5);
//	  if(!PITCut)//pit-
//	  {
//	   if(AngleOffset[1]>-2500)
//	   {
//	   	 AngleOffset[1] -=500;
//	   }
//	   key_flag = 1;
//	  } 
//	 }
//	 if(key_flag==1)
//	 {	  
//	     RLED4 =LEDON;   
//		 IapEraseSector(IAP_ADDRESS);//清除数据
//		 for(i=0;i<2;i++)
//		   IapProgramByte(IAP_ADDRESS+i,AngleOffset[i]);//写入数据
//		 Send_OffsetData(AngleOffsetBuf);//数据准备
//		 NRF24L01_TxPacket(AngleOffsetBuf);//发射数据
//		 key_flag = 0;		 	
//		// os_wait(K_IVL,2,0); //给一定延时让数据发送完成 
//		 RLED4 =LEDOFF;  
//	 } 	
// }
//	
//}








