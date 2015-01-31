#include <havroc/tracking/imu_driver.h>
#include <havroc/communications/suit/suit_i2c.h>
#include <havroc/error.h>

float mAdj_X = 0;
float mAdj_Y = 0;
float mAdj_Z = 0;
float mUserX = 0;
float mUserY = 0;
float mUserZ = 0;
float aRes = 2.0 / 32768.0;
float gRes = 250.0 / 32768.0;
float mRes = 10. * 4219. / 32760.;
float gyroOffsetX = 0;
float gyroOffsetY = 0;
float gyroOffsetZ = 0;

//Initialize gyro and accelerometer, check if device id is valid
int initMPU(uint32_t imu_index)
{
	uint8_t rxBuff[1];
	uint8_t txBuff[1];

	//Check IMU and Compass device IDs
	suit_i2c_read(imu_index, MPU_ADDRESS, WHO_AM_I_MPU9250, rxBuff, 1);

	if (rxBuff[0] != MPU_WHOAMI)
	{
		return IMU_MPU_START_ERROR;
	}

	//Reset device
	txBuff[0] = 0x80;
	suit_i2c_write(imu_index, MPU_ADDRESS, PWR_MGMT_1, txBuff, 1);
	delay(100);

	//Wake up device
	txBuff[0] = 0x00;
	suit_i2c_write(imu_index, MPU_ADDRESS, PWR_MGMT_1, txBuff, 1);
	delay(100);

	//Get stable time source - PLL gyroscope reference if ready, else use internal oscillator
	txBuff[0] = 0x01;
	suit_i2c_write(imu_index, MPU_ADDRESS, PWR_MGMT_1, txBuff, 1);

	//Disable FSYNC and set thermometer and gyro bandwidth to 41 and 42 Hz
	txBuff[0] = 0x03;
	suit_i2c_write(imu_index, MPU_ADDRESS, CONFIG, txBuff, 1);

	//Set sensor output rate to 200 kHz, remaining consistent with filter update rate
	txBuff[0] = 0x04;
	suit_i2c_write(imu_index, MPU_ADDRESS, SMPLRT_DIV, txBuff, 1);

	//Get gyro config register contents
	suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_CONFIG, rxBuff, 1);

	//Clear Fchoice and full scale bits
	txBuff[0] = rxBuff[0] & 0xE4;
	suit_i2c_write(imu_index, MPU_ADDRESS, GYRO_CONFIG, txBuff, 1);

	//Set full scale range for the gyro
	txBuff[0] = rxBuff[0] | GFS_250DPS << 3;
	suit_i2c_write(imu_index, MPU_ADDRESS, GYRO_CONFIG, txBuff, 1);

	//Get accelerometer config register contents
	suit_i2c_read(imu_index, MPU_ADDRESS, ACCEL_CONFIG, rxBuff, 1);

	//Clear full scale bits
	txBuff[0] = rxBuff[0] & 0xE7;
	suit_i2c_write(imu_index, MPU_ADDRESS, ACCEL_CONFIG, txBuff, 1);

	//Set full scale range for the accelerometer
	txBuff[0] = rxBuff[0] | AFS_2G << 3;
	suit_i2c_write(imu_index, MPU_ADDRESS, ACCEL_CONFIG, txBuff, 1);

	//Get accelerometer config2 register contents
	suit_i2c_read(imu_index, MPU_ADDRESS, ACCEL_CONFIG2, rxBuff, 1);

	//Clear Fchoice and data_I2C rate bits
	txBuff[0] = rxBuff[0] & ~0xF;
	suit_i2c_write(imu_index, MPU_ADDRESS, ACCEL_CONFIG2, txBuff, 1);

	//Set accelerometer bandwidth to 41 Hz
	txBuff[0] = rxBuff[0] | 0x3;
	suit_i2c_write(imu_index, MPU_ADDRESS, ACCEL_CONFIG2, txBuff, 1);

	//Enable Bypass mode so compass is on I2C bus
	txBuff[0] = 0x12;
	suit_i2c_write(imu_index, MPU_ADDRESS, INT_PIN_CFG, txBuff, 1);

	//Enable data_I2C ready interrupt
	txBuff[0] = 0x01;
	suit_i2c_write(imu_index, MPU_ADDRESS, INT_ENABLE, txBuff, 1);

	//Run calibration for sensors
	getGyroOffsets(&gyroOffsetX, &gyroOffsetY, &gyroOffsetZ, imu_index);

	return IMU_MPU_START_SUCCESS;
}

//Initialize compass, check if device id is valid, get factory calibrated sensitivity values
int initCompass(uint32_t imu_index)
{
	uint8_t rxBuff[1];
	uint8_t txBuff[1];

	suit_i2c_read(imu_index, AK8963_ADDRESS, WHO_AM_I_AK8963, rxBuff, 1);

	if (rxBuff[0] != AKM_WHOAMI)
	{
		return IMU_COMPASS_START_ERROR;
	}

	//Shutdown compass
	txBuff[0] = 0x00;
	suit_i2c_write(imu_index, AK8963_ADDRESS, AK8963_CNTL, txBuff, 1);
	delay(10);

	//Enter Fuse ROM access mode
	txBuff[0] = 0x0F;
	suit_i2c_write(imu_index, AK8963_ADDRESS, AK8963_CNTL, txBuff, 1);
	delay(10);

	//Get sensitivity adjustment values
	suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_ASAX, rxBuff, 1);
	mAdj_X = (float) (rxBuff[0] - 128) / 256. + 1.;

	suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_ASAY, rxBuff, 1);
	mAdj_Y = (float) (rxBuff[0] - 128) / 256. + 1.;

	suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_ASAZ, rxBuff, 1);
	mAdj_Z = (float) (rxBuff[0] - 128) / 256. + 1.;

	//Set compass to 8 Hz continuous read mode and to 16 bit resolution
	txBuff[0] = 0x12;
	suit_i2c_write(imu_index, AK8963_ADDRESS, AK8963_CNTL, txBuff, 1);
	delay(10);

	return IMU_COMPASS_START_SUCCESS;
}

int readMPUData(float * accel_X, float * accel_Y, float * accel_Z,
		float * gyro_X, float * gyro_Y, float * gyro_Z, uint32_t imu_index) {
	float x = 0;
	float y = 0;
	float z = 0;
	uint8_t rxBuff[1];
	int i2c_data = 0;

	suit_i2c_read(imu_index, MPU_ADDRESS, INT_STATUS, rxBuff, 1);

	if (rxBuff[0] == 0x01)
	{
		suit_i2c_read(imu_index, MPU_ADDRESS, ACCEL_XOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, MPU_ADDRESS, ACCEL_XOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		x = (float) i2c_data * aRes;

		suit_i2c_read(imu_index, MPU_ADDRESS, ACCEL_YOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, MPU_ADDRESS, ACCEL_YOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		y = (float) i2c_data * aRes;

		suit_i2c_read(imu_index, MPU_ADDRESS, ACCEL_ZOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, MPU_ADDRESS, ACCEL_ZOUT_H, rxBuff, 1);
		i2c_data |= rxBuff[0];

		z = (float) i2c_data * aRes;

		*accel_X = x;
		*accel_Y = y;
		*accel_Z = z;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_XOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_XOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		x = (float) i2c_data * gRes - gyroOffsetX;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_YOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_YOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		y = (float) i2c_data * gRes - gyroOffsetY;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_ZOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_ZOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		z = (float) i2c_data * gRes - gyroOffsetZ;

		*gyro_X = x;
		*gyro_Y = y;
		*gyro_Z = z;
		return IMU_READ_SUCCESS;
	}

	return IMU_READ_FAIL;
}

//Get x,y,z readings from compass
int readCompassData(float * mag_X, float * mag_Y, float * mag_Z, uint32_t imu_index) {
	float x = 0;
	float y = 0;
	float z = 0;
	uint8_t rxBuff[1];
	int i2c_data = 0;

	//Check compass status for new data_I2C
	suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_ST1, rxBuff, 1);

	//If new data_I2C is present, print out new data_I2C
	if (rxBuff[0] & 0x01)
	{
		//Read compass data_I2C then convert to milliGauss
		suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_XOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_XOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		x = (float) i2c_data * mRes * mAdj_X + mUserX;

		suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_YOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_YOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		y = (float) i2c_data * mRes * mAdj_Y + mUserY;

		suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_ZOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_ZOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		z = (float) i2c_data * mRes * mAdj_Z + mUserZ;

		suit_i2c_read(imu_index, AK8963_ADDRESS, AK8963_ST2, rxBuff, 1);

		//Check if magnetic sensor overflow set, if not then report data_I2C
		if (!(rxBuff[0] & 0x08))
		{
			*mag_X = x;
			*mag_Y = y;
			*mag_Z = z;
			return IMU_READ_SUCCESS;
		}
	}

	return IMU_READ_FAIL;
}

void getGyroOffsets(float * gyro_OffsetX, float * gyro_OffsetY,
		float * gyro_OffsetZ, int imu_index) {
	float sumX = 0;
	float sumY = 0;
	float sumZ = 0;
	int i;
	uint8_t rxBuff[1];
	int i2c_data = 0;

	for (i = 0; i < 100; i++) {
		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_XOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_XOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		sumX += (float) i2c_data * gRes;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_YOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_YOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		sumY += (float) i2c_data * gRes;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_ZOUT_H, rxBuff, 1);
		i2c_data = rxBuff[0];
		i2c_data <<= 8;

		suit_i2c_read(imu_index, MPU_ADDRESS, GYRO_ZOUT_L, rxBuff, 1);
		i2c_data |= rxBuff[0];

		sumZ += (float) i2c_data * gRes;
	}

	*gyro_OffsetX = sumX / 100.;
	*gyro_OffsetY = sumY / 100.;
	*gyro_OffsetZ = sumZ / 100.;
}
