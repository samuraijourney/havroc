#include <stdbool.h>

#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/knl/Semaphore.h>

#include "havroc/communications/suit/suit_i2c.h"

#include "uart_if.h"
#include "common.h"

/* I2C access sem*/
Semaphore_Handle sem;

SuitI2CErrorCode suit_i2c_transfer(uint8_t addr,
									uint8_t writeBuff[],
									size_t writeCount,
									uint8_t readBuff[],
									size_t readCount)
{
	SuitI2CErrorCode retVal = SUIT_I2C_E_SUCCESS;

	 /* Get access to I2C */
	Semaphore_pend(sem, BIOS_WAIT_FOREVER);

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

	/* Release I2C*/
	Semaphore_post(sem);

	if (retVal != SUIT_I2C_E_SUCCESS)
		Report("I2C Transfer Error\n\r");

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

	 /* Get access to I2C */
	Semaphore_pend(sem, BIOS_WAIT_FOREVER);

	if (I2C_IF_ReadFrom(addr, writeBuff, 1, readBuff, readCount) < 0)
	{
		retVal = SUIT_I2C_E_BUS_FAULT;
		Report("I2C Read Error\n\r");
	}

	/* Release I2C*/
	Semaphore_post(sem);

	return retVal;
}

SuitI2CErrorCode suit_i2c_write(uint8_t addr,
								uint8_t regAddr,
								uint8_t writeBuff[],
								size_t writeCount)
{
	SuitI2CErrorCode retVal = SUIT_I2C_E_SUCCESS;
	uint8_t writeBuff2[2];

	writeBuff2[0] = regAddr;
	writeBuff2[1] = writeBuff[0];

	/* Get access to I2C */
	Semaphore_pend(sem, BIOS_WAIT_FOREVER);

	if (I2C_IF_Write(addr, writeBuff2, writeCount + 1, true) < 0)
	{
		retVal = SUIT_I2C_E_BUS_FAULT;
		Report("I2C Write Error\n\r");
	}

	/* Release I2C*/
	Semaphore_post(sem);

	return retVal;
}

SuitI2CErrorCode suit_i2c_init()
{
	SuitI2CErrorCode retVal = SUIT_I2C_E_SUCCESS;
	Semaphore_Params params;

	if (I2C_IF_Open(SUIT_I2C_BITRATE) < 0)
		retVal = SUIT_I2C_E_ERROR;

	//init read/write sem
	Semaphore_Params_init(&params);
	params.mode = Semaphore_Mode_BINARY;
	sem = Semaphore_create(1, &params, NULL);

	return retVal;
}
