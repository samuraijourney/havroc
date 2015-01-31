/*
 *  ======== main.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Board Header file */
#include <Board.h>

#include "havroc/communications/suit/suit_i2c.h"
#include "havroc/actuation/motor_actuation.h"

#define MPU_ADDRESS      0x68
#define WHO_AM_I_MPU9250 0x75

Void testFxn(UArg arg0, UArg arg1)
{
	uint8_t txBuff[1];
	int time;
	int motor_index = 0;

	erm_setup(motor_index);

	delay(50);

	// select the RTP control mode
	txBuff[0] = 0x05;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x01, txBuff, 1);
	System_printf("Entered RTP mode");
	delay(50);

	// brake the motor for 1 s
	txBuff[0] = 0x81;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x02, txBuff, 1);
	delay(1000);

	// 100% motor speed time test
	time=millis();
	txBuff[0] = 0x7F;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x02, txBuff, 1);
	System_printf("100 percent speed for 3 seconds");
	while ((millis()-time)<3000){}
	System_printf("Run complete");

	time=millis();
	// brake the motor for 1 s
	System_printf("Now braking");

	// brake the motor for 1 s
	txBuff[0] = 0x81;
	suit_i2c_write(motor_index, DRV2604L_ADDR, 0x02, txBuff, 1);
	delay(1000);
	System_printf("Program complete");
}

/*
 *  ======== main ========
 */
int main(void)
{
	Task_Handle task0;

	task0 = Task_create((Task_FuncPtr)testFxn, NULL, NULL);
	if (task0 == NULL) {
		System_printf("Task create failed.");
	}

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initI2C();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
