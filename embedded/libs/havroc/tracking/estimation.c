#include "havroc/tracking/Estimation.h"

#include "uart_if.h"
#include "common.h"

//  The QVALUE affects the gyro response.

#define QVALUE	0.001f

//  The RVALUE controls the influence of the accels and compass.
//  The bigger the value, the more sluggish the response.

#define RVALUE	0.0005f

void reset(fusion* fusion_object)
{
	fusion_object->m_Q = QVALUE;
    fusion_object->m_R = RVALUE;
	fusion_object->m_firstTime = true;
	Vector_Init(&(fusion_object->m_fusionPose), 0, 0, 0);
	fromEuler(fusion_object->m_fusionPose, &(fusion_object->m_fusionQPose));
	Vector_Init(&(fusion_object->m_measuredPose), 0, 0, 0);
	fromEuler(fusion_object->m_measuredPose, &(fusion_object->m_measuredQPose));
}

void newIMUData(const Vector3 gyro, const Vector3 accel, const Vector3 compass, unsigned long timestamp, fusion* fusion_object)
{
	if (fusion_object->m_firstTime) 
	{
		fusion_object->m_lastFusionTime = timestamp;

		calculatePose(fusion_object, accel, compass);

        //  initialize the poses

		fromEuler(fusion_object->m_measuredPose, &(fusion_object->m_fusionQPose));
		fusion_object->m_fusionPose = fusion_object->m_measuredPose;
		fusion_object->m_firstTime = false;
	} 
	else 
	{
		fusion_object->m_timeDelta = (float)(timestamp - fusion_object->m_lastFusionTime) / (float)1000;
		fusion_object->m_lastFusionTime = timestamp;
		if (fusion_object->m_timeDelta <= 0)
		{
			Report("in return \n\r");
			return;
		}

		calculatePose(fusion_object, accel, compass);
        //Serial.print("qs "); Serial.println(fusion_object->m_measuredQPose.scalar());

		//  predict();

		float x2, y2, z2;
		float qs, qx, qy,qz;

		qs = fusion_object->m_fusionQPose.m_data[0];
		qx = fusion_object->m_fusionQPose.m_data[1];
		qy = fusion_object->m_fusionQPose.m_data[2];
		qz = fusion_object->m_fusionQPose.m_data[3];

//	    Serial.print("qs "); Serial.println(qs);

		x2 = gyro.m_data[0] / (float)2.0;
		y2 = gyro.m_data[1] / (float)2.0;
		z2 = gyro.m_data[2] / (float)2.0;

        // Predict new state

		fusion_object->m_fusionQPose.m_data[0] = (qs + (-x2 * qx - y2 * qy - z2 * qz) * fusion_object->m_timeDelta);
		fusion_object->m_fusionQPose.m_data[1] = (qx + (x2 * qs + z2 * qy - y2 * qz) * fusion_object->m_timeDelta);
		fusion_object->m_fusionQPose.m_data[2] = (qy + (y2 * qs - z2 * qx + x2 * qz) * fusion_object->m_timeDelta);
		fusion_object->m_fusionQPose.m_data[3] = (qz + (z2 * qs + y2 * qx - x2 * qy) * fusion_object->m_timeDelta);

		// update();

		fusion_object->m_stateQError = subtractQuaternion(fusion_object->m_measuredQPose, fusion_object->m_fusionQPose);

        // make new state estimate

		float qt = fusion_object->m_Q * fusion_object->m_timeDelta;

		fusion_object->m_fusionQPose = addQuaternion(fusion_object->m_fusionQPose, multiplyQuaternion2val(fusion_object->m_stateQError, (qt / (qt + fusion_object->m_R))));

		normalizeQuaternion(&(fusion_object->m_fusionQPose));

		toEuler(&(fusion_object->m_fusionPose), fusion_object->m_fusionQPose);
	}
}

void calculatePose(fusion* fusion_object, Vector3 accel, const Vector3 mag)
{
    Quaternion m;
    Quaternion q;
    int i;

    accelToEuler(&accel, &(fusion_object->m_measuredPose));
    
    float cosX2 = cos(fusion_object->m_measuredPose.m_data[0] / 2.0f);
    float sinX2 = sin(fusion_object->m_measuredPose.m_data[0] / 2.0f);
    float cosY2 = cos(fusion_object->m_measuredPose.m_data[1] / 2.0f);
    float sinY2 = sin(fusion_object->m_measuredPose.m_data[1] / 2.0f);

    q.m_data[0] = (cosX2 * cosY2);
    q.m_data[1] = (sinX2 * cosY2);
    q.m_data[2] =(cosX2 * sinY2);
    q.m_data[3] = ( - sinX2 * sinY2);
    
    //   normalize();

    m.m_data[0] = (0);
    m.m_data[1] = (mag.m_data[0]);
    m.m_data[2] = (mag.m_data[1]);
    m.m_data[3] = (mag.m_data[2]);

    m = multiplyQuaternion(multiplyQuaternion(q, m), conjugate(q));
    //Serial.print("mag"); Serial.println(mag.z());

    fusion_object->m_measuredPose.m_data[2]= -atan2(m.m_data[2], m.m_data[1]);
 
    fromEuler(fusion_object->m_measuredPose, &(fusion_object->m_measuredQPose));

    //  check for quaternion aliasing. If the quaternion has the wrong sign
    //  the kalman filter will be very unhappy.

    int maxIndex = -1;
    float maxVal = -1000;

    for (i = 0; i < 4; i++) {
        if (fabs(fusion_object->m_measuredQPose.m_data[i]) > maxVal) {
            maxVal = fabs(fusion_object->m_measuredQPose.m_data[i]);
            maxIndex = i;
        }
    }

    //  if the biggest component has a different sign in the measured and kalman poses,
    //  change the sign of the measured pose to match.

    if (((fusion_object->m_measuredQPose.m_data[maxIndex] < 0) && (fusion_object->m_fusionQPose.m_data[maxIndex] > 0)) ||
            ((fusion_object->m_measuredQPose.m_data[maxIndex] > 0) && (fusion_object->m_fusionQPose.m_data[maxIndex] < 0))) {
        fusion_object->m_measuredQPose.m_data[0] = (-(fusion_object->m_measuredQPose.m_data[0]));
        fusion_object->m_measuredQPose.m_data[1] = (-(fusion_object->m_measuredQPose.m_data[1]));
        fusion_object->m_measuredQPose.m_data[2] = (-(fusion_object->m_measuredQPose.m_data[2]));
        fusion_object->m_measuredQPose.m_data[3] = (-(fusion_object->m_measuredQPose.m_data[3]));
        toEuler(&(fusion_object->m_measuredPose), fusion_object->m_measuredQPose);
    }
}
