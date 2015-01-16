#ifndef H_SUIT_I2C
#define H_SUIT_I2C

void suit_i2c_transfer( uint32_t i2cIndex,
					    uint8_t addr,
					    uint8_t *txBuff,
					    size_t writeCount,
					    uint8_t *rxBuff,
					    size_t readCount );

#endif /* H_SUIT_I2C */
