#include <xdc/std.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include "havroc/communications/suit/suit_i2c.h"

SuitI2CErrorCode suit_i2c_transfer(uint8_t addr,
									uint8_t writeBuff[],
									size_t writeCount, 
									uint8_t readBuff[], 
									size_t readCount)
{
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;
    SuitI2CErrorCode retVal = SUIT_I2C_E_SUCCESS;

    /* Create I2C for usage */

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = SUIT_I2C_BITRATE;

    i2c = I2C_open(SUIT_I2C_INDEX, &i2cParams);

    if (i2c == NULL)
        retVal = SUIT_I2C_E_INIT_FAIL;

    i2cTransaction.slaveAddress = addr;
    i2cTransaction.writeBuf = writeBuff;
    i2cTransaction.writeCount = writeCount;
    i2cTransaction.readBuf = readBuff;
    i2cTransaction.readCount = readCount;

	if (!(I2C_transfer(i2c, &i2cTransaction)))
		retVal = SUIT_I2C_E_BUS_FAULT;

    /* Deinitialized I2C */
    I2C_close(i2c);

    return retVal;
}

SuitI2CErrorCode suit_i2c_read(uint8_t addr, 
							 	uint8_t regAddr, 
							 	uint8_t readBuff[], 
							 	size_t readCount)
{
	uint8_t writeBuff[1];
	writeBuff[0] = regAddr;

	return suit_i2c_transfer(addr, writeBuff, 1, readBuff, readCount);
}

SuitI2CErrorCode suit_i2c_write(uint8_t addr, 
								uint8_t regAddr, 
								uint8_t writeBuff[], 
								size_t writeCount)
{
	SuitI2CErrorCode retVal;
	uint8_t* writeBuff2;

	writeBuff2 = Memory_alloc(NULL, writeCount + 1, 0, NULL);

	writeBuff2[0] = regAddr;

	memcpy (&(writeBuff2[1]), writeBuff, writeCount);

	retVal = suit_i2c_transfer(addr, writeBuff, writeCount + 1, NULL, 0);

	Memory_free(NULL, writeBuff2, writeCount + 1);
	return retVal;
}
