#ifndef H_SERVICE
#define H_SERVICE

#if defined(CCWARE) || defined(TIVAWARE)
#include <ti/sysbios/knl/Task.h>
#endif

#include <stdbool.h>

#include <havroc/error.h>
#include <havroc/id.h>

volatile bool active = false;

int ServiceStart();

void ServiceEnd();

void ServiceRun(UArg arg0, UArg arg1);	// Loop method for publishing service

static void ServicePublish(float s_yaw, float s_pitch, float s_roll, float e_yaw,
		float e_pitch, float e_roll, float w_yaw, float w_pitch, float w_roll); // Packages and pushes data to be communicated

static int get_shoulder_imu(float* s_yaw, float* s_pitch, float* s_roll);

static int get_elbow_imu(float* e_yaw, float* e_pitch, float* e_roll);

static int get_wrist_imu(float* w_yaw, float* w_pitch, float* w_roll);

#endif

