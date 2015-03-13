#ifndef SUIT_I2C_H
#define SUIT_I2C_H

#include <xdc/runtime/System.h>
#include <ti/drivers/I2C.h>

#define SUIT_I2C_INDEX		0
#define SUIT_I2C_BITRATE	I2C_400kHz

/*
 * I2C error codes.
 */
typedef enum _SuitI2CErrorCode
{
	SUIT_I2C_E_SUCCESS = 0,
	SUIT_I2C_E_INIT_FAIL = 1,
	SUIT_I2C_E_BUS_FAULT = 2
} SuitI2CErrorCode;

/*
 * Read/Write I2C main fxn
 */
SuitI2CErrorCode suit_i2c_transfer(uint8_t addr,
									uint8_t writeBuff[],
									size_t writeCount, 
									uint8_t readBuff[], 
									size_t readCount);

/*
 * I2C read helper fxn
 */
SuitI2CErrorCode suit_i2c_read( uint8_t device_index,
								uint8_t addr,
							 	uint8_t regAddr, 
							 	uint8_t readBuff[], 
							 	size_t readCount);

/*
 * I2C write helper fxn
 */
SuitI2CErrorCode suit_i2c_write(uint8_t device_index,
								uint8_t addr,
								uint8_t regAddr, 
								uint8_t writeBuff[], 
								size_t writeCount);

#endif /* SUIT_I2C_H */
