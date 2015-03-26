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

#include "uart_if.h"

/* HaVRoC library files */
#include "havroc/communications/suit/suit_i2c.h"
#include "havroc/actuation/motor.h"
#include "havroc/havroc_utils/havrocutils.h"
#include "havroc/communications/suit/suit_net_manager.h"

Void TestFxn()
{
	char buff[5];

	suitNetManager_boardTest(0x77, 8);
	Report("\n\rEnter to start motor test...");
	GetCmd(buff, 5);

	int i;
	for (i = 0; i < 8; i++)
	{
		Report("Testing motor %i...", i);
		suitNetManager_boardMotorTest(i);
		delay(300);
		Report("DONE!");
		GetCmd(buff, 5);
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

	Task_Handle task0;

	task0 = Task_create((Task_FuncPtr)TestFxn, NULL, NULL);
	if (task0 == NULL) {
		System_printf("Task create failed.\n");
		System_flush();
	}

    /* Start BIOS */
    BIOS_start();

    return (0);
}
