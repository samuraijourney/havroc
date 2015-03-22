
// common interface includes
#include "common.h"

/* HaVRoc Library Includes */
#include "havroc/tracking/MPU9250_Driver.h"
#include "havroc/tracking/Calibration.h"
#include <havroc/communications/suit/suit_net_manager.h>
#include <havroc/error.h>
#include <havroc/havroc_utils/havrocutils.h>


void NewIMU(IMU* imu_object, int imu_index)
{
	int i;
    imu_object->m_imu_index = imu_index;

    //  MPU9250 defaults

    imu_object->m_MPU9250GyroAccelSampleRate = 40;
    imu_object->m_MPU9250CompassSampleRate = 40;
    imu_object->m_MPU9250GyroLpf = MPU9250_GYRO_LPF_41;
    imu_object->m_MPU9250AccelLpf = MPU9250_ACCEL_LPF_41;
    imu_object->m_MPU9250GyroFsr = MPU9250_GYROFSR_1000;
    imu_object->m_MPU9250AccelFsr = MPU9250_ACCELFSR_8;
    imu_object->m_calibrationMode = false;
    imu_object->m_calibrationValid = false;
    imu_object->m_gyroBiasValid = false;

    for(i = 0; i < 9; i++)
    {
    	imu_object->m_axisRotation[i] = 0;
    }

    imu_object->m_axisRotation[0] = 1;
    imu_object->m_axisRotation[4] = 1;
    imu_object->m_axisRotation[8] = 1;
}

void setCalibrationData(IMU* imu_object)
{
	int i;
    imu_object->m_calibrationValid = false;

    if (calib_valid[imu_object->m_imu_index]) {
          
       // this is the max +/- range

        for (i = 0; i < 3; i++) {
            imu_object->m_compassCalScale[i] = compassCalScale[imu_object->m_imu_index][i];            // makes everything the same range
            imu_object->m_compassCalOffset[i] = compassCalOffset[imu_object->m_imu_index][i];
        }
        imu_object->m_calibrationValid = true;
    }
    else
    {
        return;
    }
}

void gyroBiasInit(IMU* imu_object)
{
    imu_object->m_gyroAlpha = 2.0f / imu_object->m_sampleRate;
    imu_object->m_gyroSampleCount = 0;
}


void handleGyroBias(IMU* imu_object)
{  
    Vector3 deltaAccel = imu_object->m_previousAccel;
    deltaAccel  = subtractVector3 (deltaAccel, imu_object->m_accel);   // compute difference
    imu_object->m_previousAccel = imu_object->m_accel;

    if ((squareLength(deltaAccel) < RTIMU_FUZZY_ACCEL_ZERO_SQUARED) && 
                (squareLength(imu_object->m_gyro) < RTIMU_FUZZY_GYRO_ZERO_SQUARED)) {
        // what we are seeing on the gyros should be bias only so learn from this
        imu_object->m_gyroBias.m_data[0] = ((1.0 - imu_object->m_gyroAlpha) * imu_object->m_gyroBias.m_data[0] + imu_object->m_gyroAlpha * imu_object->m_gyro.m_data[0]);
        imu_object->m_gyroBias.m_data[1] = ((1.0 - imu_object->m_gyroAlpha) * imu_object->m_gyroBias.m_data[1] + imu_object->m_gyroAlpha * imu_object->m_gyro.m_data[1]);
        imu_object->m_gyroBias.m_data[2] = ((1.0 - imu_object->m_gyroAlpha) * imu_object->m_gyroBias.m_data[2] + imu_object->m_gyroAlpha * imu_object->m_gyro.m_data[2]);

        if (imu_object->m_gyroSampleCount < (5 * imu_object->m_sampleRate)) {
            imu_object->m_gyroSampleCount++;

            if (imu_object->m_gyroSampleCount == (5 * imu_object->m_sampleRate)) {
                imu_object->m_gyroBiasValid = true;
            }
        }
    }

    imu_object->m_gyro  = subtractVector3 (imu_object->m_gyro, imu_object->m_gyroBias);
}

void calibrateAverageCompass(IMU* imu_object)
{
    //  calibrate if required

    if (!imu_object->m_calibrationMode && imu_object->m_calibrationValid) {
        imu_object->m_compass.m_data[0] = ((imu_object->m_compass.m_data[0] - imu_object->m_compassCalOffset[0]) * imu_object->m_compassCalScale[0]);
        imu_object->m_compass.m_data[1] = ((imu_object->m_compass.m_data[1] - imu_object->m_compassCalOffset[1]) * imu_object->m_compassCalScale[1]);
        imu_object->m_compass.m_data[2] = ((imu_object->m_compass.m_data[2] - imu_object->m_compassCalOffset[2]) * imu_object->m_compassCalScale[2]);
    }

    //  update running average

    imu_object->m_compassAverage.m_data[0] = (imu_object->m_compass.m_data[0] * COMPASS_ALPHA + imu_object->m_compassAverage.m_data[0] * (1.0 - COMPASS_ALPHA));
    imu_object->m_compassAverage.m_data[1] = (imu_object->m_compass.m_data[1] * COMPASS_ALPHA + imu_object->m_compassAverage.m_data[1] * (1.0 - COMPASS_ALPHA));
    imu_object->m_compassAverage.m_data[2] = (imu_object->m_compass.m_data[2] * COMPASS_ALPHA + imu_object->m_compassAverage.m_data[2] * (1.0 - COMPASS_ALPHA));

    imu_object->m_compass = imu_object->m_compassAverage;
}

bool IMUGyroBiasValid(IMU* imu_object)
{
    return imu_object->m_gyroBiasValid;
}


bool setSampleVar(IMU* imu_object, int rate)
{
    if ((rate < MPU9250_SAMPLERATE_MIN) || (rate > MPU9250_SAMPLERATE_MAX)) {
        return false;
    }
    imu_object->m_sampleRate = rate;
    imu_object->m_sampleInterval = (unsigned long)1000 / imu_object->m_sampleRate;
    if (imu_object->m_sampleInterval == 0)
        imu_object->m_sampleInterval = 1;
    return true;
}

bool setGyroLpf(IMU* imu_object, unsigned char lpf)
{
    switch (lpf) {
    case MPU9250_GYRO_LPF_8800:
    case MPU9250_GYRO_LPF_3600:
    case MPU9250_GYRO_LPF_250:
    case MPU9250_GYRO_LPF_184:
    case MPU9250_GYRO_LPF_92:
    case MPU9250_GYRO_LPF_41:
    case MPU9250_GYRO_LPF_20:
    case MPU9250_GYRO_LPF_10:
    case MPU9250_GYRO_LPF_5:
        imu_object->m_gyroLpf = lpf;
        return true;

    default:
        return false;
    }
}

bool setAccelLpf(IMU* imu_object, unsigned char lpf)
{
    switch (lpf) {
    case MPU9250_ACCEL_LPF_1130:
    case MPU9250_ACCEL_LPF_460:
    case MPU9250_ACCEL_LPF_184:
    case MPU9250_ACCEL_LPF_92:
    case MPU9250_ACCEL_LPF_41:
    case MPU9250_ACCEL_LPF_20:
    case MPU9250_ACCEL_LPF_10:
    case MPU9250_ACCEL_LPF_5:
        imu_object->m_accelLpf = lpf;
        return true;

    default:
        return false;
    }
}

bool setCompassVar(IMU* imu_object, int rate)
{
    if ((rate < MPU9250_COMPASSRATE_MIN) || (rate > MPU9250_COMPASSRATE_MAX)) {
        return false;
    }
    imu_object->m_compassRate = rate;
    return true;
}

bool setGyroFsr(IMU* imu_object, unsigned char fsr)
{
    switch (fsr) {
    case MPU9250_GYROFSR_250:
        imu_object->m_gyroFsr = fsr;
        imu_object->m_gyroScale = M_PI / (131.0 * 180.0);
        return true;

    case MPU9250_GYROFSR_500:
        imu_object->m_gyroFsr = fsr;
        imu_object->m_gyroScale = M_PI / (62.5 * 180.0);
        return true;

    case MPU9250_GYROFSR_1000:
        imu_object->m_gyroFsr = fsr;
        imu_object->m_gyroScale = M_PI / (32.8 * 180.0);
        return true;

    case MPU9250_GYROFSR_2000:
        imu_object->m_gyroFsr = fsr;
        imu_object->m_gyroScale = M_PI / (16.4 * 180.0);
        return true;

    default:
        return false;
    }
}

bool setAccelFsr(IMU* imu_object, unsigned char fsr)
{
    switch (fsr) {
    case MPU9250_ACCELFSR_2:
        imu_object->m_accelFsr = fsr;
        imu_object->m_accelScale = 1.0/16384.0;
        return true;

    case MPU9250_ACCELFSR_4:
        imu_object->m_accelFsr = fsr;
        imu_object->m_accelScale = 1.0/8192.0;
        return true;

    case MPU9250_ACCELFSR_8:
        imu_object->m_accelFsr = fsr;
        imu_object->m_accelScale = 1.0/4096.0;
        return true;

    case MPU9250_ACCELFSR_16:
        imu_object->m_accelFsr = fsr;
        imu_object->m_accelScale = 1.0/2048.0;
        return true;

    default:
        return false;
    }
}


uint8_t IMUInit(IMU* imu_object)
{
    uint8_t txBuff[1];
    uint8_t rxBuff[1];
    uint8_t retVal = SUCCESS;

    imu_object->m_firstTime = true;
    //  configure IMU

    setSampleVar(imu_object, imu_object->m_MPU9250GyroAccelSampleRate);
    setCompassVar(imu_object, imu_object->m_MPU9250CompassSampleRate);
    setGyroLpf(imu_object, imu_object->m_MPU9250GyroLpf);
    setAccelLpf(imu_object, imu_object->m_MPU9250AccelLpf);
    setGyroFsr(imu_object, imu_object->m_MPU9250GyroFsr);
    setAccelFsr(imu_object, imu_object->m_MPU9250AccelFsr);

    setCalibrationData(imu_object);

    //  reset the MPU9250
    txBuff[0] = 0x80;

    retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_PWR_MGMT_1, txBuff, 1);

    delay(100);

    txBuff[0] = 0x00;
    retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_PWR_MGMT_1, txBuff, 1);
    delay(100);
    retVal += suitNetManager_imu_i2c_read(imu_object->m_imu_index, false, MPU9250_WHO_AM_I, rxBuff, 1);
    delay(100);
    if (rxBuff[0] != MPU9250_ID) {
         return IMU_MPU_START_FAIL;
    }

    //  now configure the various components

    if (!setGyroConfig(imu_object))
        return IMU_MPU_START_FAIL;

    if (!setAccelConfig(imu_object))
        return IMU_MPU_START_FAIL;

    if (!setSampleRate(imu_object))
        return IMU_MPU_START_FAIL;

    //  now configure compass

    if (!bypassOn(imu_object))
        return IMU_MPU_START_FAIL;

    // get fuse ROM data

    txBuff[0] = 0x00;
    retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, true, AK8963_CNTL, txBuff, 1);

	txBuff[0] = 0x0f;
	retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, true, AK8963_CNTL, txBuff, 1);
	
	retVal += suitNetManager_imu_i2c_read(imu_object->m_imu_index, true, AK8963_ASAX, rxBuff, 3);
	
    //  convert asa to usable scale factor

    imu_object->m_compassAdjust[0] = ((float)rxBuff[0] - 128.0) / 256.0 + 1.0f;
    imu_object->m_compassAdjust[1] = ((float)rxBuff[1] - 128.0) / 256.0 + 1.0f;
    imu_object->m_compassAdjust[2] = ((float)rxBuff[2] - 128.0) / 256.0 + 1.0f;

    txBuff[0] = 0x00;
    retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, true, AK8963_CNTL, txBuff, 1);
    
    if (!bypassOff(imu_object))
        return IMU_MPU_START_FAIL;

    //  now set up MPU9250 to talk to the compass chip

    txBuff[0] = 0x40;
    retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_I2C_MST_CTRL, txBuff, 1);
  
    txBuff[0] = 0x80 | AK8963_ADDRESS;
    retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_I2C_SLV0_ADDR, txBuff, 1);

    txBuff[0] = AK8963_ST1;
    retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_I2C_SLV0_REG, txBuff, 1);

	txBuff[0] = 0x88;
	retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_I2C_SLV0_CTRL, txBuff, 1);

	txBuff[0] = AK8963_ADDRESS;
	retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_I2C_SLV1_ADDR, txBuff, 1);

	txBuff[0] = AK8963_CNTL;
	retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_I2C_SLV1_REG, txBuff, 1);

	txBuff[0] = 0x81;
	retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_I2C_SLV1_CTRL, txBuff, 1);

	txBuff[0] = 0x1;
	retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_I2C_SLV1_DO, txBuff, 1);

	txBuff[0] = 0x3;
	retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_I2C_MST_DELAY_CTRL, txBuff, 1);

    if (!setCompassRate(imu_object))
        return IMU_COMPASS_START_FAIL;

    //  enable the sensors
    txBuff[0] = 0x1;
    retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_PWR_MGMT_1, txBuff, 1);

    txBuff[0] = 0x0;
    retVal += suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_PWR_MGMT_2, txBuff, 1);

    //  select the data to go into the FIFO and enable

    if (!resetFifo(imu_object))
        return IMU_MPU_START_FAIL;

    gyroBiasInit(imu_object);

    if(retVal > 0)
    {
    	return IMU_MPU_START_FAIL;
    }

    return SUCCESS;
}

bool resetFifo(IMU* imu_object)
{
	uint8_t txBuff[1];

	txBuff[0] = 0x0;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_INT_ENABLE, txBuff, 1);

	txBuff[0] = 0x0;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_FIFO_EN, txBuff, 1);

	txBuff[0] = 0x0;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_USER_CTRL, txBuff, 1);

	txBuff[0] = 0x04;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_USER_CTRL, txBuff, 1);

	txBuff[0] = 0x60;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_USER_CTRL, txBuff, 1);

    delay(50);

    txBuff[0] = 1;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_INT_ENABLE, txBuff, 1);

    txBuff[0] = 0x78;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_FIFO_EN, txBuff, 1);

    return true;
}

bool bypassOn(IMU* imu_object)
{
	uint8_t txBuff[1];
    unsigned char userControl;

    suitNetManager_imu_i2c_read(imu_object->m_imu_index, false, MPU9250_USER_CTRL, &userControl, 1);

    userControl &= ~0x20;
    userControl |= 2;

    txBuff[0] = userControl;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_USER_CTRL, txBuff, 1);

    delay(50);

    txBuff[0] = 0x82;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_INT_PIN_CFG, txBuff, 1);

    delay(50);
    return true;
}


bool bypassOff(IMU* imu_object)
{
	uint8_t txBuff[1];
    unsigned char userControl;

    suitNetManager_imu_i2c_read(imu_object->m_imu_index, false, MPU9250_USER_CTRL, &userControl, 1);

    userControl |= 0x20;

    txBuff[0] = userControl;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_USER_CTRL, txBuff, 1);

    delay(50);

    txBuff[0] = 0x80;
    suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_INT_PIN_CFG, txBuff, 1);

    delay(50);
    return true;
}

bool setGyroConfig(IMU* imu_object)
{
	uint8_t txBuff[1];
    unsigned char gyroConfig = imu_object->m_gyroFsr + ((imu_object->m_gyroLpf >> 3) & 3);
    unsigned char gyroLpf = imu_object->m_gyroLpf & 7;

    txBuff[0] = gyroConfig;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_GYRO_CONFIG, txBuff, 1);

	txBuff[0] = gyroLpf;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_GYRO_LPF, txBuff, 1);
	
    return true;
}

bool setAccelConfig(IMU* imu_object)
{
	uint8_t txBuff[1];

	txBuff[0] = imu_object->m_accelFsr;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_ACCEL_CONFIG, txBuff, 1);

	txBuff[0] = imu_object->m_accelLpf;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_ACCEL_LPF, txBuff, 1);
	
    return true;
}

bool setSampleRate(IMU* imu_object)
{
	uint8_t txBuff[1];

    if (imu_object->m_sampleRate > 1000)
        return true;                                        // SMPRT not used above 1000Hz

    txBuff[0] = (unsigned char) (1000 / imu_object->m_sampleRate - 1);
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_SMPRT_DIV, txBuff, 1);

    return true;
}


bool setCompassRate(IMU* imu_object)
{
    int rate;
    uint8_t txBuff[1];

    rate = imu_object->m_sampleRate / imu_object->m_compassRate - 1;

    if (rate > 31)
        rate = 31;

    txBuff[0] = rate;
	suitNetManager_imu_i2c_write(imu_object->m_imu_index, false, MPU9250_I2C_SLV4_CTRL, txBuff, 1);

    return true;
}

int IMUGetPollInterval(IMU* imu_object)
{
    return (400 / imu_object->m_sampleRate);
}

bool IMURead(IMU* imu_object)
{
    unsigned char fifoCount[2];
    unsigned int count;
    unsigned char fifoData[12];
    unsigned char compassData[8];

    suitNetManager_imu_i2c_read(imu_object->m_imu_index, false, MPU9250_FIFO_COUNT_H, fifoCount, 2);

    count = ((unsigned int)fifoCount[0] << 8) + fifoCount[1];

    if (count == 1024) {
        resetFifo(imu_object);
        imu_object->m_timestamp += imu_object->m_sampleInterval * (1024 / MPU9250_FIFO_CHUNK_SIZE + 1); // try to fix timestamp
        Report("FIFO Overflow %i \n\r", count);
        return false;
    }

    if (count > MPU9250_FIFO_CHUNK_SIZE * 40) {
        // more than 40 samples behind - going too slowly so discard some samples but maintain timestamp correctly
        while (count >= MPU9250_FIFO_CHUNK_SIZE * 10) {
            suitNetManager_imu_i2c_read(imu_object->m_imu_index, false, MPU9250_FIFO_R_W, fifoData, MPU9250_FIFO_CHUNK_SIZE);

            count -= MPU9250_FIFO_CHUNK_SIZE;
            imu_object->m_timestamp += imu_object->m_sampleInterval;
        }
    }

    if (count < MPU9250_FIFO_CHUNK_SIZE)
    {
       // Report("Not enough FIFO contents %i \n\r", count);
        return false;
    }
    suitNetManager_imu_i2c_read(imu_object->m_imu_index, false, MPU9250_FIFO_R_W, fifoData, MPU9250_FIFO_CHUNK_SIZE);

    suitNetManager_imu_i2c_read(imu_object->m_imu_index, false, MPU9250_EXT_SENS_DATA_00, compassData, 8);

    convertToVector(fifoData, &(imu_object->m_accel), imu_object->m_accelScale, true);
    convertToVector(fifoData + 6, &(imu_object->m_gyro), imu_object->m_gyroScale, true);
    convertToVector(compassData + 1, &(imu_object->m_compass), 0.6f, false);

    //  sort out gyro axes

    imu_object->m_gyro.m_data[1] = (-imu_object->m_gyro.m_data[1]);
    imu_object->m_gyro.m_data[2] = (-imu_object->m_gyro.m_data[2]);

    //  sort out accel data;

    imu_object->m_accel.m_data[0] = (-imu_object->m_accel.m_data[0]);

    //  use the fuse data adjustments for compass

    imu_object->m_compass.m_data[0] = (imu_object->m_compass.m_data[0] * imu_object->m_compassAdjust[0]);
    imu_object->m_compass.m_data[1] = (imu_object->m_compass.m_data[1] * imu_object->m_compassAdjust[1]);
    imu_object->m_compass.m_data[2] = (imu_object->m_compass.m_data[2] * imu_object->m_compassAdjust[2]);

    //  sort out compass axes

    float temp;

    temp = imu_object->m_compass.m_data[0];
    imu_object->m_compass.m_data[0] = (imu_object->m_compass.m_data[1]);
    imu_object->m_compass.m_data[1] = (-temp);

    //  now do standard processing

    handleGyroBias(imu_object);
    calibrateAverageCompass(imu_object);

    if (imu_object->m_firstTime)
        imu_object->m_timestamp = millis();
    else
        imu_object->m_timestamp += imu_object->m_sampleInterval;

    imu_object->m_firstTime = false;

    return true;
}
