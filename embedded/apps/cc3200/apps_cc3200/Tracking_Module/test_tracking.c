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

#include "havroc/tracking/imu_driver.h"
#include "havroc/tracking/estimation.h"

#define MPU_ADDRESS      0x68
#define WHO_AM_I_MPU9250 0x75

Void testFxn(UArg arg0, UArg arg1)
{
	float yaw, pitch, roll;
	int count = 0;

	int result = startIMU();

	while(1)
	{
		result = returnEstimate(0, &yaw, &pitch, &roll);
	//	if(count%100==0)
	//	{
			System_printf("Yaw: %i, Pitch: %i, Roll: %i\n", yaw, pitch, roll);
			System_flush();
	//	}
		//count++;
	}
}

/*
 *  ======== main ========
 */
int main(void)
{
	Task_Handle task0;

	task0 = Task_create((Task_FuncPtr)testFxn, NULL, NULL);
	if (task0 == NULL) {
		System_printf("Task create failed.\n");
		System_flush();
	}

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initI2C();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
