#include <havroc/tracking/Tracking_Manager.h>
#include "havroc/tracking/Estimation.h"
#include "havroc/tracking/MPU9250_Driver.h"
#include <havroc/communications/radio/wifi_communication.h>
#include <havroc/id.h>

#include "uart_if.h"
#include "common.h"

static IMU imu_object[IMU_ID_MAX];                                      // the IMU object
static fusion fusion_object[IMU_ID_MAX];                                // the fusion object
static float yaw[IMU_ID_MAX] = {0, 0, 0, 0, 0, 0};
static float pitch[IMU_ID_MAX] = {0, 0, 0, 0, 0, 0};
static float roll[IMU_ID_MAX] = {0, 0, 0, 0, 0, 0};

void Setup_IMUs(int imu_index)
{
	int i;
	for(i = R_SHOULDER_IMU_ID; i < imu_index+1; i++)
	{
		NewIMU(&imu_object[i], 0);                        // create the imu object
		IMUInit(&imu_object[i]);
		reset(&fusion_object[i]);
	}
}

void Tracking_Update(int imu_index)
{
	int i;
	for(i = R_SHOULDER_IMU_ID; i < imu_index+1; i++)
	{
		if(IMURead(&imu_object[i]))
		{
			newIMUData(imu_object[i].m_gyro, imu_object[i].m_accel, imu_object[i].m_compass, imu_object[i].m_timestamp, &fusion_object[i]);

			roll[i] = fusion_object[i].m_fusionPose.m_data[0] * RAD_TO_DEGREE;
			pitch[i] = fusion_object[i].m_fusionPose.m_data[1] * RAD_TO_DEGREE;
			yaw[i] = fusion_object[i].m_fusionPose.m_data[2] * RAD_TO_DEGREE;
		}
	}
}

void Tracking_Publish()
{
	sendMessage message;

	message.module = TRACKING_MOD;
	message.command = TRACKING_DATA_CMD;
	message.length = ((((uint16_t)0x00) << 8) & 0xFF00) | (((uint16_t)0x48) & 0x00FF);
	message.data[0] = yaw[0];
	message.data[1] = pitch[0];
	message.data[2] = roll[0];
	message.data[3] = yaw[1];
	message.data[4] = pitch[1];
	message.data[5] = roll[1];
	message.data[6] = yaw[2];
	message.data[7] = pitch[2];
	message.data[8] = roll[2];
	message.data[9] = yaw[3];
	message.data[10] = pitch[3];
	message.data[11] = roll[3];
	message.data[12] = yaw[4];
	message.data[13] = pitch[4];
	message.data[14] = roll[4];
	message.data[15] = yaw[5];
	message.data[16] = pitch[5];
	message.data[17] = roll[5];

	Report("Data being broadcast: Yaw: %.0f, Pitch: %.0f, Roll: %.0f \n\r", message.data[0], message.data[1],message.data[2]);

	WiFiSendEnQ(message);
}
