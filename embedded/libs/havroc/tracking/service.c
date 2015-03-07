#if defined(CCWARE) || defined(TIVAWARE)
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#endif

#include <havroc/tracking/service.h>
#include <havroc/tracking/imu_driver.h>
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
static volatile int active = false;
static Event_Handle   Timer_Event;
static Clock_Handle   clkHandle;


#define Passed_5ms 1

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
	float gyroX = 0;
	float gyroY = 0;
	float gyroZ = 0;
	float accelX = 0;
	float accelY = 0;
	float accelZ = 0;
	float magX = 0;
	float magY = 0;
	float magZ = 0;
	Types_FreqHz freq;

	Timestamp_getFreq(&freq);

	result = startIMU_Raw();

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
			if ((readMPUData(&accelX,&accelY,&accelZ,&gyroX,&gyroY,&gyroZ, SHOULDER_IMU_ID) != SUCCESS)
				 || (readCompassData(&magX,&magY,&magZ, SHOULDER_IMU_ID)!= SUCCESS))
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

		ServicePublish_Raw(accelX,accelY, accelZ, gyroX,gyroY,
			gyroZ, magX, magY, magZ);

		now = Timestamp_get32()/(1.0*freq.lo);

		System_printf("This is Task 3 - Elapsed Time is %.04f\n", now-prev);
		System_flush();

		Task_yield();
	}

	signal(SERVICE_EXIT);
	Task_exit();
}

void ServiceRun(void)
{
	int attempts = 0;
	int result;
	UInt events;
	float prev = 0;
	float now = 0;
	Types_FreqHz freq;

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
			if (!(!get_shoulder_imu(&_s_yaw, &_s_pitch, &_s_roll)
					//&& !get_elbow_imu(&_e_yaw, &_e_pitch, &_e_roll)
					//&& !get_wrist_imu(&_w_yaw, &_w_pitch, &_w_roll)))
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

		ServicePublish(_s_yaw, _s_pitch, _s_roll, _e_yaw, _e_pitch,
			_e_roll, _w_yaw, _w_pitch, _w_roll);

		now = Timestamp_get32()/(1.0*freq.lo);

		System_printf("This is Task 3 - Elapsed Time is %.04f\n", now-prev);
		System_flush();

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
//	message.data[3] = e_yaw;
//	message.data[4] = e_pitch;
//	message.data[5] = e_roll;
//	message.data[6] = w_yaw;
//	message.data[7] = w_pitch;
//	message.data[8] = w_roll;

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

int get_shoulder_imu(float* s_yaw, float* s_pitch, float* s_roll)
{
	return returnEstimate(SHOULDER_IMU_ID, s_yaw, s_pitch, s_roll);
}

//int get_elbow_imu(float* e_yaw, float* e_pitch, float* e_roll)
//{
//	return returnEstimate(ELBOW_IMU_ID, e_yaw, e_pitch, e_roll);
//}
//
//int get_wrist_imu(float* w_yaw, float* w_pitch, float* w_roll)
//{
//	return returnEstimate(WRIST_IMU_ID, w_yaw, w_pitch, w_roll);
//}
