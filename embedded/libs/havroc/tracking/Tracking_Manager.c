#include <havroc/tracking/Tracking_Manager.h>
#include "havroc/tracking/Estimation.h"
#include "havroc/tracking/MPU9250_Driver.h"
#include <havroc/communications/radio/wifi_communication.h>
#include <havroc/id.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/knl/Clock.h>

#include "uart_if.h"
#include "common.h"

static IMU imu_object[IMU_ID_MAX];                                      // the IMU object
static fusion fusion_object[IMU_ID_MAX];                                // the fusion object
static float yaw[IMU_ID_MAX] = {0, 0, 0, 0, 0, 0};
static float pitch[IMU_ID_MAX] = {0, 0, 0, 0, 0, 0};
static float roll[IMU_ID_MAX] = {0, 0, 0, 0, 0, 0};


static unsigned long millis ()
{
	Types_FreqHz freq;

	Timestamp_getFreq(&freq);

	return (Timestamp_get32()*1000.0/(1.0*freq.lo));
}

void Setup_IMUs(int imu_start, int count)
{
	int i;
	int retVal;

	for(i = imu_start; i < count; i++)
	{
		NewIMU(&imu_object[i], i);                        // create the imu object
		retVal = IMUInit(&imu_object[i]);
		if(retVal != SUCCESS)
			Report("Failed to start IMU %i, error code %i \n\r", i, retVal);
		reset(&fusion_object[i]);
	}
}

#define delay(ms) UtilsDelay((80000/5)*ms)


void Tracking_Update(int imu_start, int count)
{
	int i;
	//static unsigned long prevTimestamp[3] = {0,0,0};
	//unsigned long now = 0;

	for(i = imu_start; i < count; i++)
	{
		if(IMURead(&imu_object[i]))
		{
			newIMUData(imu_object[i].m_gyro, imu_object[i].m_accel, imu_object[i].m_compass, imu_object[i].m_timestamp, &fusion_object[i]);

			roll[i] = fusion_object[i].m_fusionPose.m_data[0] * RAD_TO_DEGREE;
			pitch[i] = fusion_object[i].m_fusionPose.m_data[1] * RAD_TO_DEGREE;
			yaw[i] = fusion_object[i].m_fusionPose.m_data[2] * RAD_TO_DEGREE;
			//now = millis();

//			if(i == 0)
//				Report("IMU index: %i, roll: %.0f, pitch %.0f, yaw %.0f, timestamp dif %i \n\r", i, round(roll[i]), round(pitch[i]), round(yaw[i]), now - prevTimestamp[i]);

			//prevTimestamp[i] = now;
			//delay(5);
			//Task_sleep(50);
		}
//		else
//		{
//			//Report("Skipped imu update \n\r");
//		}
	}
}

void Tracking_Publish()
{
	sendMessage message;

	message.module = TRACKING_MOD;
	message.command = TRACKING_DATA_CMD;
	message.length = ((((uint16_t)0x00) << 8) & 0xFF00) | (((uint16_t)0x19) & 0x00FF);
	message.arm = RIGHT_ARM;
	message.data[0] = yaw[0];
	message.data[1] = pitch[0];
	message.data[2] = roll[0];
	message.data[3] = yaw[1];
	message.data[4] = pitch[1];
	message.data[5] = roll[1];

	Report("Shoulder: Yaw: %.0f, Pitch: %.0f, Roll: %.0f \n\r", message.data[0], message.data[1],message.data[2]);
	Report("Elbow:    Yaw: %.0f, Pitch: %.0f, Roll: %.0f \n\r", message.data[3], message.data[4],message.data[5]);
	//Report("Wrist:    Yaw: %.0f, Pitch: %.0f, Roll: %.0f \n\r", message.data[6], message.data[7],message.data[8]);


	WiFiSendEnQ(message);
}
