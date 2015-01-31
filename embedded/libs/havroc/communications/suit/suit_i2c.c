//#include <xdc/runtime/System.h>
//#include <ti/drivers/I2C.h>
//
//#include <havroc/communications/suit/suit_i2c.h>
//
//void suit_i2c_transfer( uint32_t i2cIndex,
//					    uint8_t addr,
//					    uint8_t reg_addr,
//					    uint8_t txBuff[],
//					    size_t writeCount,
//					    uint8_t rxBuff[],
//					    size_t readCount )
//{
//    I2C_Handle      i2c;
//    I2C_Params      i2cParams;
//    I2C_Transaction i2cTransaction;
//
//    /* Create I2C for usage */
//
//    I2C_Params_init(&i2cParams);
//    i2cParams.bitRate = I2C_400kHz;
//
//    i2c = I2C_open(i2cIndex, &i2cParams);
//
//    if (i2c == NULL)
//    {
//        System_abort("Error Initializing I2C\n");
//    }
//    else
//    {
//        System_printf("I2C Initialized!\n");
//    }
//
//    uint8_t* writeBuff = (uint8_t*)malloc(sizeof(uint8_t)*(writeCount+1));
//    writeBuff[0] = reg_addr;
//
//    if(writeCount > 0)
//    	memcpy (&writeBuff[1], txBuff, writeCount*sizeof(uint8_t));
//
//    i2cTransaction.slaveAddress = addr;
//    i2cTransaction.writeBuf = &(writeBuff[0]);
//    i2cTransaction.writeCount = writeCount;
//    i2cTransaction.readBuf = &(rxBuff[0]);
//    i2cTransaction.readCount = readCount;
//
//	if (!(I2C_transfer(i2c, &i2cTransaction)))
//	{
//		System_printf("I2C Bus fault\n");
//	}
//
//    /* Deinitialized I2C */
//    I2C_close(i2c);
//}

#include <xdc/runtime/System.h>
#include <ti/drivers/I2C.h>

#include <havroc/communications/suit/suit_i2c.h>

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

    /* Create I2C for usage */

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    i2c = I2C_open(i2cIndex, &i2cParams);

    if (i2c == NULL)
    {
        System_abort("Error Initializing I2C\n");
    }
    else
    {
        System_printf("I2C Initialized!\n");
    }

    i2cTransaction.slaveAddress = addr;
    i2cTransaction.writeBuf = &(txBuff[0]);
    i2cTransaction.writeCount = writeCount;
    i2cTransaction.readBuf = &(rxBuff[0]);
    i2cTransaction.readCount = readCount;

	if (!(I2C_transfer(i2c, &i2cTransaction)))
	{
		System_printf("I2C Bus fault\n");
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
