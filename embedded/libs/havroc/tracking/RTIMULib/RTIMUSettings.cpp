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

#include "havroc\tracking\RTIMULib\RTIMUSettings.h"
#include "havroc\tracking\RTIMULib\RTIMUMPU9250.h"

#define RATE_TIMER_INTERVAL 2

RTIMUSettings::RTIMUSettings()
{
    //  preset general defaults

    m_imuType = -1;
    m_I2CSlaveAddress = 0;

    //  MPU9250 defaults

    m_MPU9250GyroAccelSampleRate = 80;
    m_MPU9250CompassSampleRate = 40;
    m_MPU9250GyroLpf = MPU9250_GYRO_LPF_41;
    m_MPU9250AccelLpf = MPU9250_ACCEL_LPF_41;
    m_MPU9250GyroFsr = MPU9250_GYROFSR_1000;
    m_MPU9250AccelFsr = MPU9250_ACCELFSR_8;
    m_I2CSlaveAddress = MPU9250_ADDRESS0;
}
