#if defined(CCWARE) || defined(TIVAWARE)
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
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
static Clock_Handle   clkHandle;

static IMU imu_object[IMU_ID_MAX];                                      // the IMU object
static fusion fusion_object[IMU_ID_MAX];                                // the fusion object

void PostTimer()
{
	Event_post(Timer_Event, Passed_5ms);

	Task_yield();
}

int ServiceStart()
{
	Clock_Params   clockParams;
	Task_Handle    task0;
	Task_Params    params;

	//EventRegisterCB(TRACKING_CMD, &(ServiceEnd));

	active = true;

	Clock_Params_init(&clockParams);
	clockParams.period = 10;/* every 4 Clock ticks */
	clockParams.startFlag = FALSE;/* start immediately */
	clkHandle = Clock_create((Clock_FuncPtr)PostTimer, 1, &clockParams, NULL);

	Task_Params_init(&params);
	params.instance->name = "TrackingBroadcast_Task";
	params.priority = 15;

	Timer_Event = Event_create(NULL, NULL);

	task0 = Task_create((Task_FuncPtr) ServiceRun, &params, NULL);
	if (task0 == NULL || clkHandle == NULL || Timer_Event == NULL)
	{
		signal(SERVICE_START_FAIL);
		return SERVICE_START_FAIL;
	}

	UART_PRINT("Started service task successfully \n\r");

	return SUCCESS;
}

int ServiceStart_Raw()
{
	Task_Handle    task0;
	Task_Params    params;

	//EventRegisterCB(TRACKING_CMD, &(ServiceEnd));

	active = true;

	Task_Params_init(&params);
	params.instance->name = "TrackingBroadcast_Task";
	params.priority = 10;

	task0 = Task_create((Task_FuncPtr) ServiceRun_Raw, &params, NULL);
	if (task0 == NULL)
	{
		signal(SERVICE_START_FAIL);
		return SERVICE_START_FAIL;
	}

	UART_PRINT("Started service task successfully \n\r");

	return SUCCESS;
}

void ServiceEnd()
{
	active = false;
}

void ServiceRun_Raw(void)
{
	int attempts = 0;
	int result;
	float prev = 0;
	float now = 0;
	Types_FreqHz freq;

	Timestamp_getFreq(&freq);

	result = startIMU_Raw(R_SHOULDER_IMU_ID);

	InitTerm();

	if ((result == IMU_MPU_START_FAIL)
			|| (result == IMU_COMPASS_START_FAIL))
	{
		signal(result);
	}

	while(!(isWiFiActive()))
	{
		Task_sleep(10);
	}

	while(active)
	{
		prev = Timestamp_get32()/(1.0*freq.lo);

		System_printf("Innnnnnnnnnn Tracking Broadcaster\n");
		System_flush();

		while ((attempts < MAX_ATTEMPTS))
		{
			if (IMURead(&imu_object[R_WRIST_IMU_ID]))
			{
				attempts = 0;
				break;
			}
			else
			{
				attempts++;
			}
		}

		if (attempts == MAX_ATTEMPTS)
		{
			signal(SERVICE_READ_FAIL);
			ServiceEnd();
		}

		ServicePublish_Raw(imu_object[R_WRIST_IMU_ID].m_accel.m_data[0],imu_object[R_WRIST_IMU_ID].m_accel.m_data[1], imu_object[R_WRIST_IMU_ID].m_accel.m_data[2],
				imu_object[R_WRIST_IMU_ID].m_gyro.m_data[0],imu_object[R_WRIST_IMU_ID].m_gyro.m_data[1], imu_object[R_WRIST_IMU_ID].m_gyro.m_data[2],
				imu_object[R_WRIST_IMU_ID].m_compass.m_data[0], imu_object[R_WRIST_IMU_ID].m_compass.m_data[1], imu_object[R_WRIST_IMU_ID].m_compass.m_data[2]);

		now = Timestamp_get32()/(1.0*freq.lo);

		System_printf("This is Task 3 - Elapsed Time is %.04f\n", now-prev);
		System_flush();

		Task_yield();
	}

	signal(SERVICE_EXIT);
	Task_exit();
}

int startIMU()
{
	int i;
	for(i = 0; i < IMU_ID_MAX; i++)
	{
		NewIMU(&imu_object[i], i);                        // create the imu object
		IMUInit(&imu_object[i]);
		reset(&fusion_object[i]);
	}

	return SUCCESS;

}

int startIMU_Raw(int imu_index)
{
	NewIMU(&imu_object[imu_index], imu_index);                        // create the imu object
	IMUInit(&imu_object[imu_index]);
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

	InitTerm();

	if ((result == IMU_MPU_START_FAIL)
			|| (result == IMU_COMPASS_START_FAIL))
	{
		signal(result);
	}

	while(!(isWiFiActive()))
	{
		Task_sleep(10);
	}

	Clock_start(clkHandle);
	System_printf("Service Started\n");
	System_flush();

	while(active)
	{
		prev = Timestamp_get32()/(1.0*freq.lo);

		System_printf("Innnnnnnnnnn Tracking Broadcaster\n");
		System_flush();
		events = Event_pend(Timer_Event, Passed_5ms, Event_Id_NONE, BIOS_WAIT_FOREVER);

		System_printf("Innnnnnnnnnnnnnn Tracking Broadcaster - Event Received\n");
		System_flush();

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

		ServicePublish(_r_s_yaw, _r_s_pitch, _r_s_roll, _r_e_yaw, _r_e_pitch,
			_r_e_roll, _r_w_yaw, _r_w_pitch, _r_w_roll, _l_s_yaw, _l_s_pitch,
			_l_s_roll, _l_e_yaw, _l_e_pitch, _l_e_roll, _l_w_yaw, _l_w_pitch, _l_w_roll);

		now = Timestamp_get32()/(1.0*freq.lo);

		System_printf("This is Task 3 - Elapsed Time is %.04f\n", now-prev);
		System_flush();

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

	message.module = TRACKING_CMD;
	message.command = DATA_CMD;
	message.length = 0x24;
	message.data = (float*)malloc(sizeof(float)*message.length);
	message.data[0] = r_s_yaw;
	message.data[1] = r_s_pitch;
	message.data[2] = r_s_roll;
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

	UART_PRINT("Data being broadcast: Yaw: %.1f, Pitch: %.1f, Yaw: %.1f \n\r", message.data[0], message.data[1],
			message.data[2]);

	while(WiFiSendEnQ(message) != 0)
	{
		Task_yield();
	}

	free(message.data);
}

void ServicePublish_Raw(float accelX, float accelY, float accelZ, float gyroX,
		float gyroY, float gyroZ, float magX, float magY, float magZ)
{
	sendMessage message;

	message.command = TRACKING_CMD;
	message.length = 0x24;
	message.data = (float*)malloc(sizeof(float)*message.length);
	message.data[0] = accelX;
	message.data[1] = accelY;
	message.data[2] = accelZ;
	message.data[3] = gyroX;
	message.data[4] = gyroY;
	message.data[5] = gyroZ;
	message.data[6] = magX;
	message.data[7] = magY;
	message.data[8] = magZ;

	UART_PRINT("Data being broadcast: AccelX: %.1f, AccelY: %.1f, AccelZ: %.1f GyroX: %.1f, GyroY: %.1f, GyroZ: %.1f MagX: %.1f, MagY: %.1f, MagZ: %.1f \n\r", message.data[0], message.data[1],
			message.data[2], message.data[3], message.data[4], message.data[5], message.data[6], message.data[7], message.data[8]);

	while(WiFiSendEnQ(message) != 0)
	{
		Task_yield();
	}

	free(message.data);
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
