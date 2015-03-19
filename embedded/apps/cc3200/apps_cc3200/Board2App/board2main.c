/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>

/* Example/Board Header files */
#include <Board.h>
#include "uart_if.h"
#include "common.h"

/* HaVRoC library files */
#include "havroc/communications/radio/wifi_communication.h"
#include "havroc/eventmgr/eventmgr.h"
#include "havroc/communications/suit/suit_i2c.h"

static void BoardInit(void)
{
    // Enable Processor
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

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

	// Initialize I2C (suit_i2c module)
    if (suit_i2c_init() != SUIT_I2C_E_SUCCESS)
    {
    	Report("I2C init error!\n");
    	System_flush();
    }

	WlanStartTask();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
