#if defined(CCWARE) || defined(TIVAWARE)
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#endif

#include <havroc/tracking/service.h>
#include <havroc/tracking/estimation.h>
#include <havroc/eventmgr/eventmgr.h>
#include <havroc/communications/radio/wifi_communication.h>

#define MAX_ATTEMPTS 100

static float _s_yaw;
static float _s_pitch;
static float _s_roll;
static float _e_yaw;
static float _e_pitch;
static float _e_roll;
static float _w_yaw;
static float _w_pitch;
static float _w_roll;

int ServiceStart()
{
	Task_Handle task0;
	int result;

	result = startIMU();

	if ((result == IMU_MPU_START_FAIL)
			|| (result == IMU_COMPASS_START_FAIL))
	{
		signal(result);
		return result;
	}

	EventRegisterCB(TRACKING_CMD, &(ServiceEnd));

	active = true;

	task0 = Task_create((Task_FuncPtr) ServiceRun, NULL, NULL);
	if (task0 == NULL)
	{
		signal(SERVICE_START_FAIL);
		return SERVICE_START_FAIL;
	}

	return 0;
}

void ServiceEnd()
{
	active = false;
}

void ServiceRun(UArg arg0, UArg arg1)
{
	int attempts = 0;

	while(active)
	{
		while (attempts < MAX_ATTEMPTS)
		{
			if (!(!get_shoulder_imu(&_s_yaw, &_s_pitch, &_s_roll)
					&& !get_elbow_imu(&_e_yaw, &_e_pitch, &_e_roll)
					&& !get_wrist_imu(&_w_yaw, &_w_pitch, &_w_roll)))
			{
				attempts++;
			}
			else
			{
				break;
			}
		}

		if (attempts == MAX_ATTEMPTS)
		{
			signal(SERVICE_READ_FAIL);
			ServiceEnd();
		}

		ServicePublish(_s_yaw, _s_pitch, _s_roll, _e_yaw, _e_pitch,
			_e_roll, _w_yaw, _w_pitch, _w_roll);

		Task_yield();
	}

	signal(SERVICE_EXIT);
	Task_exit();
}

void ServicePublish(float s_yaw, float s_pitch, float s_roll, float e_yaw,
		float e_pitch, float e_roll, float w_yaw, float w_pitch, float w_roll)
{
	sendMessage message;

	message.command = TRACKING_CMD;
	message.length = 0x24;
	message.data = (float*)malloc(sizeof(float)*message.length);
	message.data[0] = s_yaw;
	message.data[1] = s_pitch;
	message.data[2] = s_roll;
	message.data[3] = e_yaw;
	message.data[4] = e_pitch;
	message.data[5] = e_roll;
	message.data[6] = w_yaw;
	message.data[7] = w_pitch;
	message.data[8] = w_roll;

	while(WiFiSendEnq(message) != 0)
	{
		Task_yield();
	}

	free(message.data);
}

int get_shoulder_imu(float* s_yaw, float* s_pitch, float* s_roll)
{
	return returnEstimate(SHOULDER_IMU_ID, s_yaw, s_pitch, s_roll);
}

int get_elbow_imu(float* e_yaw, float* e_pitch, float* e_roll)
{
	return returnEstimate(ELBOW_IMU_ID, e_yaw, e_pitch, e_roll);
}

int get_wrist_imu(float* w_yaw, float* w_pitch, float* w_roll)
{
	return returnEstimate(WRIST_IMU_ID, w_yaw, w_pitch, w_roll);
}
