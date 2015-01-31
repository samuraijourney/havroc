#ifndef H_IMU_DRIVER
#define H_IMU_DRIVER

#include <stdint.h>
#include <utils.h>

//Device Addresses
#define MPU_ADDRESS      0x68
#define AK8963_ADDRESS   0x0C
#define WHO_AM_I_AK8963  0x00
#define WHO_AM_I_MPU9250 0x75
#define AKM_WHOAMI       0x48
#define MPU_WHOAMI       0x71
#define AK8963_XOUT_L    0x03
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define INT_PIN_CFG      0x37
#define AK8963_CNTL      0x0A
#define AK8963_ST1       0x02
#define PWR_MGMT_1       0x6B
#define CONFIG           0x1A
#define SMPLRT_DIV       0x19
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG2    0x1D
#define AK8963_ASAX      0x10
#define AK8963_ASAY      0x11
#define AK8963_ASAZ      0x12
#define AK8963_ST2       0x09
#define INT_ENABLE       0x38
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48 

//Device modes
#define GFS_250DPS       0
#define AFS_2G           0
#define delay(ms) UtilsDelay((80000/5)*ms)



#ifdef __cplusplus 
extern "C"
{ 
#endif
	
	int initMPU(uint32_t imu_index);
	int initCompass(uint32_t imu_index);
	int readMPUData(float * accel_X, float * accel_Y, float * accel_Z, float * gyro_X, float * gyro_Y, float * gyro_Z, uint32_t imu_index);
	int readCompassData(float * mag_X, float * mag_Y, float * mag_Z, uint32_t imu_index);
	void getGyroOffsets(float * gyro_OffsetX, float * gyro_OffsetY, float * gyro_OffsetZ, int imu_index);

#ifdef __cplusplus 
} 
#endif

#endif
