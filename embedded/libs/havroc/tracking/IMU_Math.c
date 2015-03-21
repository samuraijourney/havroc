
/* HaVRoc Library Includes */
#include "havroc/tracking/IMU_Math.h"

float invSqRt(float x)
{
    long i;
    float x2, y;
    void *temp;

    x2 = x * 0.5f;
    y = x;
    temp = &y;
    i = *(long *)temp;
    i = 0x5f3759df - (i >> 1);
    temp = &i;
    y = *(float *)temp;
    y = y * (1.5f - (x2 * y * y));
    return y;
}

Vector3 poseFromAccelMag(Vector3 accel, const Vector3 mag)
{
    Vector3 result;
    Quaternion m;
    Quaternion q;

    accelToEuler(&accel, &result);

//  q.fromEuler(result);
//  since result.z() is always 0, this can be optimized a little

    float cosX2 = cos(result.m_data[0] / 2.0f);
    float sinX2 = sin(result.m_data[0] / 2.0f);
    float cosY2 = cos(result.m_data[1] / 2.0f);
    float sinY2 = sin(result.m_data[1] / 2.0f);

    q.m_data[0] = (cosX2 * cosY2);
    q.m_data[1] = (sinX2 * cosY2);
    q.m_data[2] = (cosX2 * sinY2);
    q.m_data[3] = (-sinX2 * sinY2);
//    q.normalize();

    m.m_data[0] = (0);
    m.m_data[1] = (mag.m_data[0]);
    m.m_data[2] = (mag.m_data[1]);
    m.m_data[3] = (mag.m_data[2]);

    m = multiplyQuaternion(q, multiplyQuaternion(m, conjugate(q)));
    result.m_data[2] = (-atan2(m.m_data[2], m.m_data[1]));
    return result;
}

void convertToVector(unsigned char *rawData, Vector3 *vec, float scale, bool bigEndian)
{
    if (bigEndian) {
        vec->m_data[0] = ((float)((int16_t)(((uint16_t)rawData[0] << 8) | (uint16_t)rawData[1])) * scale);
        vec->m_data[1] = ((float)((int16_t)(((uint16_t)rawData[2] << 8) | (uint16_t)rawData[3])) * scale);
        vec->m_data[2] = ((float)((int16_t)(((uint16_t)rawData[4] << 8) | (uint16_t)rawData[5])) * scale);
    } else {
        vec->m_data[0] = ((float)((int16_t)(((uint16_t)rawData[1] << 8) | (uint16_t)rawData[0])) * scale);
        vec->m_data[1] = ((float)((int16_t)(((uint16_t)rawData[3] << 8) | (uint16_t)rawData[2])) * scale);
        vec->m_data[2] = ((float)((int16_t)(((uint16_t)rawData[5] << 8) | (uint16_t)rawData[4])) * scale);
     }
}

//----------------------------------------------------------
//
//  Vector Object Functions

void Vector_Init(Vector3 *vector_object, float x, float y, float z)
{
    vector_object->m_data[0] = x;
    vector_object->m_data[1] = y;
    vector_object->m_data[2] = z;
}

Vector3 subtractVector3 (const Vector3 a, const Vector3 b)
{
    Vector3 result;
    Vector_Init(&result, a.m_data[0] - b.m_data[0], a.m_data[1] - b.m_data[1], a.m_data[2] - b.m_data[2]);
    
    return result;
}

float dotProduct(const Vector3 a, const Vector3 b)
{
    return a.m_data[0] * b.m_data[0] + a.m_data[1] * b.m_data[1] + a.m_data[2] * b.m_data[2];
}

void crossProduct(const Vector3 a, const Vector3 b, Vector3 *d)
{
    d->m_data[0] = (a.m_data[1] * b.m_data[2] - a.m_data[2] * b.m_data[1]);
    d->m_data[1] = (a.m_data[2] * b.m_data[0] - a.m_data[0] * b.m_data[2]);
    d->m_data[2] = (a.m_data[0] * b.m_data[1] - a.m_data[1] * b.m_data[0]);
}


void accelToEuler(Vector3 *vector_object, Vector3 *rollPitchYaw)
{
    normalizeVector(vector_object);

    rollPitchYaw->m_data[0] = (atan2(vector_object->m_data[1], vector_object->m_data[2]));
    rollPitchYaw->m_data[1] = (-atan2(vector_object->m_data[0], sqrt(vector_object->m_data[1] * 
                             vector_object->m_data[1] + vector_object->m_data[2] * vector_object->m_data[2])));
    rollPitchYaw->m_data[2] = 0;
}


void accelToQuaternion(Vector3 *vector_object, Quaternion *qPose)
{
    Vector3 vec;
    Vector3 z;

    Vector_Init(&vec, 0, 0, 0);
    Vector_Init(&z, 0, 0, 1.0);

    normalizeVector(vector_object);

    float angle = acos(dotProduct(z, *vector_object));
    crossProduct(*vector_object, z, &vec);
    normalizeVector(&vec);

    fromAngleVector(angle, vec, qPose);
}


void normalizeVector(Vector3 *vector_object)
{
    float invLength = invSqRt(vector_object->m_data[0] * vector_object->m_data[0] + vector_object->m_data[1] * vector_object->m_data[1] +
                              vector_object->m_data[2] * vector_object->m_data[2]);

    if ((invLength == 0) || (invLength == 1))
        return;

    vector_object->m_data[0] *= invLength;
    vector_object->m_data[1] *= invLength;
    vector_object->m_data[2] *= invLength;
}

float length(Vector3 vector_object)
{
    float invLength = invSqRt(vector_object.m_data[0] * vector_object.m_data[0] + vector_object.m_data[1] * vector_object.m_data[1] +
                              vector_object.m_data[2] * vector_object.m_data[2]);

    if (invLength == 0)
        return 0;
    else
        return 1/invLength;
}

float squareLength(Vector3 vector_object)
{
   return vector_object.m_data[0] * vector_object.m_data[0] + vector_object.m_data[1] * vector_object.m_data[1] +
          vector_object.m_data[2] * vector_object.m_data[2];
}

//----------------------------------------------------------
//
//  The RTQuaternion class

void Quaternion_Init(Quaternion *qObject, float scalar, float x, float y, float z)
{
    qObject->m_data[0] = scalar;
    qObject->m_data[1] = x;
    qObject->m_data[2] = y;
    qObject->m_data[3] = z;
}

Quaternion addQuaternion (const Quaternion a, const Quaternion b)
{
    Quaternion result;
    Quaternion_Init(&result, a.m_data[0] + b.m_data[0], a.m_data[1] + b.m_data[1], a.m_data[2] + b.m_data[2], a.m_data[3] + b.m_data[3]);
    
    return result;
}

Quaternion subtractQuaternion (const Quaternion a, const Quaternion b)
{
    Quaternion result;
    Quaternion_Init(&result, a.m_data[0] - b.m_data[0], a.m_data[1] - b.m_data[1], a.m_data[2] - b.m_data[2], a.m_data[3] - b.m_data[3]);
    
    return result;
}

Quaternion multiplyQuaternion (const Quaternion a, const Quaternion b)
{
    Quaternion result;
    Vector3 va;
    Vector3 vb;
    float dotAB;
    Vector3 crossAB;

    va.m_data[0] = (a.m_data[1]);
    va.m_data[1] = (a.m_data[2]);
    va.m_data[2] = (a.m_data[3]);

    vb.m_data[0] = (b.m_data[1]);
    vb.m_data[1] = (b.m_data[2]);
    vb.m_data[2] = (b.m_data[3]);

    dotAB = dotProduct(va, vb);
    crossProduct(va, vb, &crossAB);
    float myScalar = a.m_data[0];

    result.m_data[0] = myScalar * b.m_data[0] - dotAB;
    result.m_data[1] = myScalar * vb.m_data[0] + b.m_data[0] * va.m_data[0] + crossAB.m_data[0];
    result.m_data[2] = myScalar * vb.m_data[1] + b.m_data[0] * va.m_data[1] + crossAB.m_data[1];
    result.m_data[3] = myScalar * vb.m_data[2] + b.m_data[0] * va.m_data[2] + crossAB.m_data[2];

    return result;
}

Quaternion multiplyQuaternion2val (const Quaternion a, const float val)
{
    Quaternion result;
    Quaternion_Init(&result, a.m_data[0] * val, a.m_data[1] * val, a.m_data[2] * val, a.m_data[3] * val);
    
    return result;
}

void normalizeQuaternion(Quaternion *qObject)
{
    float invLength = invSqRt(qObject->m_data[0] * qObject->m_data[0] + qObject->m_data[1] * qObject->m_data[1] +
            qObject->m_data[2] * qObject->m_data[2] + qObject->m_data[3] * qObject->m_data[3]);

    if ((invLength == 0) || (invLength == 1))
        return;

    qObject->m_data[0] *= invLength;
    qObject->m_data[1] *= invLength;
    qObject->m_data[2] *= invLength;
    qObject->m_data[3] *= invLength;
}

void toEuler(Vector3 *vec, Quaternion qObject)
{
    vec->m_data[0] = (atan2(2.0 * (qObject.m_data[2] * qObject.m_data[3] + qObject.m_data[0] * qObject.m_data[1]),
            1 - 2.0 * (qObject.m_data[1] * qObject.m_data[1] + qObject.m_data[2] * qObject.m_data[2])));

    vec->m_data[1] = (asin(2.0 * (qObject.m_data[0] * qObject.m_data[2] - qObject.m_data[1] * qObject.m_data[3])));

    vec->m_data[2] = (atan2(2.0 * (qObject.m_data[1] * qObject.m_data[2] + qObject.m_data[0] * qObject.m_data[3]),
            1 - 2.0 * (qObject.m_data[2] * qObject.m_data[2] + qObject.m_data[3] * qObject.m_data[3])));
}

void fromEuler(Vector3 vec, Quaternion *qObject)
{
    float cosX2 = cos(vec.m_data[0] / 2.0f);
    float sinX2 = sin(vec.m_data[0] / 2.0f);
    float cosY2 = cos(vec.m_data[1] / 2.0f);
    float sinY2 = sin(vec.m_data[1] / 2.0f);
    float cosZ2 = cos(vec.m_data[2] / 2.0f);
    float sinZ2 = sin(vec.m_data[2] / 2.0f);

    qObject->m_data[0] = cosX2 * cosY2 * cosZ2 + sinX2 * sinY2 * sinZ2;
    qObject->m_data[1] = sinX2 * cosY2 * cosZ2 - cosX2 * sinY2 * sinZ2;
    qObject->m_data[2] = cosX2 * sinY2 * cosZ2 + sinX2 * cosY2 * sinZ2;
    qObject->m_data[3] = cosX2 * cosY2 * sinZ2 - sinX2 * sinY2 * cosZ2;
    normalizeQuaternion(qObject);
}

Quaternion conjugate(Quaternion qObject)
{
    Quaternion q;
    q.m_data[0] = (qObject.m_data[0]);
    q.m_data[1] = (-qObject.m_data[1]);
    q.m_data[2] = (-qObject.m_data[2]);
    q.m_data[3] = (-qObject.m_data[3]);
    return q;
}

void fromAngleVector(const float angle, const Vector3 vec, Quaternion *qObject)
{
    float sinHalfTheta = sin(angle / 2.0);
    qObject->m_data[0] = cos(angle / 2.0);
    qObject->m_data[1] = vec.m_data[0] * sinHalfTheta;
    qObject->m_data[2] = vec.m_data[1] * sinHalfTheta;
    qObject->m_data[3] = vec.m_data[2] * sinHalfTheta;
}
