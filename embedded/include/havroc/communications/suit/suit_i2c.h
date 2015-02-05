#ifndef H_SUIT_I2C
#define H_SUIT_I2C

#include <stdint.h>
#include <stdlib.h>

void suit_i2c_transfer(uint32_t i2cIndex, uint8_t addr,
		uint8_t txBuff[], size_t writeCount, uint8_t rxBuff[], size_t readCount);

void suit_i2c_read(uint32_t i2cIndex, uint8_t addr, uint8_t reg_addr, uint8_t rxBuff[], size_t readCount);

void suit_i2c_write(uint32_t i2cIndex, uint8_t addr, uint8_t reg_addr, uint8_t txBuff[], size_t writeCount);

#endif /* H_SUIT_I2C */
