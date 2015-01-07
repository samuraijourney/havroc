#include "service.h"
#include "radio_comm.h"
#include "estimation.h"

#define MAX_ATTEMPTS 100

bool _active = false;

float _s_yaw;
float _s_pitch;
float _s_roll;
float _e_yaw;
float _e_pitch;
float _e_roll;
float _w_yaw;
float _w_pitch;
float _w_roll;

int start_service()
{
	_active = true;
	signal(SERVICE_START_SUCCESS);
	return 0;
}

int end_service()
{
	_active = false;
	signal(SERVICE_END_SUCCESS);
	return 0;
}

int terminate_service()
{
	_active = false;
	signal(SERVICE_KILLED);
	return SERVICE_KILLED;
}

int update()
{
	int attempts = 0;

	if(!_active)
	{
		return 0;
	}

	while(attempts < MAX_ATTEMPTS)
	{
		if( !(!get_shoulder_imu(&_s_yaw,&_s_pitch,&_s_roll) &&
		      !get_elbow_imu(&_e_yaw,&_e_pitch,&_e_roll)    &&
		      !get_wrist_imu(&_w_yaw,&_w_pitch,&_w_roll)) )
		{
			attempts++;
		}
	}

	if(attempts == MAX_ATTEMPTS)
	{
		_active = false;

		return terminate_service();
	}

	return publish( _s_yaw,_s_pitch,_s_roll,
		            _e_yaw,_e_pitch,_e_roll,
		            _w_yaw,_w_pitch,_w_roll );
}

int publish(  float s_yaw,
	          float s_pitch,
	          float s_roll,
	          float e_yaw,
	          float e_pitch,
	          float e_roll,
	          float w_yaw,
	          float w_pitch,
	          float w_roll )
{
	return send_frame( s_yaw,s_pitch,s_roll,
		               e_yaw,e_pitch,e_roll,
		               w_yaw,w_pitch,w_roll );
}

int get_imu( int id
			 float* yaw,
	         float* pitch,
	         float* roll )
{

}

int get_shoulder_imu( float* s_yaw,
			          float* s_pitch,
			          float* s_roll )
{
	int result;

	result = set_imu(SHOULDER_IMU_ID);
	if(!result)
	{
		return result;
	}

	return get_imu(SHOULDER_IMU_ID,&s_yaw,&s_pitch,&s_roll);
}

int get_elbow_imu( float* e_yaw,
		           float* e_pitch,
			       float* e_roll );
{
	int result;

	result = set_imu(ELBOW_IMU_ID);
	if(!result)
	{
		return result;
	}

	return get_imu(ELBOW_IMU_ID,&s_yaw,&s_pitch,&s_roll);
}

int get_wrist_imu( float* w_yaw,
		           float* w_pitch,
			       float* w_roll )
{
	int result;

	result = set_imu(WRIST_IMU_ID);
	if(!result)
	{
		return result;
	}

	return get_imu(WRIST_IMU_ID,&s_yaw,&s_pitch,&s_roll);
}

void signal( float error )
{
	send_error(error);
}

