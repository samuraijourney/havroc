#ifndef H_SERVICE
#define H_SERVICE

#if defined(CCWARE) || defined(TIVAWARE)
#include <ti/sysbios/knl/Task.h>
#endif

#include <stdbool.h>

#include <havroc/error.h>
#include <havroc/id.h>

static int startIMU();
static int startIMU_Raw(int imu_index);

void ServiceStart();

int ServiceStart_Raw();

void ServiceEnd();

void ServiceRun(void);	// Loop method for publishing service

void ServiceRun_Raw(void);	// Loop method for publishing service

static void ServicePublish_Raw(float accelX, float accelY, float accelZ, float gyroX,
		float gyroY, float gyroZ, float magX, float magY, float magZ); // Packages and pushes data to be communicated

static void ServicePublish(float r_s_yaw, float r_s_pitch, float r_s_roll, float r_e_yaw,
		float r_e_pitch, float r_e_roll, float r_w_yaw, float r_w_pitch, float r_w_roll,
		float l_s_yaw, float l_s_pitch, float l_s_roll, float l_e_yaw,
		float l_e_pitch, float l_e_roll, float l_w_yaw, float l_w_pitch, float l_w_roll); // Packages and pushes data to be communicated

static int get_r_shoulder_imu(float* yaw, float* pitch, float* roll);

static int get_l_shoulder_imu(float* yaw, float* pitch, float* roll);

static int get_r_elbow_imu(float* yaw, float* pitch, float* roll);

static int get_l_elbow_imu(float* yaw, float* pitch, float* roll);

static int get_r_wrist_imu(float* yaw, float* pitch, float* roll);

static int get_l_wrist_imu(float* yaw, float* pitch, float* roll);

#endif

