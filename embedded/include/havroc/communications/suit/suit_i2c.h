#ifndef SUIT_I2C_H
#define SUIT_I2C_H

#include <xdc/runtime/System.h>
#include <i2c_if.h>

#define SUIT_I2C_BITRATE	I2C_MASTER_MODE_FST

/*
 * I2C error codes.
 */
typedef enum _SuitI2CErrorCode
{
	SUIT_I2C_E_SUCCESS = 0,
	SUIT_I2C_E_BUS_FAULT,
	SUIT_I2C_E_ERROR
} SuitI2CErrorCode;

/*
 * I2C read helper fxn
 */
SuitI2CErrorCode suit_i2c_read(uint8_t addr,
							 	uint8_t regAddr,
							 	uint8_t readBuff[],
							 	size_t readCount);

/*
 * I2C write helper fxn
 */
SuitI2CErrorCode suit_i2c_write(uint8_t addr,
								uint8_t regAddr,
								uint8_t writeBuff[],
								size_t writeCount);

/*
 * I2C read/write tranfer
 */
SuitI2CErrorCode suit_i2c_transfer(uint8_t addr,
									uint8_t writeBuff[],
									size_t writeCount,
									uint8_t readBuff[],
									size_t readCount);

/*
 * I2C init
 */
SuitI2CErrorCode suit_i2c_init();

#endif /* SUIT_I2C_H */
