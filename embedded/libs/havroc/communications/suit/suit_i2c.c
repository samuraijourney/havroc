#include <xdc/runtime/System.h>
#include <ti/drivers/I2C.h>
#include <havroc/communications/suit/suit_i2c.h>
#include <xdc/runtime/Memory.h>
#include "uart_if.h"
#include "common.h"

void suit_i2c_transfer( uint32_t i2cIndex,
					    uint8_t addr,
					    uint8_t txBuff[],
					    size_t writeCount,
					    uint8_t rxBuff[],
					    size_t readCount )
{
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;

    i2cIndex = 0;

    /* Create I2C for usage */

    I2C_Params_init(&i2cParams);
    i2cParams.transferMode = I2C_MODE_BLOCKING;
    i2cParams.bitRate = I2C_400kHz;

    i2c = I2C_open(i2cIndex, &i2cParams);

    if (i2c == NULL)
    {
        Report("Error Initializing I2C\n\r");
    }
    else
    {
        //System_printf("I2C Initialized!\n");
    }

    i2cTransaction.slaveAddress = addr;
    i2cTransaction.writeBuf = &(txBuff[0]);
    i2cTransaction.writeCount = writeCount;
    i2cTransaction.readBuf = &(rxBuff[0]);
    i2cTransaction.readCount = readCount;

	if (!(I2C_transfer(i2c, &i2cTransaction)))
	{
		Report("I2C Bus fault\n\r");
	}

    /* Deinitialized I2C */
    I2C_close(i2c);
}

void suit_i2c_read(uint32_t i2cIndex, uint8_t addr, uint8_t reg_addr, uint8_t rxBuff[], size_t readCount)
{
	uint8_t txBuff[1];
	txBuff[0] = reg_addr;

	suit_i2c_transfer(i2cIndex, addr, txBuff, 1, rxBuff, readCount);
}

void suit_i2c_write(uint32_t i2cIndex, uint8_t addr, uint8_t reg_addr, uint8_t txBuff[], size_t writeCount)
{
	uint8_t* writeBuff = (uint8_t*)malloc(sizeof(uint8_t)*(writeCount+1));
	writeBuff[0] = reg_addr;

	memcpy (&writeBuff[1], txBuff, writeCount*sizeof(uint8_t));

	suit_i2c_transfer(i2cIndex, addr, writeBuff, writeCount+1, NULL, 0);

	free(writeBuff);
}
