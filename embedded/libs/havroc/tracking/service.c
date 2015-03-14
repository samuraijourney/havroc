#if defined(CCWARE) || defined(TIVAWARE)
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Memory.h>
#endif

#include <havroc/tracking/service.h>
#include "havroc/tracking/Estimation.h"
#include "havroc/tracking/IMU_Math.h"
#include "havroc/tracking/MPU9250_Driver.h"
#include <havroc/eventmgr/eventmgr.h>
#include <havroc/communications/radio/wifi_communication.h>

#define MAX_ATTEMPTS 		100
#define Passed_5ms 			1

static volatile int active = false;
static Event_Handle   Timer_Event;

static IMU imu_object[IMU_ID_MAX];                                      // the IMU object
static fusion fusion_object[IMU_ID_MAX];                                // the fusion object

void TimerISR()
{
	Event_post(Timer_Event, Passed_5ms);
}

void ServiceStart()
{
	//EventRegisterCB(TRACKING_CMD, &(ServiceEnd));

	active = true;

	Timer_Event = Event_create(NULL, NULL);
	Task_Handle task2 = Task_Object_get(NULL, 1);
	Task_setPri(task2, 15);
}

void ServiceEnd()
{
	active = false;
}

int startIMU()
{
	int i;
	for(i = 0; i < 1/*IMU_ID_MAX*/; i++)
	{
		NewIMU(&imu_object[i], i);                        // create the imu object
		IMUInit(&imu_object[i]);
		reset(&fusion_object[i]);
	}

	return SUCCESS;

}

void ServiceRun(void)
{
	int attempts = 0;
	int result;
	UInt events;
	float prev = 0;
	float now = 0;
	Types_FreqHz freq;
	float _r_s_yaw;
	float _r_s_pitch;
	float _r_s_roll;
	float _r_e_yaw;
	float _r_e_pitch;
	float _r_e_roll;
	float _r_w_yaw;
	float _r_w_pitch;
	float _r_w_roll;
	float _l_s_yaw;
	float _l_s_pitch;
	float _l_s_roll;
	float _l_e_yaw;
	float _l_e_pitch;
	float _l_e_roll;
	float _l_w_yaw;
	float _l_w_pitch;
	float _l_w_roll;

	Timestamp_getFreq(&freq);

	result = startIMU();

	if ((result == IMU_MPU_START_FAIL)
			|| (result == IMU_COMPASS_START_FAIL))
	{
		signal(result);
	}

	while(!(isWiFiActive()))
	{
		Task_sleep(5);
	}


	//Report("Service Started\n\r");

	while(active)
	{

		events = Event_pend(Timer_Event, Passed_5ms, Event_Id_NONE, BIOS_WAIT_FOREVER);
		while ((attempts < MAX_ATTEMPTS) && (events & Passed_5ms))
		{
			if (!(!get_r_shoulder_imu(&_r_s_yaw, &_r_s_pitch, &_r_s_roll)
					//&& !get_r_elbow_imu(&_r_e_yaw, &_r_e_pitch, &_r_e_roll)
					//&& !get_r_wrist_imu(&_r_w_yaw, &_r_w_pitch, &_r_w_roll)
					//&& !get_l_shoulder_imu(&_l_e_yaw, &_l_e_pitch, &_l_e_roll)
					//&& !get_l_elbow_imu(&_l_e_yaw, &_l_e_pitch, &_l_e_roll)
					//&& !get_l_wrist_imu(&_l_e_yaw, &_l_e_pitch, &_l_e_roll)))
					))
			{
				attempts++;
			}
			else
			{
				attempts = 0;
				break;
			}
		}

		if (attempts == MAX_ATTEMPTS)
		{
			signal(SERVICE_READ_FAIL);
			ServiceEnd();
		}

		now = Timestamp_get32()/(1.0*freq.lo);

	//	if((now - prev) > 50)
		//{
			ServicePublish(_r_s_yaw, _r_s_pitch, _r_s_roll, _r_e_yaw, _r_e_pitch,
			_r_e_roll, _r_w_yaw, _r_w_pitch, _r_w_roll, _l_s_yaw, _l_s_pitch,
			_l_s_roll, _l_e_yaw, _l_e_pitch, _l_e_roll, _l_w_yaw, _l_w_pitch, _l_w_roll);
		//}

		Report("In Service, interval is %.05f\n\r", now-prev);

		prev = now;

		Task_yield();
	}

	signal(SERVICE_EXIT);
	Task_exit();
}

void ServicePublish(float r_s_yaw, float r_s_pitch, float r_s_roll, float r_e_yaw,
		float r_e_pitch, float r_e_roll, float r_w_yaw, float r_w_pitch, float r_w_roll,
		float l_s_yaw, float l_s_pitch, float l_s_roll, float l_e_yaw,
		float l_e_pitch, float l_e_roll, float l_w_yaw, float l_w_pitch, float l_w_roll)
{
	sendMessage message;

	message.module = TRACKING_MOD;
	message.command = TRACKING_DATA_CMD;
	message.length_high = 0x0;
	message.length_low = 0x48;
	message.data[0] = r_s_yaw;
	message.data[1] = r_s_pitch;
	message.data[2] = r_s_roll;
	message.data[3] = 0;
	message.data[4] = 0;
	message.data[5] = 0;
	message.data[6] = 0;
	message.data[7] = 0;
	message.data[8] = 0;
	message.data[9] = 0;
	message.data[10] = 0;
	message.data[11] = 0;
	message.data[12] = 0;
	message.data[13] = 0;
	message.data[14] = 0;
	message.data[15] = 0;
	message.data[16] = 0;
	message.data[17] = 0;
//	message.data[3] = r_e_yaw;
//	message.data[4] = r_e_pitch;
//	message.data[5] = r_e_roll;
//	message.data[6] = r_w_yaw;
//	message.data[7] = r_w_pitch;
//	message.data[8] = r_w_roll;
//	message.data[9] = l_s_yaw;
//	message.data[10] = l_s_pitch;
//	message.data[11] = l_s_roll;
//	message.data[12] = l_e_yaw;
//	message.data[13] = l_e_pitch;
//	message.data[14] = l_e_roll;
//	message.data[15] = l_w_yaw;
//	message.data[16] = l_w_pitch;
//	message.data[17] = l_w_roll;

	Report("Data being broadcast: Yaw: %.1f, Pitch: %.1f, Roll: %.1f \n\r", message.data[0], message.data[1],message.data[2]);

	while(WiFiSendEnQ(message) != 0)
	{
		Task_yield();
	}
}

int get_r_shoulder_imu(float* yaw, float* pitch, float* roll)
{
	if(IMURead(&imu_object[R_SHOULDER_IMU_ID]))
	{
		newIMUData(imu_object[R_SHOULDER_IMU_ID].m_gyro, imu_object[R_SHOULDER_IMU_ID].m_accel, imu_object[R_SHOULDER_IMU_ID].m_compass, imu_object[R_SHOULDER_IMU_ID].m_timestamp, &fusion_object[R_SHOULDER_IMU_ID]);

		*roll = fusion_object[R_SHOULDER_IMU_ID].m_fusionPose.m_data[0] * RAD_TO_DEGREE;
		*pitch = fusion_object[R_SHOULDER_IMU_ID].m_fusionPose.m_data[1] * RAD_TO_DEGREE;
		*yaw = fusion_object[R_SHOULDER_IMU_ID].m_fusionPose.m_data[2] * RAD_TO_DEGREE;

		return SUCCESS;
	}
	return IMU_MPU_READ_FAIL;
}

int get_l_shoulder_imu(float* yaw, float* pitch, float* roll)
{
	if(IMURead(&imu_object[L_SHOULDER_IMU_ID]))
	{
		newIMUData(imu_object[L_SHOULDER_IMU_ID].m_gyro, imu_object[L_SHOULDER_IMU_ID].m_accel, imu_object[L_SHOULDER_IMU_ID].m_compass, imu_object[L_SHOULDER_IMU_ID].m_timestamp, &fusion_object[L_SHOULDER_IMU_ID]);

		*roll = fusion_object[L_SHOULDER_IMU_ID].m_fusionPose.m_data[0] * RAD_TO_DEGREE;
		*pitch = fusion_object[L_SHOULDER_IMU_ID].m_fusionPose.m_data[1] * RAD_TO_DEGREE;
		*yaw = fusion_object[L_SHOULDER_IMU_ID].m_fusionPose.m_data[2] * RAD_TO_DEGREE;

		return SUCCESS;
	}
	return IMU_MPU_READ_FAIL;
}

int get_r_elbow_imu(float* yaw, float* pitch, float* roll)
{
	if(IMURead(&imu_object[R_ELBOW_IMU_ID]))
	{
		newIMUData(imu_object[R_ELBOW_IMU_ID].m_gyro, imu_object[R_ELBOW_IMU_ID].m_accel, imu_object[R_ELBOW_IMU_ID].m_compass, imu_object[R_ELBOW_IMU_ID].m_timestamp, &fusion_object[R_ELBOW_IMU_ID]);

		*roll = fusion_object[R_ELBOW_IMU_ID].m_fusionPose.m_data[0] * RAD_TO_DEGREE;
		*pitch = fusion_object[R_ELBOW_IMU_ID].m_fusionPose.m_data[1] * RAD_TO_DEGREE;
		*yaw = fusion_object[R_ELBOW_IMU_ID].m_fusionPose.m_data[2] * RAD_TO_DEGREE;
		return SUCCESS;
	}
	return IMU_MPU_READ_FAIL;
}

int get_l_elbow_imu(float* yaw, float* pitch, float* roll)
{
	if(IMURead(&imu_object[L_ELBOW_IMU_ID]))
	{
		newIMUData(imu_object[L_ELBOW_IMU_ID].m_gyro, imu_object[L_ELBOW_IMU_ID].m_accel, imu_object[L_ELBOW_IMU_ID].m_compass, imu_object[L_ELBOW_IMU_ID].m_timestamp, &fusion_object[L_ELBOW_IMU_ID]);

		*roll = fusion_object[L_ELBOW_IMU_ID].m_fusionPose.m_data[0] * RAD_TO_DEGREE;
		*pitch = fusion_object[L_ELBOW_IMU_ID].m_fusionPose.m_data[1] * RAD_TO_DEGREE;
		*yaw = fusion_object[L_ELBOW_IMU_ID].m_fusionPose.m_data[2] * RAD_TO_DEGREE;
		return SUCCESS;
	}
	return IMU_MPU_READ_FAIL;
}

int get_r_wrist_imu(float* yaw, float* pitch, float* roll)
{
	if(IMURead(&imu_object[R_WRIST_IMU_ID]))
	{
		newIMUData(imu_object[R_WRIST_IMU_ID].m_gyro, imu_object[R_WRIST_IMU_ID].m_accel, imu_object[R_WRIST_IMU_ID].m_compass, imu_object[R_WRIST_IMU_ID].m_timestamp, &fusion_object[R_WRIST_IMU_ID]);

		*roll = fusion_object[R_WRIST_IMU_ID].m_fusionPose.m_data[0] * RAD_TO_DEGREE;
		*pitch = fusion_object[R_WRIST_IMU_ID].m_fusionPose.m_data[1] * RAD_TO_DEGREE;
		*yaw = fusion_object[R_WRIST_IMU_ID].m_fusionPose.m_data[2] * RAD_TO_DEGREE;
		return SUCCESS;
	}
	return IMU_MPU_READ_FAIL;
}

int get_l_wrist_imu(float* yaw, float* pitch, float* roll)
{
	if(IMURead(&imu_object[L_WRIST_IMU_ID]))
	{
		newIMUData(imu_object[L_WRIST_IMU_ID].m_gyro, imu_object[L_WRIST_IMU_ID].m_accel, imu_object[L_WRIST_IMU_ID].m_compass, imu_object[L_WRIST_IMU_ID].m_timestamp, &fusion_object[L_WRIST_IMU_ID]);

		*roll = fusion_object[L_WRIST_IMU_ID].m_fusionPose.m_data[0] * RAD_TO_DEGREE;
		*pitch = fusion_object[L_WRIST_IMU_ID].m_fusionPose.m_data[1] * RAD_TO_DEGREE;
		*yaw = fusion_object[L_WRIST_IMU_ID].m_fusionPose.m_data[2] * RAD_TO_DEGREE;
		return SUCCESS;
	}
	return IMU_MPU_READ_FAIL;
}
