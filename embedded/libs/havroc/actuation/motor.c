
/* Standard C Includes */
#include "stdbool.h"

/* HaVRoc Library Includes */
#include "havroc/actuation/motor.h"


#define DRV2604L_ADDR 			0x5A

#define MOTOR_RTP_REG			0x02

#define MOTOR_MODE_REG			0x01
#define MOTOR_MODE_REG_VAL		0x05

#define MOTOR_CTL3_REG			0x1D
#define MOTOR_CTL3_REG_VAL		0x88

MotorErrorCode motor_run(Motor *motor, uint8_t intensity)
{
	uint8_t writeBuff[1];
	MotorErrorCode retVal = MOTOR_E_SUCCESS;

	motor->intensity = intensity;

	writeBuff[0] = intensity;
	if (suit_i2c_write(DRV2604L_ADDR, MOTOR_RTP_REG, writeBuff, 1) > 0)
		retVal = MOTOR_E_FAIL;

	return retVal;
}

MotorErrorCode motor_calibrate(Motor *motor)
{
	uint8_t readBuff[1];
	uint8_t writeBuff[1];
	MotorErrorCode retVal = MOTOR_E_SUCCESS;

	if (suit_i2c_read(DRV2604L_ADDR, 0x00, readBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	// put in standby mode to begin programming
	writeBuff[0] = 0x07;
	if (suit_i2c_write(DRV2604L_ADDR, 0x01, writeBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	// specify actuator specific data, this is pg 26 step 3a, b and c
	writeBuff[0] = 0x2A;
	if (suit_i2c_write(DRV2604L_ADDR, 0x1A, writeBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	// pg 26 step 3d-motor voltage set at 3V here
	writeBuff[0] = 0x8E;
	if (suit_i2c_write(DRV2604L_ADDR, 0x16, writeBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	// pg 26 step 3e- max motor voltage set at 3.5 V here
	writeBuff[0] = 0xA2;
	if (suit_i2c_write(DRV2604L_ADDR, 0x17, writeBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	// pg 26 step 3g- setting drive time, 2.4 ms for ERM
	writeBuff[0] = 0x93;
	if (suit_i2c_write(DRV2604L_ADDR, 0x1B, writeBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	// pg 26 step 3h, i and j
	writeBuff[0] = 0xF5;
	if (suit_i2c_write(DRV2604L_ADDR, 0x1C, writeBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	// pg 26 step 3f and k
	writeBuff[0] = 0x30;
	if (suit_i2c_write(DRV2604L_ADDR, 0x1E, writeBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	// pg 26 step 4, setting the go bit to 1 to start auto calibration
	writeBuff[0] = 0x01;
	if (suit_i2c_write(DRV2604L_ADDR, 0x0C, writeBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	// pg 26 step 5, check results of auto calibration
	if (suit_i2c_read(DRV2604L_ADDR, 0x00, readBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;
	
	//set Mode reg to read from SPEED CTRL REG
	writeBuff[0] = MOTOR_MODE_REG_VAL;
	if (suit_i2c_write(DRV2604L_ADDR, MOTOR_MODE_REG, writeBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	//set CTL3 reg to do unsigned speeds values
	writeBuff[0] = MOTOR_CTL3_REG_VAL;
	if (suit_i2c_write(DRV2604L_ADDR, MOTOR_CTL3_REG, writeBuff, 1) != SUIT_I2C_E_SUCCESS)
		retVal = MOTOR_E_FAIL;

	if (retVal == MOTOR_E_SUCCESS)
	{
		motor->calibrateComplete = true;
	}
	else
	{
		motor->calibrateComplete = false;
	}

	return retVal;
}

