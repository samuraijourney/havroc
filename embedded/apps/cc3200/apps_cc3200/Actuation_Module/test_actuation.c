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

/* HaVRoC library files */
#include "havroc/communications/suit/suit_i2c.h"
#include "havroc/actuation/motor.h"

extern Motor motors[];

Void TestFxn()
{
	uint8_t buff[1];
	int i;
	suitNetManager_boardTest(0x77, 8);
	Report("Press enter to start motor test...");
	GetCmd(buff, 10);
	for (i = 1; i < 3; i++)
	{
		Report("Testing %d", i);
		suitNetManager_boardMotorTest(i);
		Report("...DONE");
		GetCmd(buff, 10);
	}
}

/*
 *  ======== main ========
 */
int main(void)
{
	/* Call board init functions. */
	Board_initGeneral();

	// Initialize the uDMA
	UDMAInit();

	// Configuring UART
	InitTerm();

	// Initialize I2C
    if (suit_i2c_init() != SUIT_I2C_E_SUCCESS)
    {
    	Report("I2C init error!\n");
    	System_flush();
    }

    Task_setPri(Test_Task, 5);

    /* Start BIOS */
    BIOS_start();

    return (0);
}
