#ifndef _IMU_H_
#define _IMU_H_
#include "stm32f10x.h"
#include "sysconfig.h"
#include <stdint.h>

extern float 	AngleOffset_Rol,AngleOffset_Pit; 

typedef struct
{
  int16_t x;
  int16_t y;
  int16_t z;
} Axis3i16;

typedef struct
{
  int32_t x;
  int32_t y;
  int32_t z;
} Axis3i32;

typedef struct
{
  float x;
  float y;
  float z;
} Axis3f;

//Axis3i16   accelMpu;
//Axis3i16   accelLPF;
extern Axis3i32   accelStoredFilterValues;
#define IIR_SHIFT         8
int16_t iirLPFilterSingle(int32_t in, int32_t attenuation,  int32_t* filt);
/**
 * IMU update frequency dictates the overall update frequency.
 */
#define IMU_UPDATE_FREQ   500
//#define IMU_UPDATE_DT     (float)(1.0/IMU_UPDATE_FREQ)

/**
 * Set ACC_WANTED_LPF1_CUTOFF_HZ to the wanted cut-off freq in Hz.
 * The highest cut-off freq that will have any affect is fs /(2*pi).
 * E.g. fs = 350 Hz -> highest cut-off = 350/(2*pi) = 55.7 Hz -> 55 Hz
*设置ACC_WANTED_LPF1_CUTOFF_HZ希望截止频率的赫兹。
*最高的截止频率,将取决于fs /(2 * pi)。
*如fs = 350赫兹- >最高截止= 350 /(2 * pi)= 55.7 Hz - > 55 Hz
 */
#define IMU_ACC_WANTED_LPF_CUTOFF_HZ  10
/**
 * Attenuation should be between 1 to 256.
 *
 * f0 = fs / 2*pi*attenuation ->
 * attenuation = fs / 2*pi*f0
 */
#define IMU_ACC_IIR_LPF_ATTENUATION (IMU_UPDATE_FREQ / (2 * 3.1415 * IMU_ACC_WANTED_LPF_CUTOFF_HZ))
#define IMU_ACC_IIR_LPF_ATT_FACTOR  (int)(((1<<IIR_SHIFT) / IMU_ACC_IIR_LPF_ATTENUATION) + 0.5)

void Prepare_Data(T_int16_xyz *acc_in,T_int16_xyz *acc_out);

void IMUupdate(T_int16_xyz *gyr, T_int16_xyz *acc, T_float_angle *angle);


#endif
