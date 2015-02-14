#include "havroc/communications/suit/suit_i2c.h"
#include "havroc/actuation/motor_actuation.h"
#include <xdc/runtime/System.h>


unsigned long time;

void erm_setup(int motor_index)
{
	uint8_t rxBuff[1];
	uint8_t txBuff[1];

	// initial reading to see if driver is accessible
	delay(70);

	suit_i2c_read(motor_index, DRV2604L_ADDR, 0x00, rxBuff, 1);

	delay(250);
	// testing complete, should output 192

	// put in standby mode to begin programming
	txBuff[0] = 0x07;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x01, txBuff, 1);

	delay(50);

	// specify actuator specific data, this is pg 26 step 3a, b and c
	txBuff[0] = 0x2A;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x1A, txBuff, 1);
	delay(50);
	// pg 26 step 3d-motor voltage set at 3V here

	txBuff[0] = 0x8E;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x16, txBuff, 1);
	delay(50);

	// pg 26 step 3e- max motor voltage set at 3.5 V here
	txBuff[0] = 0xA2;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x17, txBuff, 1);
	delay(50);

	// pg 26 step 3g- setting drive time, 2.4 ms for ERM
	txBuff[0] = 0x93;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x1B, txBuff, 1);
	delay(50);

	// pg 26 step 3h, i and j
	txBuff[0] = 0xF5;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x1C, txBuff, 1);
	delay(50);

	// pg 26 step 3f and k
	txBuff[0] = 0x30;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x1E, txBuff, 1);

	delay(50);
	// pg 26 step 4, setting the go bit to 1 to start auto calibration
	txBuff[0] = 0x01;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x0C, txBuff, 1);

	delay(70);

	// pg 26 step 5, check results of auto calibration
	suit_i2c_read(motor_index, DRV2604L_ADDR, 0x00, rxBuff, 1);
}
