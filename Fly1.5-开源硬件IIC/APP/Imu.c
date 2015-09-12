#include "imu.h"
#include "math.h"
#include "control.h"

#define RtA 		57.324841f				
#define AtR    		0.0174533f				
#define Acc_G 		0.0011963f				
#define Gyro_G 		0.0610351f				
#define Gyro_Gr		0.0010653f			
#define FILTER_NUM 	20
#define SHIFT 4

float 	AngleOffset_Rol=0,AngleOffset_Pit=0;//角度补偿
/**
 * IIR filter the samples.
 */
int16_t iirLPFilterSingle(int32_t in, int32_t attenuation,  int32_t* filt)
{
  int32_t inScaled;
  int32_t filttmp = *filt;
  int16_t out;

  if (attenuation > (1<<IIR_SHIFT))
  {
    attenuation = (1<<IIR_SHIFT);
  }
  else if (attenuation < 1)
  {
    attenuation = 1;
  }

  // Shift to keep accuracy
  inScaled = in << IIR_SHIFT;
  // Calculate IIR filter
  filttmp = filttmp + (((inScaled-filttmp) >> IIR_SHIFT) * attenuation);
  // Scale and round
  out = (filttmp >> 8) + ((filttmp & (1 << (IIR_SHIFT - 1))) >> (IIR_SHIFT - 1));
  *filt = filttmp;

  return out;
}
static void imuAccIIRLPFilter(T_int16_xyz *in, T_int16_xyz *out, Axis3i32* storedValues, int32_t attenuation)
{
  out->X = iirLPFilterSingle(in->X, attenuation, &storedValues->x);
  out->Y = iirLPFilterSingle(in->Y, attenuation, &storedValues->y);
  out->Z = iirLPFilterSingle(in->Z, attenuation, &storedValues->z);
}
Axis3i32   accelStoredFilterValues;
void Prepare_Data(T_int16_xyz *acc_in,T_int16_xyz *acc_out)
{  
#if 1
	static uint8_t 	filter_cnt=0;
	static int16_t	ACC_X_BUF[FILTER_NUM],ACC_Y_BUF[FILTER_NUM],ACC_Z_BUF[FILTER_NUM];
	int32_t temp1=0,temp2=0,temp3=0;
	uint8_t i;	
	ACC_X_BUF[filter_cnt] = acc_in->X;
	ACC_Y_BUF[filter_cnt] = acc_in->Y;
	ACC_Z_BUF[filter_cnt] = acc_in->Z;
	for(i=0;i<FILTER_NUM;i++)//滑动平滑滤波
	{
		temp1 += ACC_X_BUF[i];
		temp2 += ACC_Y_BUF[i];
		temp3 += ACC_Z_BUF[i];
	}
	acc_out->X = temp1 / FILTER_NUM;
	acc_out->Y = temp2 / FILTER_NUM;
	acc_out->Z = temp3 / FILTER_NUM;
	filter_cnt++;
	if(filter_cnt==FILTER_NUM)	filter_cnt=0;
#else
	imuAccIIRLPFilter(acc_in,acc_out, &accelStoredFilterValues,
                    IMU_ACC_IIR_LPF_ATT_FACTOR);
#endif
}
////////////////////////////////////////////////////////////////////////////////
#define Kp 1.6f                        // proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.001f                          // integral gain governs rate of convergence of gyroscope biases
//#define Kp 2.0f                        // proportional gain governs rate of convergence to accelerometer/magnetometer
//#define Ki 0.002f                          // integral gain governs rate of convergence of gyroscope biases
#define halfT 0.001f                   // half the sample period???????

float q0 = 1, q1 = 0, q2 = 0, q3 = 0;    //初始化四元数 quaternion elements representing the estimated orientation
float exInt = 0, eyInt = 0, ezInt = 0;    //定义姿态解算误差的积分 scaled integral error
void IMUupdate(T_int16_xyz *gyr, T_int16_xyz *acc, T_float_angle *angle)//　由于加速度的噪声较大，此处应采用滤波后的数据
{
	float ax = acc->X,ay = acc->Y,az = acc->Z;//参数ax，ay，az分别对应三个轴的加速度原始数据
	float gx = gyr->X,gy = gyr->Y,gz = gyr->Z;//参数gx，gy，gz分别对应三个轴的角速度，单位是弧度/秒;
  float norm;
//  float hx, hy, hz, bx, bz;
  float vx, vy, vz;// wx, wy, wz;
  float ex, ey, ez;

  // ???????????
  float q0q0 = q0*q0;
  float q0q1 = q0*q1;
  float q0q2 = q0*q2;
//  float q0q3 = q0*q3;
  float q1q1 = q1*q1;
//  float q1q2 = q1*q2;
  float q1q3 = q1*q3;
  float q2q2 = q2*q2;
  float q2q3 = q2*q3;
  float q3q3 = q3*q3;
	
	if(ax*ay*az==0)
 		return;
		
	gx *= Gyro_Gr;
	gy *= Gyro_Gr;
	gz *= Gyro_Gr;
	//将加速度的原始数据，归一化，得到单位加速度	
  norm = sqrt(ax*ax + ay*ay + az*az);       //acc?????
  ax = ax /norm;
  ay = ay / norm;
  az = az / norm;
  /*把四元数换算成“方向余弦矩阵”中的第三列的三个元素。
  根据余弦矩阵和欧拉角的定义，地理坐标系的重力向量，转到机体坐标系，
  正好是这三个元素。所以这里的vx、vy、vz，其实就是当前的机体坐标参照系上，
  换算出来的重力单位向量。(用表示机体姿态的四元数进行换算)
  */
	if(norm>18000)
	{
	 //Rc_C.ARMED=0;
	}
  // estimated direction of gravity and flux (v and w)              ?????????/??
  vx = 2*(q1q3 - q0q2);												//????xyz???
  vy = 2*(q0q1 + q2q3);
  vz = q0q0 - q1q1 - q2q2 + q3q3 ;

  // error is sum of cross product between reference direction of fields and direction measured by sensors
  ex = (ay*vz - az*vy) ;  //叉乘                         					 //???????????????
  ey = (az*vx - ax*vz) ;
  ez = (ax*vy - ay*vx) ;
//将叉乘误差进行积分
  exInt = exInt + ex * Ki;								  //???????
  eyInt = eyInt + ey * Ki;
  ezInt = ezInt + ez * Ki;

  // adjusted gyroscope measurements
  /*/用叉乘误差来做PI修正陀螺零偏，通过调节Kp，Ki两个参数，
  可以控制加速度计修正陀螺仪积分姿态的速度*/
  gx = gx + Kp*ex + exInt;					   							//???PI???????,???????
  gy = gy + Kp*ey + eyInt;
  gz = gz + Kp*ez + ezInt;				   							//???gz????????????????,??????????????

  // integrate quaternion rate and normalise						   //????????
  q0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
  q1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
  q2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
  q3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;

  // normalise quaternion
  norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	
  q0 = q0 / norm;
  q1 = q1 / norm;
  q2 = q2 / norm;
  q3 = q3 / norm;

	angle->yaw += gyr->Z*Gyro_G*0.002f;
	
	angle->rol = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3 - AngleOffset_Pit; // pitch  
	angle->pit = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3 - AngleOffset_Rol; // roll
}

