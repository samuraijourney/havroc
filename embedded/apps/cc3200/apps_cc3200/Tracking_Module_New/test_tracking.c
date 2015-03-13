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
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Board Header file */
#include <Board.h>
#include <utils.h>

#include "havroc/tracking/Estimation.h"
#include "havroc/tracking/IMU_Math.h"
#include "havroc/tracking/MPU9250_Driver.h"

#include "uart_if.h"
#include "common.h"

IMU test_imu_object;                                      // the IMU object
fusion test_fusion_object;                                // the fusion object

#define OFFSET_COUNT 1000.0

#define delay(ms) UtilsDelay((80000/5)*ms)

Void testFxn(UArg arg0, UArg arg1)
{
	float yaw, pitch, roll;
	double roll_offset = 0, pitch_offset = 0, yaw_offset = 0;
	int count = 0;
	Types_FreqHz freq;
	float now = 0, prev = 0;
	Timestamp_getFreq(&freq);

	NewIMU(&test_imu_object, 0);                        // create the imu object
	IMUInit(&test_imu_object);
	reset(&test_fusion_object);

    while(1)
    {
    	if(count < OFFSET_COUNT)
    	{
    		if(IMURead(&test_imu_object))
			{
				newIMUData(test_imu_object.m_gyro, test_imu_object.m_accel, test_imu_object.m_compass, test_imu_object.m_timestamp, &test_fusion_object);

				roll_offset += test_fusion_object.m_fusionPose.m_data[0] * RAD_TO_DEGREE;
				pitch_offset += test_fusion_object.m_fusionPose.m_data[1] * RAD_TO_DEGREE;
				yaw_offset += test_fusion_object.m_fusionPose.m_data[2] * RAD_TO_DEGREE;
				count++;
				delay(5);
			}

    		if(count == OFFSET_COUNT)
    		{
    			roll_offset/=-OFFSET_COUNT;
    			pitch_offset/=-OFFSET_COUNT;
    			yaw_offset/=-OFFSET_COUNT;
    		}
    	}
    	else
    	{
			if(IMURead(&test_imu_object))
			{
				newIMUData(test_imu_object.m_gyro, test_imu_object.m_accel, test_imu_object.m_compass, test_imu_object.m_timestamp, &test_fusion_object);

				roll = test_fusion_object.m_fusionPose.m_data[0] * RAD_TO_DEGREE + roll_offset;
				pitch = test_fusion_object.m_fusionPose.m_data[1] * RAD_TO_DEGREE + pitch_offset;
				yaw = test_fusion_object.m_fusionPose.m_data[2] * RAD_TO_DEGREE + yaw_offset;

				now = Timestamp_get32()/(1.0*freq.lo);

				Report("roll: %.0f, pitch %.0f, yaw %.0f, timestamp %i \n\r", round(roll), round(pitch), round(yaw), test_imu_object.m_timestamp);

				//Report("roll: %.0f, pitch %.0f, yaw %.0f, timestamp %i, interval %.05f \n\r", round(roll), round(pitch), round(yaw), test_imu_object.m_timestamp, now-prev);

				prev = now;

				delay(5);
			}
    	}
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
