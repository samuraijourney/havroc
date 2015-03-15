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
#include "havroc/communications/radio/wifi_communication.h"
#include "havroc/communications/suit/suit_i2c.h"

static void BoardInit(void)
{
    // Enable Processor
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

/*
 *  ======== main ========
 */
int main(void)
{
	/* Call board init functions. */
	Board_initGeneral();

    // Board Initialization
	BoardInit();

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

	WlanStartTask();
	//EventStart();
	//ServiceStart();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
