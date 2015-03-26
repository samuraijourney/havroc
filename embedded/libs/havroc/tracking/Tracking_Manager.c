
/* HaVRoc Library Includes */
#include <havroc/tracking/Tracking_Manager.h>
#include "havroc/tracking/Estimation.h"
#include "havroc/tracking/MPU9250_Driver.h"
#include <havroc/communications/radio/wifi_communication.h>
#include <havroc/id.h>
#include <havroc/havroc_utils/havrocutils.h>

static IMU imu_object[2];                                      // the IMU object
static fusion fusion_object[2];                                // the fusion object
static float yaw[2] = {0, 0};
static float pitch[2] = {0, 0};
static float roll[2] = {0, 0};
static Quaternion qPose[2];
static uint8_t m_board_arm;

uint8_t Setup_IMUs(uint8_t* imu_select, uint8_t count, uint8_t board_arm)
{
	uint8_t i;
	uint8_t imu_index;
	uint8_t retVal;

	m_board_arm = board_arm;

	for(i = 0; i < count; i++)
	{
		imu_index = imu_select[i];
		NewIMU(&imu_object[i], imu_index);                        // create the imu object
		retVal = IMUInit(&imu_object[i]);

		if(retVal != SUCCESS)
		{
			Report("Failed to start IMU %i, error code %i \n\r", imu_index, retVal);
			return retVal;
		}

		reset(&fusion_object[i]);
	}

	return SUCCESS;
}

void Tracking_Update(uint8_t count)
{
	uint8_t i;
	float now = 0;
	float before = 0;
	uint8_t iteration = 0;
	static float prevTimestamp[IMU_ID_MAX] = {0,0};

	for(i = 0; i < count; i++)
	{
		if(IMURead(&imu_object[i]))
		{
			before = millis();
			newIMUData(imu_object[i].m_gyro, imu_object[i].m_accel, imu_object[i].m_compass, imu_object[i].m_timestamp, &fusion_object[i]);

			roll[i] = fusion_object[i].m_fusionPose.m_data[0] * RAD_TO_DEGREE;
			pitch[i] = fusion_object[i].m_fusionPose.m_data[1] * RAD_TO_DEGREE;
			yaw[i] = fusion_object[i].m_fusionPose.m_data[2] * RAD_TO_DEGREE;

			qPose[i].m_data[0] = fusion_object[i].m_fusionQPose.m_data[0];
			qPose[i].m_data[1] = fusion_object[i].m_fusionQPose.m_data[1];
			qPose[i].m_data[2] = fusion_object[i].m_fusionQPose.m_data[2];
			qPose[i].m_data[3] = fusion_object[i].m_fusionQPose.m_data[3];

			now = millis();

			if(i == 0 && iteration == 0)
			{
				//Report("Shoulder:  roll: %.1f, pitch %.1f, yaw %.1f, timestamp dif %.3f, proc time %.3f \n\r", i, round(roll[i]), round(pitch[i]), round(yaw[i]), now - prevTimestamp[i], now-before);
//				Report("Shoulder:  scalar: %.5f, x: %.5f, y %.5f, z %.5f, timestamp dif %.3f, proc time %.3f \n\r", (qPose[i].m_data[0]), (qPose[i].m_data[1]), (qPose[i].m_data[2]), (qPose[i].m_data[3]), now - prevTimestamp[i], now-before);
			}
			if(i == 1 && iteration == 0)
			{
				//Report("Elbow:     roll: %.1f, pitch %.1f, yaw %.1f, timestamp dif %.3f \n\r", round(roll[i]), round(pitch[i]), round(yaw[i]), now - prevTimestamp[i]);
			}
			iteration = (iteration + 1) % 50;
			prevTimestamp[i] = now;
		}
		else
		{
			//delay(2);
			//Report("Skipped imu update \n\r");
		}
	}
}

void Tracking_Update_New(uint8_t count)
{
	uint8_t i;
	float now = 0;
	float before = 0;
	uint8_t iteration = 0;
	bool result[IMU_ID_MAX] = {false, false};
	static float prevTimestamp[IMU_ID_MAX] = {0,0};

	for(i = 0; i < count; i++)
	{
		result[i] = IMURead(&imu_object[i]);
	}

	for(i = 0; i < count; i++)
	{
		if(result[i])
		{
//			before = millis();

			newIMUData(imu_object[i].m_gyro, imu_object[i].m_accel, imu_object[i].m_compass, imu_object[i].m_timestamp, &fusion_object[i]);

			roll[i] = fusion_object[i].m_fusionPose.m_data[0] * RAD_TO_DEGREE;
			pitch[i] = fusion_object[i].m_fusionPose.m_data[1] * RAD_TO_DEGREE;
			yaw[i] = fusion_object[i].m_fusionPose.m_data[2] * RAD_TO_DEGREE;

			qPose[i].m_data[0] = fusion_object[i].m_fusionQPose.m_data[0];
			qPose[i].m_data[1] = fusion_object[i].m_fusionQPose.m_data[1];
			qPose[i].m_data[2] = fusion_object[i].m_fusionQPose.m_data[2];
			qPose[i].m_data[3] = fusion_object[i].m_fusionQPose.m_data[3];

			//Report("Shoulder:  accel x: %.1f, accel y %.1f, accel z %.1f\n\r", (imu_object[i].m_accel.m_data[0]), (imu_object[i].m_accel.m_data[1]), (imu_object[i].m_accel.m_data[2]));

			//Report("Shoulder:  gyro x: %.1f, gyro y %.1f, gyro z %.1f\n\r", (imu_object[i].m_gyro.m_data[0]), (imu_object[i].m_gyro.m_data[1]), (imu_object[i].m_gyro.m_data[2]));

			//Report("Shoulder:  compass x: %.1f, compass y %.1f, compass z %.1f\n\r", (imu_object[i].m_compass.m_data[0]), (imu_object[i].m_compass.m_data[1]), (imu_object[i].m_compass.m_data[2]));


//			now = millis();

			if(i == 0 && iteration == 0)
			{
				//Report("Shoulder:  roll: %.1f, pitch %.1f, yaw %.1f, timestamp dif %.3f, proc time %.3f \n\r", i, round(roll[i]), round(pitch[i]), round(yaw[i]), now - prevTimestamp[i], now-before);
////				Report("Shoulder:  scalar: %.5f, x: %.5f, y %.5f, z %.5f, timestamp dif %.3f, proc time %.3f \n\r", (qPose[i].m_data[0]), (qPose[i].m_data[1]), (qPose[i].m_data[2]), (qPose[i].m_data[3]), now - prevTimestamp[i], now-before);
			}
			if(i == 1 && iteration == 0)
			{
				//Report("Elbow:     roll: %.1f, pitch %.1f, yaw %.1f, timestamp dif %.3f \n\r", round(roll[i]), round(pitch[i]), round(yaw[i]), now - prevTimestamp[i]);
			}
//			iteration = (iteration + 1) % 50;
//			prevTimestamp[i] = now;
		}
		else
		{
			//delay(2);
//			Report("Skipped imu update \n\r");
		}

		result[i] = false;
	}
}

void Tracking_Publish()
{
	sendMessage message;

	message.module = TRACKING_MOD;
	message.command = TRACKING_DATA_CMD;
	message.length = ((((uint16_t)0x00) << 8) & 0xFF00) | (((uint16_t)0x19) & 0x00FF);
	message.arm = m_board_arm;

	message.data[0] = yaw[0];
	message.data[1] = pitch[0];
	message.data[2] = roll[0];
	message.data[3] = yaw[1];
	message.data[4] = pitch[1];
	message.data[5] = roll[1];

	WiFiSendEnQ(message);
}

void Tracking_Publish_Quaternion()
{
	sendMessage message;

	message.module = TRACKING_MOD;
	message.command = TRACKING_DATA_CMD;
	message.length = ((((uint16_t)0x00) << 8) & 0xFF00) | (((uint16_t)0x21) & 0x00FF);
	message.arm = m_board_arm;

	message.data[0] = qPose[0].m_data[0];
	message.data[1] = qPose[0].m_data[1];
	message.data[2] = qPose[0].m_data[2];
	message.data[3] = qPose[0].m_data[3];
	message.data[4] = qPose[1].m_data[0];
	message.data[5] = qPose[1].m_data[1];
	message.data[6] = qPose[1].m_data[2];
	message.data[7] = qPose[1].m_data[3];

	WiFiSendEnQ(message);
}

void Tracking_Publish_Error(uint8_t error_code)
{
	sendMessage message;

	message.module = TRACKING_MOD;
	message.command = ERROR_CMD;
	message.length = ((((uint16_t)0x00) << 8) & 0xFF00) | (((uint16_t)0x05) & 0x00FF);
	message.arm = m_board_arm;
	message.data[0] = error_code;

	WiFiSendEnQ(message);
}
