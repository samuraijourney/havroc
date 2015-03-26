#ifndef _MPU9250_DRIVER_H
#define _MPU9250_DRIVER_H

#include "havroc/tracking/IMU_Math.h"

//  this sets the learning rate for compass running average calculation

#define COMPASS_ALPHA                   (float)0.2

//  this defines the accelerometer noise level

#define FUZZY_GYRO_ZERO           (float)0.20

#define FUZZY_GYRO_ZERO_SQUARED   (FUZZY_GYRO_ZERO * FUZZY_GYRO_ZERO)

//  this defines the accelerometer noise level

#define FUZZY_ACCEL_ZERO          (float)0.05

#define FUZZY_ACCEL_ZERO_SQUARED   (FUZZY_ACCEL_ZERO * FUZZY_ACCEL_ZERO)

//  MPU9250 I2C Slave Addresses

#define MPU9250_ADDRESS0            0x68
#define MPU9250_ID                  0x71

#define AK8963_ADDRESS              0x0c

//  Register map

#define MPU9250_SMPRT_DIV           0x19
#define MPU9250_GYRO_LPF            0x1a
#define MPU9250_GYRO_CONFIG         0x1b
#define MPU9250_ACCEL_CONFIG        0x1c
#define MPU9250_ACCEL_LPF           0x1d
#define MPU9250_FIFO_EN             0x23
#define MPU9250_I2C_MST_CTRL        0x24
#define MPU9250_I2C_SLV0_ADDR       0x25
#define MPU9250_I2C_SLV0_REG        0x26
#define MPU9250_I2C_SLV0_CTRL       0x27
#define MPU9250_I2C_SLV1_ADDR       0x28
#define MPU9250_I2C_SLV1_REG        0x29
#define MPU9250_I2C_SLV1_CTRL       0x2a
#define MPU9250_I2C_SLV2_ADDR       0x2b
#define MPU9250_I2C_SLV2_REG        0x2c
#define MPU9250_I2C_SLV2_CTRL       0x2d
#define MPU9250_I2C_SLV4_CTRL       0x34
#define MPU9250_INT_PIN_CFG         0x37
#define MPU9250_INT_ENABLE          0x38
#define MPU9250_INT_STATUS          0x3a
#define MPU9250_ACCEL_XOUT_H        0x3b
#define MPU9250_GYRO_XOUT_H         0x43
#define MPU9250_EXT_SENS_DATA_00    0x49
#define MPU9250_I2C_SLV1_DO         0x64
#define MPU9250_I2C_MST_DELAY_CTRL  0x67
#define MPU9250_USER_CTRL           0x6a
#define MPU9250_PWR_MGMT_1          0x6b
#define MPU9250_PWR_MGMT_2          0x6c
#define MPU9250_FIFO_COUNT_H        0x72
#define MPU9250_FIFO_R_W            0x74
#define MPU9250_WHO_AM_I            0x75

//  sample rate defines (applies to gyros and accels, not mags)

#define MPU9250_SAMPLERATE_MIN      5                       // 5 samples per second is the lowest
#define MPU9250_SAMPLERATE_MAX      1000                    // 1000 samples per second is the absolute maximum

//  compass rate defines

#define MPU9250_COMPASSRATE_MIN     1                       // 1 samples per second is the lowest
#define MPU9250_COMPASSRATE_MAX     100                     // 100 samples per second is maximum

//  Gyro LPF options

#define MPU9250_GYRO_LPF_8800       0x11                    // 8800Hz, 0.64mS delay
#define MPU9250_GYRO_LPF_3600       0x10                    // 3600Hz, 0.11mS delay
#define MPU9250_GYRO_LPF_250        0x00                    // 250Hz, 0.97mS delay
#define MPU9250_GYRO_LPF_184        0x01                    // 184Hz, 2.9mS delay
#define MPU9250_GYRO_LPF_92         0x02                    // 92Hz, 3.9mS delay
#define MPU9250_GYRO_LPF_41         0x03                    // 41Hz, 5.9mS delay
#define MPU9250_GYRO_LPF_20         0x04                    // 20Hz, 9.9mS delay
#define MPU9250_GYRO_LPF_10         0x05                    // 10Hz, 17.85mS delay
#define MPU9250_GYRO_LPF_5          0x06                    // 5Hz, 33.48mS delay

//  Gyro FSR options

#define MPU9250_GYROFSR_250         0                       // +/- 250 degrees per second
#define MPU9250_GYROFSR_500         8                       // +/- 500 degrees per second
#define MPU9250_GYROFSR_1000        0x10                    // +/- 1000 degrees per second
#define MPU9250_GYROFSR_2000        0x18                    // +/- 2000 degrees per second

//  Accel FSR options

#define MPU9250_ACCELFSR_2          0                       // +/- 2g
#define MPU9250_ACCELFSR_4          8                       // +/- 4g
#define MPU9250_ACCELFSR_8          0x10                    // +/- 8g
#define MPU9250_ACCELFSR_16         0x18                    // +/- 16g

//  Accel LPF options

#define MPU9250_ACCEL_LPF_1130      0x08                    // 1130Hz, 0.75mS delay
#define MPU9250_ACCEL_LPF_460       0x00                    // 460Hz, 1.94mS delay
#define MPU9250_ACCEL_LPF_184       0x01                    // 184Hz, 5.80mS delay
#define MPU9250_ACCEL_LPF_92        0x02                    // 92Hz, 7.80mS delay
#define MPU9250_ACCEL_LPF_41        0x03                    // 41Hz, 11.80mS delay
#define MPU9250_ACCEL_LPF_20        0x04                    // 20Hz, 19.80mS delay
#define MPU9250_ACCEL_LPF_10        0x05                    // 10Hz, 35.70mS delay
#define MPU9250_ACCEL_LPF_5         0x06                    // 5Hz, 66.96mS delay

//  AK8963 compass registers

#define AK8963_DEVICEID             0x48                    // the device ID
#define AK8963_ST1                  0x02                    // status 1
#define AK8963_CNTL                 0x0a                    // control reg
#define AK8963_ASAX                 0x10                    // start of the fuse ROM data

//  FIFO transfer size

#define MPU9250_FIFO_CHUNK_SIZE     12                      // gyro and accels take 12 bytes

typedef struct _IMU
{
	bool m_firstTime;                                       // if first sample
    bool m_calibrationMode;                                 // true if cal mode so don't use cal data!
    bool m_calibrationValid;                                // tru if call data is valid and can be used

    unsigned char m_slaveAddr;                              // I2C address of MPU9250
    unsigned char m_bus;                                    // I2C bus (usually 1 for Raspberry Pi for example)

    unsigned char m_gyroLpf;                                // gyro low pass filter setting
    unsigned char m_accelLpf;                               // accel low pass filter setting
    int m_compassRate;                                      // compass sample rate in Hz
    int m_sampleRate;                                       // samples per second
    uint64_t m_sampleInterval;                              // interval between samples in microseonds
    unsigned char m_gyroFsr;
    unsigned char m_accelFsr;

    float m_gyroScale;
    float m_accelScale;
    float m_compassAdjust[3];                             // the compass fuse ROM values converted for use
    float m_gyroAlpha;                                    // gyro bias learning rate

    Vector3 m_gyro;                                       // the gyro readings in radians/sec
    Vector3 m_accel;                                      // the accel readings in gs
    Vector3 m_compass;                                    // the compass readings in uT
    unsigned long m_timestamp;                              // the timestamp

    int m_gyroSampleCount;                                  // number of gyro samples used
    bool m_gyroBiasValid;                                   // true if the recorded gyro bias is valid
    Vector3 m_gyroBias;                                   // the recorded gyro bias

    Vector3 m_previousAccel;                              // previous step accel for gyro learning

    float m_compassCalOffset[3];
    float m_compassCalScale[3];
    Vector3 m_compassAverage;                             // a running average to smooth the mag outputs

    float m_axisRotation[9];						        // axis rotation matrix

    //  IMU-specific vars
    //  MPU9250

    int m_MPU9250GyroAccelSampleRate;                       // the sample rate (samples per second) for gyro and accel
    int m_MPU9250CompassSampleRate;                         // same for the compass
    int m_MPU9250GyroLpf;                                   // low pass filter code for the gyro
    int m_MPU9250AccelLpf;                                  // low pass filter code for the accel
    int m_MPU9250GyroFsr;                                   // FSR code for the gyro
    int m_MPU9250AccelFsr;                                  // FSR code for the accel

    int m_imu_index;
}IMU;

#ifdef __cplusplus
extern "C"
{
#endif

void NewIMU(IMU* imu_object, int imu_index);

bool setGyroConfig(IMU* imu_object);
bool setAccelConfig(IMU* imu_object);
bool setSampleRate(IMU* imu_object);
bool compassSetup(IMU* imu_object);
bool setCompassRate(IMU* imu_object);
bool resetFifo(IMU* imu_object);
bool bypassOn(IMU* imu_object);
bool bypassOff(IMU* imu_object);
void handleGyroBias(IMU* imu_object);                                  		// adjust gyro for bias
void gyroBiasInit(IMU* imu_object);                                    		// sets up gyro bias calculation
void calibrateAverageCompass(IMU* imu_object);                         		// calibrate and smooth compass

bool setGyroLpf(IMU* imu_object, unsigned char lpf);
bool setAccelLpf(IMU* imu_object, unsigned char lpf);
bool setSampleVar(IMU* imu_object, int rate);
bool setCompassVar(IMU* imu_object, int rate);
bool setGyroFsr(IMU* imu_object, unsigned char fsr);
bool setAccelFsr(IMU* imu_object, unsigned char fsr);

uint8_t IMUInit(IMU* imu_object);
bool IMURead(IMU* imu_object);
int IMUGetPollInterval(IMU* imu_object);

//  setCalibrationData configured the cal data and also enables use if valid

void setCalibrationData(IMU* imu_object);

//  getCalibrationValid() returns true if the calibration data is being used

bool IMUGyroBiasValid(IMU* imu_object);

#ifdef __cplusplus
}
#endif

#endif //_MPU9250_DRIVER_H
