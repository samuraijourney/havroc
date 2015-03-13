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

#define MUX_ADDR			0x77

Void testFxn(UArg arg0, UArg arg1)
{
	uint8_t readBuff[1], writeBuff[3];
	readBuff[0] = 0;

	int i;
	for(i = 0; i < 5; i++)
	{
		writeBuff[0] = 1 << i;
		suit_i2c_transfer(MUX_ADDR, writeBuff, 1, NULL, 0);
		suit_i2c_read(0x5A, 0, readBuff, 1);
		System_printf("Motor%d Status: 0x%x\n",i ,readBuff[0]);
		readBuff[0] = 0xff;
	}

	System_flush();
}

/*
 *  ======== main ========
 */
int main(void)
{
	Task_Handle task0;
	Task_Params params;

	Task_Params_init(&params);
	params.priority = 5;
	task0 = Task_create((Task_FuncPtr)testFxn, &params, NULL);
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
