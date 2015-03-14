#include <xdc/std.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <stdbool.h>

#include "havroc/communications/suit/suit_i2c.h"

SuitI2CErrorCode suit_i2c_transfer(uint8_t addr,
									uint8_t writeBuff[],
									size_t writeCount,
									uint8_t readBuff[],
									size_t readCount)
{
	SuitI2CErrorCode retVal = SUIT_I2C_E_SUCCESS;
	uint8_t stopCond = false;

	if (readBuff == NULL || readCount == 0)
	{
		if (I2C_IF_Write(addr, writeBuff, writeCount, true) < 0)
			retVal = SUIT_I2C_E_BUS_FAULT;
	}
	else
	{
		if (I2C_IF_Write(addr, writeBuff, writeCount, false) < 0)
		{
			retVal = SUIT_I2C_E_BUS_FAULT;
		}
		else
		{
			if (I2C_IF_Read(addr, readBuff, readCount) < 0)
			{
				retVal = SUIT_I2C_E_BUS_FAULT;
			}

		}
	}

	if (retVal != SUIT_I2C_E_SUCCESS)
		Report("I2C Transfer Error\n");

    return retVal;
}

SuitI2CErrorCode suit_i2c_read(uint8_t addr,
							 	uint8_t regAddr,
							 	uint8_t readBuff[],
							 	size_t readCount)
{
	SuitI2CErrorCode retVal = SUIT_I2C_E_SUCCESS;
	uint8_t writeBuff[1];
	writeBuff[0] = regAddr;

	if (I2C_IF_ReadFrom(addr, writeBuff, 1, readBuff, readCount) < 0)
	{
		retVal = SUIT_I2C_E_BUS_FAULT;
		Report("I2C Read Error\n");
	}


	return retVal;
}

SuitI2CErrorCode suit_i2c_write(uint8_t addr,
								uint8_t regAddr,
								uint8_t writeBuff[],
								size_t writeCount)
{
	SuitI2CErrorCode retVal = SUIT_I2C_E_SUCCESS;
	uint8_t* writeBuff2;

	writeBuff2 = Memory_alloc(NULL, writeCount + 1, 0, NULL);

	writeBuff2[0] = regAddr;

	memcpy (&(writeBuff2[1]), writeBuff, writeCount);

	if (I2C_IF_Write(addr, writeBuff2, writeCount + 1, true) < 0)
	{
		retVal = SUIT_I2C_E_BUS_FAULT;
		Report("I2C Write Error\n");
	}

	Memory_free(NULL, writeBuff2, writeCount + 1);

	return retVal;
}

SuitI2CErrorCode suit_i2c_init()
{
	SuitI2CErrorCode retVal = SUIT_I2C_E_SUCCESS;

	if (I2C_IF_Open(SUIT_I2C_BITRATE) < 0)
		retVal = SUIT_I2C_E_ERROR;

	return retVal;
}
