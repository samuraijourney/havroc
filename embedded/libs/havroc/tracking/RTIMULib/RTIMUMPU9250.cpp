////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib-Arduino
//
//  Copyright (c) 2014, richards-tech
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "havroc/tracking/RTIMULib/RTIMUMPU9250.h"
#include "havroc/tracking/RTIMULib/RTIMUSettings.h"
#include "havroc/tracking/CalLib/Calibration.h"
#include <havroc/communications/suit/suit_i2c.h>

#include <utils.h>
#include "common.h"
#include <havroc/error.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/knl/Clock.h>

#define delay(ms) UtilsDelay((80000/5)*ms)

unsigned long millis ()
{
	Types_FreqHz freq;

	Timestamp_getFreq(&freq);

	return (Timestamp_get32()*1000.0/(1.0*freq.lo));
}

//  Axis rotation array

RTFLOAT RTIMUMPU9250::m_axisRotation[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

RTIMUMPU9250::RTIMUMPU9250(RTIMUSettings *settings, int imu_index)
{
    m_settings = settings;
    m_imu_index = imu_index;
    m_calibrationMode = false;
    m_calibrationValid = false;
    m_gyroBiasValid = false;
}

RTIMUMPU9250::~RTIMUMPU9250()
{

}


void RTIMUMPU9250::setCalibrationData()
{
    float maxDelta = -1;
    float delta;

    m_calibrationValid = false;

	if (!calib_valid[m_imu_index]) {
		return;
	}

	//  find biggest range
	for (int i = 0; i < 3; i++) {
		if ((magMax[m_imu_index][i] - magMin[m_imu_index][i]) > maxDelta)
			maxDelta = magMax[m_imu_index][i] - magMin[m_imu_index][i];
	}
	if (maxDelta < 0) {
		return;
	}
	maxDelta /= 2.0f;                                       // this is the max +/- range

	for (int i = 0; i < 3; i++) {
		delta = (magMax[m_imu_index][i] - magMin[m_imu_index][i]) / 2.0f;
		m_compassCalScale[i] = maxDelta / delta;            // makes everything the same range
		m_compassCalOffset[i] = (magMax[m_imu_index][i] + magMin[m_imu_index][i]) / 2.0f;
	}
	m_calibrationValid = true;

}


void RTIMUMPU9250::gyroBiasInit()
{
    m_gyroAlpha = 2.0f / m_sampleRate;
    m_gyroSampleCount = 0;
}


void RTIMUMPU9250::handleGyroBias()
{
    RTVector3 deltaAccel = m_previousAccel;
    deltaAccel -= m_accel;   // compute difference
    m_previousAccel = m_accel;

    if ((deltaAccel.squareLength() < RTIMU_FUZZY_ACCEL_ZERO_SQUARED) && 
                (m_gyro.squareLength() < RTIMU_FUZZY_GYRO_ZERO_SQUARED)) {
        // what we are seeing on the gyros should be bias only so learn from this
        m_gyroBias.setX((1.0 - m_gyroAlpha) * m_gyroBias.x() + m_gyroAlpha * m_gyro.x());
        m_gyroBias.setY((1.0 - m_gyroAlpha) * m_gyroBias.y() + m_gyroAlpha * m_gyro.y());
        m_gyroBias.setZ((1.0 - m_gyroAlpha) * m_gyroBias.z() + m_gyroAlpha * m_gyro.z());

        if (m_gyroSampleCount < (5 * m_sampleRate)) {
            m_gyroSampleCount++;

            if (m_gyroSampleCount == (5 * m_sampleRate)) {
                m_gyroBiasValid = true;
            }
        }
    }

    m_gyro -= m_gyroBias;
}

void RTIMUMPU9250::calibrateAverageCompass()
{
    //  calibrate if required

    if (!m_calibrationMode && m_calibrationValid) {
        m_compass.setX((m_compass.x() - m_compassCalOffset[0]) * m_compassCalScale[0]);
        m_compass.setY((m_compass.y() - m_compassCalOffset[1]) * m_compassCalScale[1]);
        m_compass.setZ((m_compass.z() - m_compassCalOffset[2]) * m_compassCalScale[2]);
    }

    //  update running average

    m_compassAverage.setX(m_compass.x() * COMPASS_ALPHA + m_compassAverage.x() * (1.0 - COMPASS_ALPHA));
    m_compassAverage.setY(m_compass.y() * COMPASS_ALPHA + m_compassAverage.y() * (1.0 - COMPASS_ALPHA));
    m_compassAverage.setZ(m_compass.z() * COMPASS_ALPHA + m_compassAverage.z() * (1.0 - COMPASS_ALPHA));

    m_compass = m_compassAverage;
}

bool RTIMUMPU9250::IMUGyroBiasValid()
{
    return m_gyroBiasValid;
}


bool RTIMUMPU9250::setSampleRate(int rate)
{
    if ((rate < MPU9250_SAMPLERATE_MIN) || (rate > MPU9250_SAMPLERATE_MAX)) {
        return false;
    }
    m_sampleRate = rate;
    m_sampleInterval = (unsigned long)1000 / m_sampleRate;
    if (m_sampleInterval == 0)
        m_sampleInterval = 1;
    return true;
}

bool RTIMUMPU9250::setGyroLpf(unsigned char lpf)
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
        m_gyroLpf = lpf;
        return true;

    default:
        return false;
    }
}

bool RTIMUMPU9250::setAccelLpf(unsigned char lpf)
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
        m_accelLpf = lpf;
        return true;

    default:
        return false;
    }
}

bool RTIMUMPU9250::setCompassRate(int rate)
{
    if ((rate < MPU9250_COMPASSRATE_MIN) || (rate > MPU9250_COMPASSRATE_MAX)) {
        return false;
    }
    m_compassRate = rate;
    return true;
}

bool RTIMUMPU9250::setGyroFsr(unsigned char fsr)
{
    switch (fsr) {
    case MPU9250_GYROFSR_250:
        m_gyroFsr = fsr;
        m_gyroScale = RTMATH_PI / (131.0 * 180.0);
        return true;

    case MPU9250_GYROFSR_500:
        m_gyroFsr = fsr;
        m_gyroScale = RTMATH_PI / (62.5 * 180.0);
        return true;

    case MPU9250_GYROFSR_1000:
        m_gyroFsr = fsr;
        m_gyroScale = RTMATH_PI / (32.8 * 180.0);
        return true;

    case MPU9250_GYROFSR_2000:
        m_gyroFsr = fsr;
        m_gyroScale = RTMATH_PI / (16.4 * 180.0);
        return true;

    default:
        return false;
    }
}

bool RTIMUMPU9250::setAccelFsr(unsigned char fsr)
{
    switch (fsr) {
    case MPU9250_ACCELFSR_2:
        m_accelFsr = fsr;
        m_accelScale = 1.0/16384.0;
        return true;

    case MPU9250_ACCELFSR_4:
        m_accelFsr = fsr;
        m_accelScale = 1.0/8192.0;
        return true;

    case MPU9250_ACCELFSR_8:
        m_accelFsr = fsr;
        m_accelScale = 1.0/4096.0;
        return true;

    case MPU9250_ACCELFSR_16:
        m_accelFsr = fsr;
        m_accelScale = 1.0/2048.0;
        return true;

    default:
        return false;
    }
}


bool RTIMUMPU9250::IMUInit()
{
    uint8_t txBuff[1];
    uint8_t rxBuff[1];

    m_firstTime = true;

    //  configure IMU

    m_slaveAddr = m_settings->m_I2CSlaveAddress;

    setSampleRate(m_settings->m_MPU9250GyroAccelSampleRate);
    setCompassRate(m_settings->m_MPU9250CompassSampleRate);
    setGyroLpf(m_settings->m_MPU9250GyroLpf);
    setAccelLpf(m_settings->m_MPU9250AccelLpf);
    setGyroFsr(m_settings->m_MPU9250GyroFsr);
    setAccelFsr(m_settings->m_MPU9250AccelFsr);

    setCalibrationData();

    //  reset the MPU9250
    txBuff[0] = 0x80;

    suit_i2c_write(m_slaveAddr, MPU9250_PWR_MGMT_1, txBuff, 1);

    delay(100);

    txBuff[0] = 0x00;
    suit_i2c_write(m_slaveAddr, MPU9250_PWR_MGMT_1, txBuff, 1);

	suit_i2c_read(m_slaveAddr, MPU9250_WHO_AM_I, rxBuff, 1);

    if (rxBuff[0] != MPU9250_ID) {
         return false;
    }

    //  now configure the various components

    if (!setGyroConfig())
        return false;

    if (!setAccelConfig())
        return false;

    if (!setSampleRate())
        return false;

    //  now configure compass

    if (!bypassOn())
        return false;

    // get fuse ROM data

    txBuff[0] = 0x00;
	suit_i2c_write(AK8963_ADDRESS, AK8963_CNTL, txBuff, 1);

	txBuff[0] = 0x0f;
	suit_i2c_write(AK8963_ADDRESS, AK8963_CNTL, txBuff, 1);

	suit_i2c_read(AK8963_ADDRESS, AK8963_ASAX, rxBuff, 3);

    //  convert asa to usable scale factor

    m_compassAdjust[0] = ((float)rxBuff[0] - 128.0) / 256.0 + 1.0f;
    m_compassAdjust[1] = ((float)rxBuff[1] - 128.0) / 256.0 + 1.0f;
    m_compassAdjust[2] = ((float)rxBuff[2] - 128.0) / 256.0 + 1.0f;

    txBuff[0] = 0x00;
	suit_i2c_write(AK8963_ADDRESS, AK8963_CNTL, txBuff, 1);

    if (!bypassOff())
        return false;

    //  now set up MPU9250 to talk to the compass chip

    txBuff[0] = 0x40;
	suit_i2c_write(m_slaveAddr, MPU9250_I2C_MST_CTRL, txBuff, 1);

    txBuff[0] = 0x80 | AK8963_ADDRESS;
	suit_i2c_write(m_slaveAddr, MPU9250_I2C_SLV0_ADDR, txBuff, 1);

    txBuff[0] = AK8963_ST1;
	suit_i2c_write(m_slaveAddr, MPU9250_I2C_SLV0_REG, txBuff, 1);

	txBuff[0] = 0x88;
	suit_i2c_write(m_slaveAddr, MPU9250_I2C_SLV0_CTRL, txBuff, 1);

	txBuff[0] = AK8963_ADDRESS;
	suit_i2c_write(m_slaveAddr, MPU9250_I2C_SLV1_ADDR, txBuff, 1);

	txBuff[0] = AK8963_CNTL;
	suit_i2c_write(m_slaveAddr, MPU9250_I2C_SLV1_REG, txBuff, 1);

	txBuff[0] = 0x81;
	suit_i2c_write(m_slaveAddr, MPU9250_I2C_SLV1_CTRL, txBuff, 1);

	txBuff[0] = 0x1;
	suit_i2c_write(m_slaveAddr, MPU9250_I2C_SLV1_DO, txBuff, 1);

	txBuff[0] = 0x3;
	suit_i2c_write(m_slaveAddr, MPU9250_I2C_MST_DELAY_CTRL, txBuff, 1);

    if (!setCompassRate())
        return false;

    //  enable the sensors
    txBuff[0] = 0x1;
	suit_i2c_write(m_slaveAddr, MPU9250_PWR_MGMT_1, txBuff, 1);

    txBuff[0] = 0x0;
	suit_i2c_write(m_slaveAddr, MPU9250_PWR_MGMT_2, txBuff, 1);

    //  select the data to go into the FIFO and enable

    if (!resetFifo())
        return false;

    gyroBiasInit();
    return true;
}

bool RTIMUMPU9250::resetFifo()
{
	uint8_t txBuff[1];

	txBuff[0] = 0x0;
	suit_i2c_write(m_slaveAddr, MPU9250_INT_ENABLE, txBuff, 1);

	txBuff[0] = 0x0;
	suit_i2c_write(m_slaveAddr, MPU9250_FIFO_EN, txBuff, 1);

	txBuff[0] = 0x0;
	suit_i2c_write(m_slaveAddr, MPU9250_USER_CTRL, txBuff, 1);

	txBuff[0] = 0x04;
	suit_i2c_write(m_slaveAddr, MPU9250_USER_CTRL, txBuff, 1);

	txBuff[0] = 0x60;
	suit_i2c_write(m_slaveAddr, MPU9250_USER_CTRL, txBuff, 1);

    delay(50);

    txBuff[0] = 1;
	suit_i2c_write(m_slaveAddr, MPU9250_INT_ENABLE, txBuff, 1);

    txBuff[0] = 0x78;
	suit_i2c_write(m_slaveAddr, MPU9250_FIFO_EN, txBuff, 1);

    return true;
}

bool RTIMUMPU9250::bypassOn()
{
	uint8_t txBuff[1];
    unsigned char userControl;

    suit_i2c_read(m_slaveAddr, MPU9250_USER_CTRL, &userControl, 1);

    userControl &= ~0x20;
    userControl |= 2;

    txBuff[0] = userControl;
	suit_i2c_write(m_slaveAddr, MPU9250_USER_CTRL, txBuff, 1);

    delay(50);

    txBuff[0] = 0x82;
	suit_i2c_write(m_slaveAddr, MPU9250_INT_PIN_CFG, txBuff, 1);

    delay(50);
    return true;
}


bool RTIMUMPU9250::bypassOff()
{
	uint8_t txBuff[1];
    unsigned char userControl;

    suit_i2c_read(m_slaveAddr, MPU9250_USER_CTRL, &userControl, 1);

    userControl |= 0x20;

    txBuff[0] = userControl;
	suit_i2c_write(m_slaveAddr, MPU9250_USER_CTRL, txBuff, 1);

    delay(50);

    txBuff[0] = 0x80;
    suit_i2c_write(m_slaveAddr, MPU9250_INT_PIN_CFG, txBuff, 1);

    delay(50);
    return true;
}

bool RTIMUMPU9250::setGyroConfig()
{
	uint8_t txBuff[1];
    unsigned char gyroConfig = m_gyroFsr + ((m_gyroLpf >> 3) & 3);
    unsigned char gyroLpf = m_gyroLpf & 7;

    txBuff[0] = gyroConfig;
	suit_i2c_write(m_slaveAddr, MPU9250_GYRO_CONFIG, txBuff, 1);

	txBuff[0] = gyroLpf;
	suit_i2c_write(m_slaveAddr, MPU9250_GYRO_LPF, txBuff, 1);

    return true;
}

bool RTIMUMPU9250::setAccelConfig()
{
	uint8_t txBuff[1];

	txBuff[0] = m_accelFsr;
	suit_i2c_write(m_slaveAddr, MPU9250_ACCEL_CONFIG, txBuff, 1);

	txBuff[0] = m_accelLpf;
	suit_i2c_write(m_slaveAddr, MPU9250_ACCEL_LPF, txBuff, 1);

    return true;
}

bool RTIMUMPU9250::setSampleRate()
{
	uint8_t txBuff[1];

    if (m_sampleRate > 1000)
        return true;                                        // SMPRT not used above 1000Hz

    txBuff[0] = (unsigned char) (1000 / m_sampleRate - 1);
	suit_i2c_write(m_slaveAddr, MPU9250_SMPRT_DIV, txBuff, 1);

    return true;
}


bool RTIMUMPU9250::setCompassRate()
{
    int rate;
    uint8_t txBuff[1];

    rate = m_sampleRate / m_compassRate - 1;

    if (rate > 31)
        rate = 31;

    txBuff[0] = rate;
	suit_i2c_write(m_slaveAddr, MPU9250_I2C_SLV4_CTRL, txBuff, 1);

    return true;
}

int RTIMUMPU9250::IMUGetPollInterval()
{
    return (400 / m_sampleRate);
}

bool RTIMUMPU9250::IMURead()
{
    unsigned char fifoCount[2];
    unsigned int count;
    unsigned char fifoData[12];
    unsigned char compassData[8];

    suit_i2c_read(m_slaveAddr, MPU9250_FIFO_COUNT_H, fifoCount, 2);

    count = ((unsigned int)fifoCount[0] << 8) + fifoCount[1];

    if (count == 1024) {
        resetFifo();
        m_timestamp += m_sampleInterval * (1024 / MPU9250_FIFO_CHUNK_SIZE + 1); // try to fix timestamp
        return false;
    }

    if (count > MPU9250_FIFO_CHUNK_SIZE * 40) {
        // more than 40 samples behind - going too slowly so discard some samples but maintain timestamp correctly
        while (count >= MPU9250_FIFO_CHUNK_SIZE * 10) {
            suit_i2c_read(m_slaveAddr, MPU9250_FIFO_R_W, fifoData, MPU9250_FIFO_CHUNK_SIZE);

            count -= MPU9250_FIFO_CHUNK_SIZE;
            m_timestamp += m_sampleInterval;
        }
    }

    if (count < MPU9250_FIFO_CHUNK_SIZE)
        return false;

    suit_i2c_read(m_slaveAddr, MPU9250_FIFO_R_W, fifoData, MPU9250_FIFO_CHUNK_SIZE);

    suit_i2c_read(m_slaveAddr, MPU9250_EXT_SENS_DATA_00, compassData, 8);

    RTMath::convertToVector(fifoData, m_accel, m_accelScale, true);
    RTMath::convertToVector(fifoData + 6, m_gyro, m_gyroScale, true);
    RTMath::convertToVector(compassData + 1, m_compass, 0.6f, false);

    //  sort out gyro axes

    m_gyro.setY(-m_gyro.y());
    m_gyro.setZ(-m_gyro.z());

    //  sort out accel data;

    m_accel.setX(-m_accel.x());

    //  use the fuse data adjustments for compass

    m_compass.setX(m_compass.x() * m_compassAdjust[0]);
    m_compass.setY(m_compass.y() * m_compassAdjust[1]);
    m_compass.setZ(m_compass.z() * m_compassAdjust[2]);

    //  sort out compass axes

    float temp;

    temp = m_compass.x();
    m_compass.setX(m_compass.y());
    m_compass.setY(-temp);

    //  now do standard processing

    handleGyroBias();
    calibrateAverageCompass();

    if (m_firstTime)
        m_timestamp = millis();
    else
        m_timestamp += m_sampleInterval;

    m_firstTime = false;

    return true;
}
