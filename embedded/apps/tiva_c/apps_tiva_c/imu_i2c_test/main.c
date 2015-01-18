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

#include <havroc/communications/suit/suit_i2c.h>

#define MPU_ADDRESS      0x68
#define WHO_AM_I_MPU9250 0x75


Void testFxn(UArg arg0, UArg arg1)
{
	uint8_t txBuff[1];
	uint8_t rxBuff[1];

	txBuff[0] = WHO_AM_I_MPU9250;
	rxBuff[0] = 0;

	suit_i2c_transfer(EK_TM4C123GXL_I2C3, MPU_ADDRESS, txBuff, 1, rxBuff, 1);

	System_printf("%d", rxBuff[0]);
	System_flush();
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
