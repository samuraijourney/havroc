#ifndef H_ERROR
#define H_ERROR

// Tracking service layer errors

#define SERVICE_ERROR_BASE 0

enum
{
	SERVICE_START_FAIL = 1,
	SERVICE_END_FAIL,
	SERVICE_READ_FAIL,
	SERVICE_SEND_FAIL,
	SERVICE_EXIT,
	SERVICE_ERROR_MAX
};

// IMU Driver layer errors

#define IMU_ERROR_BASE 0

enum
{
	IMU_MPU_START_FAIL = 10,
	IMU_COMPASS_START_FAIL,
	IMU_MPU_READ_FAIL,
	IMU_COMPASS_READ_FAIL,
	IMU_ERROR_MAX
};

// Event manager layer errors

#define EVENT_ERROR_BASE 0

enum
{
	EVENT_START_FAIL = 20,
	EVENT_ERROR_MAX
};

// WiFi layer errors

#define WiFi_ERROR_BASE 0

enum
{
	WiFi_START_FAIL = 30,
	WiFi_BUFFER_FULL,
	WiFi_ERROR_MAX
};


void signal(int message);

#endif
