#ifndef _IMU_MATH_H
#define _IMU_MATH_H

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#define	M_PI                        3.1415926535    
#define	DEGREE_TO_RAD        		(M_PI / 180.0)
#define	RAD_TO_DEGREE        		(180.0 / M_PI)

typedef struct _VECTOR3
{
    float m_data[3];
}Vector3;

typedef struct _QUATERNION
{
    float m_data[4];
}Quaternion;

#ifdef __cplusplus
extern "C"
{
#endif

//Vector Functions
void Vector_Init(Vector3 *vector_object, float x, float y, float z);

Vector3 subtractVector3 (const Vector3 a, const Vector3 b);

float squareLength(Vector3 vector_object);

float length(Vector3 vector_object);
void normalizeVector(Vector3 *vector_object);

void accelToEuler(Vector3 *vector_object, Vector3 *rollPitchYaw);
void accelToQuaternion(Vector3 *vector_object, Quaternion *qPose);

//Quaternion Functions
void Quaternion_Init(Quaternion *qObject, float scalar, float x, float y, float z);

Quaternion addQuaternion(const Quaternion a, const Quaternion b);
Quaternion subtractQuaternion(const Quaternion a, const Quaternion b);
Quaternion multiplyQuaternion(const Quaternion a, const Quaternion b);
Quaternion multiplyQuaternion2val(const Quaternion a, const float val);

void normalizeQuaternion(Quaternion *qObject);
void toEuler(Vector3 *vec, Quaternion qObject);
void fromEuler(Vector3 vec, Quaternion *qObject);
Quaternion conjugate(Quaternion qObject);
void fromAngleVector(const float angle, const Vector3 vec, Quaternion *qObject);

//  poseFromAccelMag generates pose Euler angles from measured settings

Vector3 poseFromAccelMag(Vector3 accel, const Vector3 mag);

//  Takes signed 16 bit data from a char array and converts it to a vector of scaled floats

void convertToVector(unsigned char *rawData, Vector3 *vec, float scale, bool bigEndian);

//  Fast inverse quare root (as used by Madgwick http://www.x-io.co.uk/res/sw/madgwick_algorithm_c.zip
//  and as described in http://en.wikipedia.org/wiki/Fast_inverse_square_root

float invSqRt(float x);

float dotProduct(const Vector3 a, const Vector3 b);
void crossProduct(const Vector3 a, const Vector3 b, Vector3 *d);

#ifdef __cplusplus
}
#endif

#endif //_IMU_MATH_H
