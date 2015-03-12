#ifndef _ESTIMATION_H
#define	_ESTIMATION_H

#include "havroc/tracking/IMU_Math.h"

typedef struct _fusion
{
	float m_timeDelta;                                    	// time between predictions

    Quaternion m_stateQError;                             // difference between stateQ and measuredQ

    float m_Q;                                            	// process noise covariance
    float m_R;                                            	// the measurement noise covariance

    Quaternion m_measuredQPose;       					// quaternion form of pose from measurement
    Vector3 m_measuredPose;								// vector form of pose from measurement
    Quaternion m_fusionQPose;                             // quaternion form of pose from fusion
    Vector3 m_fusionPose;                                 // vector form of pose from fusion

    bool m_firstTime;                                       // if first time after reset
    unsigned long m_lastFusionTime;                         // for delta time calculation

} fusion;

#ifdef __cplusplus
extern "C"
{
#endif

void reset(fusion* fusion_object);

//  newIMUData() should be called for subsequent updates
//  deltaTime is in units of seconds

void newIMUData(const Vector3 gyro, const Vector3 accel, const Vector3 compass, unsigned long timestamp, fusion* fusion_object);

void calculatePose(fusion* fusion_object, Vector3 accel, const Vector3 mag); // generates pose from accels and heading

#ifdef __cplusplus
}
#endif

#endif // _ESTIMATION_H
