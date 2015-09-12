/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "sysconfig.h"
#include "bsp.h"
#include "led.h"
#include "tim3.h"	
#include "MPU6050.h"
#include "moto.h"
#include "spi.h"
#include "nrf24l01.h"
#include "tim_pwm_in.h"
#include "rc.h"
#include "imu.h"
#include "control.h"
#include "data_transfer.h"

#include "delay.h"
#include "sys.h"
#include "adc.h"


u8 SYS_INIT_OK=0;
bool DEBUG_EN = FALSE;
////////////////////////////////////////////////////////////////////////////////
void SYS_INIT(void)
{
	LED_INIT();
	Moto_Init(); 
	delay_init();
    LED_FLASH();

    if(PBin(12)==0)//判断调试   //引脚设置默认不调试
    {
      delay_ms(10);
	    if(PBin(12)==0)
			  DEBUG_EN = TRUE;//调试 
	  }	
	  else 
			  DEBUG_EN = FALSE;//不调试 
	CLI();
	FLASH_Unlock();
	EE_INIT();
	EE_READ_ACC_OFFSET();
	EE_READ_GYRO_OFFSET();
	EE_READ_PID();
  SEI();
	ADC1_Init();
	Tim3_Init(500);
#ifdef CONTROL_USE_RC		//若开启航母遥控控制宏,则进行PWM采集初始化
	Tim_Pwm_In_Init();
#endif
	Nvic_Init();
	if(DEBUG_EN == TRUE)
	{
//		Uart1_Init(9600);
//		LED_FLASH();
//		LED_FLASH();
//		Uart1_Put_String("AT+BAUD");
		LED_FLASH();
		Uart1_Init(115200);
//		LED_FLASH();
//		LED_FLASH();
//		Uart1_Put_String("AT+BAUD");
//		LED_FLASH();
//		Uart1_Put_String("AT+NAMELY_FLY");  
//			LED_FLASH();
//		Uart1_Put_String("AT+NAMBLY_FLY");   
	}
	else
	{
	//	Uart2_Init(9600);
		//LED_FLASH();
	//	Uart2_Put_String("AT+BAUD6");
		//Uart2_Init(38400);
	//	Uart2_Init(460800);
	//	LED_FLASH();
		Uart2_Put_String("AT+BAUD6");
		
		Uart2_Init(115200);		
	//	LED_FLASH();
	//	Uart2_Put_String("AT+BAUD6");
		
//		LED_FLASH();
//		Uart2_Put_String("AT+DUAL1");
		LED_FLASH();
		//Uart2_Put_String("AT+PIO01"); 
	 // Uart2_Put_String("AT+NAMBFLY1.5_BLE"); 
	// 	LED_FLASH();
	//Uart2_Put_String("AT+NAMEFLY1.5_SPP"); 	
	//	LED_FLASH();
	}
	
  i2cdevInit(I2C1);
	MPU6050_Init(I2C1);
//	MS5611_Init();
	
	Spi1_Init();
	Nrf24l01_Init(MODEL_TX2,40);
	if(DEBUG_EN == TRUE)
	{
	 	if(Nrf24l01_Check())	
			Uart1_Put_String("NRF24L01 IS OK !\r\n");
	 	else 									
			Uart1_Put_String("NRF24L01 IS NOT OK !\r\n"); 
	}

	else 
	{
		 	if(Nrf24l01_Check())	
			Uart2_Put_String("NRF24L01 IS OK !\r\n");
	 	else 									
			Uart2_Put_String("NRF24L01 IS NOT OK !\r\n"); 
	}
    ADC_Control(1);
	Tim3_Control(1);
}

////////////////////////////////////////////////////////////////////////////////
u8 FLAG_ATT=0;
T_int16_xyz 		Acc,Gyr;	//两次综合后的传感器数据
T_int16_xyz			Acc_AVG;
T_float_angle 		Att_Angle;	//ATT函数计算出的姿态角
vs32				Alt;
T_RC_Data 			Rc_D;		//遥控通道数据
T_RC_Control		Rc_C;		//遥控功能数据
int main(void)
{
	static u8 att_cnt=0;
	static u8 rc_cnt=0;
	static T_int16_xyz mpu6050_dataacc1,mpu6050_dataacc2,mpu6050_datagyr1,mpu6050_datagyr2;
	static u8 senser_cnt=0,status_cnt=0,dt_rc_cnt=0,dt_moto_cnt=0;
	static u8 OneRoll_cnt=0;
	u8 led_num =0;
	bool led_state = true ;
	
	SYS_INIT();
	Rc_C.ARMED = 0;//上电锁定飞控
	Rc_C.OneRoll= 0;//上电清楚翻滚标志
	while (1)
	{		
		//if(PBin(3)==1)  PBout(14)=0;
	//	else if(DEBUG_EN == TRUE) PAout(12)=0;	
   // else 		
		PBout(14)=1;
	//  if(!BT_LOW) LED1_ONOFF();
		if(FLAG_ATT)//TIM3中置位 1MS
		{
			FLAG_ATT = 0;
			att_cnt++;
			rc_cnt++;

			if(link_num-- == 0 && DEBUG_EN == false)//1000ms   连接断开
			{
			  Rc_C.ARMED=0;
				link_num=0;
			}
			if(rc_cnt==20)//20ms
			{
				rc_cnt = 0;
				#ifdef CONTROL_USE_RC
				Rc_GetValue(&Rc_D);
				#endif
				Rc_Fun(&Rc_D,&Rc_C);
			}
			if(att_cnt==1)//前1ms
				MPU6050_Dataanl(&mpu6050_dataacc1,&mpu6050_datagyr1);
			else//后1ms
			{   //相当于2ms计算一次
				att_cnt = 0;
				MPU6050_Dataanl(&mpu6050_dataacc2,&mpu6050_datagyr2);
				Acc.X = (mpu6050_dataacc1.X+mpu6050_dataacc2.X)/2;
				Acc.Y = (mpu6050_dataacc1.Y+mpu6050_dataacc2.Y)/2;
				Acc.Z = (mpu6050_dataacc1.Z+mpu6050_dataacc2.Z)/2;
				Gyr.X = (mpu6050_datagyr1.X+mpu6050_datagyr2.X)/2;
				Gyr.Y = (mpu6050_datagyr1.Y+mpu6050_datagyr2.Y)/2;
				Gyr.Z = (mpu6050_datagyr1.Z+mpu6050_datagyr2.Z)/2;
				Prepare_Data(&Acc,&Acc_AVG);//加速度计滤波
				IMUupdate(&Gyr,&Acc_AVG,&Att_Angle);//四元数计算
				//Att_Angle;	//ATT函数计算出的姿态角
				//Acc,Gyr;	//两次综合后的传感器数据
				//Rc_D;		//遥控数据
				if(Rc_C.OneRoll)
				{
					LED2_ON;
					LED3_ON;
				  Rc_D.THROTTLE += OneRoll_cnt/8;
					if(OneRoll_cnt++ >200)//2ms*500
					{
					  LED2_OFF;
						LED3_OFF;
						OneRoll_cnt=0;
						Rc_C.OneRoll= 0 ;
					}
						
				}
				Control(&Att_Angle,&Gyr,&Rc_D,Rc_C.ARMED);//四轴控制
				if(Rc_C.ARMED)//解锁指示
					LED1_ONOFF();
				else
					LED1_OFF;
					
				senser_cnt++;
				status_cnt++;
				dt_rc_cnt++;
				dt_moto_cnt++;
				if(senser_cnt==5)//10ms
				{
					senser_cnt = 0;
					Send_Senser = 1;//发送传感器数据
				}
				if(status_cnt==5)//10ms
				{
					status_cnt = 0;
					Send_Status = 1;//发送状态数据
				}
				if(dt_rc_cnt==10)//20ms
				{
					dt_rc_cnt=0;
					Send_RCData = 1;//发送遥控数据
				}
				if(dt_moto_cnt==10)//20ms
				{
					dt_moto_cnt=0;
					Send_MotoPwm = 1;//发送电机数据
				}
			}
			
			if(BT_low_state == true)
			{
				 if(led_state == true)
				 {
						LED2_ON;
						LED3_ON;
						 if(led_num++>250)
						 {
							led_num =0;
							led_state =false;
						 }		 
				 }
				 else 
				 {
						LED2_OFF;
						LED3_OFF;
						 if(led_num++>250)
						 {
							led_num=0;
							led_state =true;
						 }
				 }	 
			}
			
		}
	}
}
////////////////////////////////////////////////////////////////////////////////

