#ifndef H_SERVICE
#define H_SERVICE

#if defined(CCWARE) || defined(TIVAWARE)
	#include <ti/sysbios/knl/Task.h>
#endif

#include <error.h>
#include <id.h>

int start_service();

int end_service();

void execute(UArg arg0, UArg arg1);		// Loop method for publishing service

static int terminate_service();

static int publish( float s_yaw,
			        float s_pitch,
			        float s_roll,
			        float e_yaw,
			        float e_pitch,
			        float e_roll,
			        float w_yaw,
			        float w_pitch,
			        float w_roll );  	// Packages and pushes data to be communicated

static int get_imu( int id,
			        float* yaw,
			        float* pitch,
			        float* roll );   	// Gathers yaw, pitch, roll values from the selected IMU

static int get_shoulder_imu( float* s_yaw,
					         float* s_pitch,
					         float* s_roll );

static int get_elbow_imu( float* e_yaw,
				          float* e_pitch,
					      float* e_roll );

static int get_wrist_imu( float* w_yaw,
		           		  float* w_pitch,
			       		  float* w_roll );

static void signal( int error );		// Communicates an error to client

#endif

