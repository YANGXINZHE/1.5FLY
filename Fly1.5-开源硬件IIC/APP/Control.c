#include "control.h"

PID PID_ROL,PID_PIT,PID_YAW,PID_ALT,PID_POS;
PID PID_PID_1,PID_PID_2,PID_PID_3,PID_PID_4,PID_PID_5,PID_PID_6,PID_PID_7,PID_PID_8,PID_PID_9,PID_PID_10,PID_PID_11,PID_PID_12;

int16_t getlast_roll=0,geilast_pitch=0;
float rol_i=0,pit_i=0,yaw_p=0;

vs16 Moto_PWM_1=0,Moto_PWM_2=0,Moto_PWM_3=0,Moto_PWM_4=0,Moto_PWM_5=0,Moto_PWM_6=0,Moto_PWM_7=0,Moto_PWM_8=0;

#ifndef USE_USART1
void init_pid()
{
//	PID_ROL.P = 5.0f;
//	PID_ROL.I = 0.004;
//	PID_ROL.D = 0.08f;
//	
//	PID_PIT.P = 5.0f;
//	PID_PIT.I = 0.004;
//	PID_PIT.D = 0.08f;
	
//	PID_ROL.P = 4.0f;
//	PID_ROL.I = 0.0045f;
//	PID_ROL.D = 0.05f;
//	
//	PID_PIT.P = 4.0f;
//	PID_PIT.I = 0.0045f;
//	PID_PIT.D = 0.05f;
	
	PID_ROL.P = 5.4f;
	PID_ROL.I = 0.005f;
	PID_ROL.D = 0.068f;
	
	PID_PIT.P = 5.4f;
	PID_PIT.I = 0.005f;
	PID_PIT.D = 0.068f;

	PID_YAW.P = 8.0f;
	PID_YAW.I = 0.06f;
	PID_YAW.D = 0.09f;
}
#endif
void Control(T_float_angle *att_in,T_int16_xyz *gyr_in, T_RC_Data *rc_in, u8 armed)
{                          //加速度             角速度
	static u8 first =1;
	T_float_angle angle;	
	#ifndef USE_USART1
	if(first)
	{
	 init_pid();
		first=0;
	}
	#endif
	
	//**************roll轴指向***********************************************************
  angle.rol = att_in->rol - (rc_in->ROLL-1500)/25;//误差
	if(rc_in->THROTTLE<1200)
		rol_i=0;
	else
	  rol_i += angle.rol;//误差和
	LIMIT(-2000, rol_i,2000);//积分限幅
	PID_ROL.pout = PID_ROL.P * angle.rol;      //比例项 = 比例*误差
	PID_ROL.dout = -PID_ROL.D * gyr_in->Y;     //微分项 = 微分*变化量
	//PID_ROL.iout = PID_ROL.I * PID_ROL.dout; //积分项 = 积分*误差和
	PID_ROL.iout = PID_ROL.I * rol_i;

	PID_ROL.OUT =  (-PID_ROL.pout)-PID_ROL.iout +PID_ROL.dout;//
    //**************pith轴指向***********************************************************
	angle.pit = att_in->pit + (rc_in->PITCH-1500)/25;
	if(rc_in->THROTTLE<1200)
		pit_i=0;
	else
	  pit_i += angle.pit;//误差和
	LIMIT(-2000, pit_i,2000);
	PID_PIT.pout = PID_PIT.P * angle.pit;   //比例项 = 比例*误差
	PID_PIT.dout = PID_PIT.D * gyr_in->X;   //微分项 = 微分*变化量
	PID_PIT.iout = PID_PIT.I * pit_i;       //积分项 = 积分*误差和
	//PID_PIT.iout = PID_PIT.I *PID_PIT.dout ;

	PID_PIT.OUT = PID_PIT.pout + PID_PIT.iout + PID_PIT.dout;
	//**************yaw轴指向***********************************************************
	if(rc_in->YAW<1400||rc_in->YAW>1600)
	{gyr_in->Z=gyr_in->Z+(rc_in->YAW-1500)*2;}
	if(rc_in->THROTTLE<1200)
		yaw_p=0;
	else 
		yaw_p+=gyr_in->Z*0.0609756f*0.002f;// +(Rc_Get.YAW-1500)*30
	
	if(yaw_p>20)
		yaw_p=20;
	if(yaw_p<-20)
		yaw_p=-20;
	PID_YAW.pout = PID_YAW.P*yaw_p;
	PID_YAW.dout = PID_YAW.D * gyr_in->Z;		
	PID_YAW.iout = PID_YAW.I *PID_YAW.dout ;

	PID_YAW.OUT = PID_YAW.pout + PID_YAW.iout + PID_YAW.dout;

    //**************电机控制***********************************************************
	if(rc_in->THROTTLE>1200&&armed)//armed判断解锁
	{                                       //横滚            //俯仰        //航向
//		Moto_PWM_1 = rc_in->THROTTLE - 1000  - PID_ROL.OUT + PID_PIT.OUT - PID_YAW.OUT;
//		Moto_PWM_2 = rc_in->THROTTLE - 1000  - PID_ROL.OUT - PID_PIT.OUT + PID_YAW.OUT;
//		Moto_PWM_3 = rc_in->THROTTLE - 1000  + PID_ROL.OUT + PID_PIT.OUT - PID_YAW.OUT;
//		Moto_PWM_4 = rc_in->THROTTLE - 1000  + PID_ROL.OUT - PID_PIT.OUT + PID_YAW.OUT;
		Moto_PWM_1 = rc_in->THROTTLE - 1000 + PID_ROL.OUT + PID_PIT.OUT + PID_YAW.OUT;//反逻辑 电机下沉端用加号
		Moto_PWM_2 = rc_in->THROTTLE - 1000 + PID_ROL.OUT - PID_PIT.OUT - PID_YAW.OUT;
		Moto_PWM_3 = rc_in->THROTTLE - 1000 - PID_ROL.OUT - PID_PIT.OUT + PID_YAW.OUT;
		Moto_PWM_4 = rc_in->THROTTLE - 1000 - PID_ROL.OUT + PID_PIT.OUT - PID_YAW.OUT;
	}
	else
	{
		Moto_PWM_1 = 0;
		Moto_PWM_2 = 0;
		Moto_PWM_3 = 0;
		Moto_PWM_4 = 0;
	}
	//**************更新PWM***********************************************************
	Moto_PwmRflash(Moto_PWM_1,Moto_PWM_2,Moto_PWM_3,Moto_PWM_4);
}
