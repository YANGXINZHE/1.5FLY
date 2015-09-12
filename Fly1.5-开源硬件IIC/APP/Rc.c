#include "rc.h"

//#define a
/*
#ifdef a
void Rc_Fun(T_RC_Data *rc_in,T_RC_Control *rc_ct) //???
{
	static u8 cnt_arm=0,cnt_fun=0;
	if(rc_in->THROTTLE > RC_MIN && rc_in->THROTTLE < RC_FUN_MIN && rc_in->YAW > RC_MIN && rc_in->YAW < RC_FUN_MIN)
	{
		cnt_arm++;
		if(cnt_arm==75)
		{
			cnt_arm=0;
			rc_ct->ARMED = 0;
		}
	}
	else if(rc_in->THROTTLE > RC_MIN && rc_in->THROTTLE < RC_FUN_MIN && rc_in->YAW > RC_FUN_MAX && rc_in->YAW < RC_MAX)
	{
		cnt_arm++;
		if(cnt_arm==75)
		{
			cnt_arm=0;
			rc_ct->ARMED = 1;
		}
	}
	else
		cnt_arm = 0;
		
	if(rc_ct->ARMED==1)
		return;
	
	if(rc_in->THROTTLE > RC_MIN && rc_in->THROTTLE<RC_FUN_MIN && rc_in->ROLL<RC_FUN_MIN && rc_in->ROLL > RC_MIN)
	{
		cnt_fun++;
		if(cnt_fun==75)
		{
			cnt_fun = 0;
			MPU6050_CalOff_Acc();
		}
	}
	else if(rc_in->THROTTLE > RC_MIN && rc_in->THROTTLE<RC_FUN_MIN && rc_in->ROLL>RC_FUN_MAX && rc_in->ROLL < RC_MAX)
	{
		cnt_fun++;
		if(cnt_fun==75)
		{
			cnt_fun = 0;
			MPU6050_CalOff_Gyr();
		}
	}
	else
		cnt_fun = 0;
}
#else*/ 
/*****************************************************************
*Description:???????,??,??????,?????
*
*
*****************************************************************/
void Rc_Fun(T_RC_Data *rc_in,T_RC_Control *rc_ct)//?????????????????
{
	static u8 cnt_arm=0,cnt_fun=0;
	if(rc_in->THROTTLE<RC_FUN_MIN&&rc_in->YAW<=RC_FUN_MIN)
	{//rc_in->THROTTLE油门参数   rc_in->YAW 航向角  
	 //说明：当油门打到最低 航向角向左打到最低  解锁飞控
		cnt_arm++;
	    LED2_ON ;
		if(cnt_arm==75)
		{
			cnt_arm=0;
			rc_ct->ARMED = 1;
			 
		}
	}
	else if(rc_in->THROTTLE<RC_FUN_MIN&&rc_in->YAW>=RC_FUN_MAX)
	{//rc_in->THROTTLE油门参数   rc_in->YAW 航向角  
	 //说明：当油门打到最低 航向角向右打到最低  锁定飞控
		cnt_arm++;
	  LED3_ON;
		if(cnt_arm==75)
		{
			cnt_arm=0;
			rc_ct->ARMED = 0;
			
		}
	}
	else
	{
	  LED3_OFF ;
	  LED2_OFF ;
    cnt_arm = 0;
	}
		
		
	if(rc_ct->ARMED==1)//解锁飞控以后就不执行后面的程序，即不做补偿
		return;
	
	if(rc_in->THROTTLE<RC_FUN_MIN&&rc_in->ROLL<RC_FUN_MIN)
	{//rc_in->THROTTLE油门参数   rc_in->ROLL 横滚角  
	 //说明：当油门打到最低 横滚角减小  加速度计补偿
		cnt_fun++;
		
		if(cnt_fun==75)
		{  
			LED2_ON ;
			cnt_fun = 0;
			MPU6050_CalOff_Acc();//使能加速度计补偿
		}
	}
	else if(rc_in->THROTTLE<RC_FUN_MIN&&rc_in->ROLL>RC_FUN_MAX)
	{//rc_in->THROTTLE油门参数   rc_in->ROLL 横滚角  
	 //说明：当油门打到最低 横滚角增大 陀螺仪补偿
		cnt_fun++;
		
		if(cnt_fun==75)
		{
			LED3_ON;
			cnt_fun = 0;
			MPU6050_CalOff_Gyr();//使能陀螺仪
		}
	}
	else
	{
		cnt_fun = 0;
	}
		
}
//#endif
