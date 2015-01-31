#ifndef H_ERROR
#define H_ERROR

// Tracking service layer errors
#define SERVICE_START_FAIL 		1
#define SERVICE_END_FAIL		2
#define SERVICE_KILLED			3
#define SERVICE_READ_FAIL		4

// IMU Driver layer errors
#define IMU_START_SUCCESS		  10
#define IMU_READ_SUCCESS		  11
#define IMU_READ_FAIL			  12
#define IMU_MPU_ERROR			  13
#define IMU_COMPASS_ERROR		  14
#define IMU_MPU_START_ERROR		  15
#define IMU_COMPASS_START_ERROR   16
#define IMU_MPU_START_SUCCESS     17
#define IMU_COMPASS_START_SUCCESS 18

#endif
