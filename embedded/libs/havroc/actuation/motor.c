
/* Standard C Includes */
#include "stdbool.h"

/* HaVRoc Library Includes */
#include "havroc/actuation/motor.h"


#define DRV2604L_ADDR 			0x5A
#define MOTOR_RTP_REG			0x02

void motor_run(uint8_t intensity)
{
	uint8_t writeBuff[1];

	//write MODE to do internal trigger
	writeBuff[0] = 0x05;
	suit_i2c_write(DRV2604L_ADDR, 0x01, writeBuff, 1);

	writeBuff[0] = intensity;
	suit_i2c_write(DRV2604L_ADDR, MOTOR_RTP_REG, writeBuff, 1);
}

void motor_runWaveform()
{
	uint8_t writeBuff[1];

	//write MODE to do internal trigger
	writeBuff[0] = 0x00;
	suit_i2c_write(DRV2604L_ADDR, 0x01, writeBuff, 1);

	//write to waveform seq to select effect 1
	writeBuff[0] = 0x01;
	suit_i2c_write(DRV2604L_ADDR, 0x04, writeBuff, 1);

	//write GO bit
	writeBuff[0] = 0x01;
	suit_i2c_write(DRV2604L_ADDR, 0x0C, writeBuff, 1);
}

void motor_init()
{
	uint8_t readBuff[1];
	uint8_t writeBuff[10];

	suit_i2c_read(DRV2604L_ADDR, 0x00, readBuff, 1);

	// bring motor out of standby
	writeBuff[0] = 0x00;
	suit_i2c_write(DRV2604L_ADDR, 0x01, writeBuff, 1);

	// specify actuator specific data - select ERM
	writeBuff[0] = 0x3A;
	suit_i2c_write(DRV2604L_ADDR, 0x1A, writeBuff, 1);

	// pg 26 step 3d-motor voltage set at 3V here
	writeBuff[0] = 0x8E;
	suit_i2c_write(DRV2604L_ADDR, 0x16, writeBuff, 1);

	// pg 26 step 3e- max motor voltage set at 3.5 V here
	writeBuff[0] = 0xA2;
	suit_i2c_write(DRV2604L_ADDR, 0x17, writeBuff, 1);

	//set CTL3 reg to do ERM Open Loop, Unsigned speed values
	writeBuff[0] = 0x28;
	suit_i2c_write(DRV2604L_ADDR, 0x1D, writeBuff, 1);

	//set Mode reg to read from RTP
	writeBuff[0] = 0x05;
	suit_i2c_write(DRV2604L_ADDR, 0x01, writeBuff, 1);

	/*Write waveform to RAM*/
	//write MODE to do internal trigger
	writeBuff[0] = 0x00;
	suit_i2c_write(DRV2604L_ADDR, 0x01, writeBuff, 1);

	//select RAM lower byte
	writeBuff[0] = 0;
	suit_i2c_write(DRV2604L_ADDR, 0xFE, writeBuff, 1);

	//write RAM data
	//select RAM data input reg
	writeBuff[0] = 0xFF;

	//revision
	writeBuff[1] = 0x0;

	//header
	writeBuff[2] = 0x0; //upper start
	writeBuff[3] = 0x4; //lower start
	writeBuff[4] = 0x2; //number of data bytes

	//data
	writeBuff[5] = 0x3F; //max speed
	writeBuff[6] = 0x28; //200ms
	writeBuff[7] = 0x00; //stop motor
	writeBuff[8] = 0x28; //same time

	//multi write to RAM
	suit_i2c_transfer(DRV2604L_ADDR, writeBuff, 7, NULL, 0);
}

