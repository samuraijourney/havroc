#include <I2C.h>

#define MATLAB_OUTPUT 

//Device Addresses
#define MPU_ADDRESS      0x68
#define AK8963_ADDRESS   0x0C
#define WHO_AM_I_AK8963  0x00
#define WHO_AM_I_MPU9250 0x75
#define AKM_WHOAMI       0x48
#define MPU_WHOAMI       0x71
#define AK8963_XOUT_L    0x03
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define INT_PIN_CFG      0x37
#define AK8963_CNTL      0x0A
#define AK8963_ST1       0x02
#define PWR_MGMT_1       0x6B
#define CONFIG           0x1A
#define SMPLRT_DIV       0x19
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG2    0x1D
#define AK8963_ASAX      0x10
#define AK8963_ASAY      0x11
#define AK8963_ASAZ      0x12
#define AK8963_ST2       0x09
#define INT_ENABLE       0x38
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48 

//Device modes
#define GFS_250DPS       0
#define AFS_2G           0

//Variable Init
int data = 0;
int result = 0;
float mAdj_X = 0;
float mAdj_Y = 0;
float mAdj_Z = 0;
float mUserX = 0;
float mUserY = 0;
float mUserZ = 0;
float aRes = 2.0/32768.0;
float gRes = 250.0/32768.0;
float mRes = 10.*4219./32760.;
float gyroX = 0;
float gyroY = 0;
float gyroZ = 0;
float accelX = 0;
float accelY = 0;
float accelZ = 0;
float magX = 0;
float magY = 0;
float magZ = 0;
float gyroOffsetX = 0;
float gyroOffsetY = 0;
float gyroOffsetZ = 0;
float deltat = 0.0f;
float pitch, yaw, roll;
uint32_t lastUpdate = 0;
uint32_t now = 0;
uint32_t delt_t = 0;
uint32_t count = 0;

/*Filter Variables*/
// global constants for 9 DoF fusion and AHRS (Attitude and Heading Reference System)
#define GyroMeasError PI * (60.0f / 180.0f)       // gyroscope measurement error in rads/s (shown as 60 deg/s)
#define GyroMeasDrift PI * (0.0f / 180.0f)      // gyroscope measurement drift in rad/s/s (shown as 0.0 deg/s/s)
// There is a tradeoff in the beta parameter between accuracy and response speed.
// In the original Madgwick study, beta of 0.041 (corresponding to GyroMeasError of 2.7 degrees/s) was found to give optimal accuracy.
// However, with this value, the LSM9SD0 response time is about 10 seconds to a stable initial quaternion.
// Subsequent changes also require a longish lag time to a stable output, not fast enough for a quadcopter or robot car!
// By increasing beta (GyroMeasError) by about a factor of fifteen, the response time constant is reduced to ~2 sec
// I haven't noticed any reduction in solution accuracy. This is essentially the I coefficient in a PID control sense; 
// the bigger the feedback coefficient, the faster the solution converges, usually at the expense of accuracy. 
// In any case, this is the free parameter in the Madgwick filtering and fusion scheme.
#define beta sqrt(3.0f / 4.0f) * GyroMeasError   // compute beta
#define zeta sqrt(3.0f / 4.0f) * GyroMeasDrift   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
float eInt[3] = {0.0f, 0.0f, 0.0f};       // vector to hold integral error for Mahony method


//Initialize gyro and accelerometer, check if device id is valid
void initMPU()
{
  //Check IMU and Compass device IDs
  result = I2c.read(MPU_ADDRESS, WHO_AM_I_MPU9250, 1);
  data = I2c.receive();

  if (data != MPU_WHOAMI)
  {
    Serial.println("IMU Init Failed!");

    //Alert user using LED
    while(1)
    {
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
    }
  }

  //Reset device
  I2c.write(MPU_ADDRESS, PWR_MGMT_1, 0x80);
  delay(100);

  //Wake up device
  I2c.write(MPU_ADDRESS, PWR_MGMT_1, 0x00);
  delay(100);

  //Get stable time source - PLL gyroscope reference if ready, else use internal oscillator
  I2c.write(MPU_ADDRESS, PWR_MGMT_1, 0x01);

  //Disable FSYNC and set thermometer and gyro bandwidth to 41 and 42 Hz
  I2c.write(MPU_ADDRESS, CONFIG, 0x03);

  //Set sensor output rate to 200 kHz, remaining consistent with filter update rate
  I2c.write(MPU_ADDRESS, SMPLRT_DIV, 0x04);

  //Get gyro config register contents
  result = I2c.read(MPU_ADDRESS, GYRO_CONFIG, 1);
  data = I2c.receive();

  //Clear Fchoice and full scale bits
  I2c.write(MPU_ADDRESS, GYRO_CONFIG, result & 0xE4);

  //Set full scale range for the gyro
  I2c.write(MPU_ADDRESS, GYRO_CONFIG, result | GFS_250DPS << 3);

  //Get accelerometer config register contents
  result = I2c.read(MPU_ADDRESS, ACCEL_CONFIG, 1);
  data = I2c.receive();

  //Clear full scale bits
  I2c.write(MPU_ADDRESS, ACCEL_CONFIG, result & 0xE7);

  //Set full scale range for the accelerometer
  I2c.write(MPU_ADDRESS, ACCEL_CONFIG, result | AFS_2G << 3);

  //Get accelerometer config2 register contents
  result = I2c.read(MPU_ADDRESS, ACCEL_CONFIG2, 1);
  data = I2c.receive();

  //Clear Fchoice and data rate bits
  I2c.write(MPU_ADDRESS, ACCEL_CONFIG2, result & ~0xF);

  //Set accelerometer bandwidth to 41 Hz
  I2c.write(MPU_ADDRESS, ACCEL_CONFIG2, result | 0x3);

  //Enable Bypass mode so compass is on I2C bus
  I2c.write(MPU_ADDRESS,INT_PIN_CFG,0x12);

  //Enable data ready interrupt
  I2c.write(MPU_ADDRESS, INT_ENABLE, 0x01);
}

//Initialize compass, check if device id is valid, get factory calibrated sensitivity values
void initCompass()
{
  result = I2c.read(AK8963_ADDRESS, WHO_AM_I_AK8963, 1);
  data = I2c.receive();

  if (data != AKM_WHOAMI)
  {
    Serial.println("Compass Init Failed!");

    //Alert user using LED
    while(1)
    {
      digitalWrite(13, HIGH);
      delay(1000);
      digitalWrite(13, LOW);
    }
  }

  //Shutdown compass
  I2c.write(AK8963_ADDRESS, AK8963_CNTL, 0x00);
  delay(10);

  //Enter Fuse ROM access mode
  I2c.write(AK8963_ADDRESS, AK8963_CNTL, 0x0F);
  delay(10);

  //Get sensitivity adjustment values
  result = I2c.read(AK8963_ADDRESS, AK8963_ASAX, 1);
  mAdj_X = (float)(I2c.receive()-128)/256. + 1.;

  result = I2c.read(AK8963_ADDRESS, AK8963_ASAY, 1);
  mAdj_Y = (float)(I2c.receive()-128)/256. + 1.;

  result = I2c.read(AK8963_ADDRESS, AK8963_ASAZ, 1);
  mAdj_Z = (float)(I2c.receive()-128)/256. + 1.;

  //Set compass to 8 Hz continuous read mode and to 16 bit resolution
  I2c.write(AK8963_ADDRESS, AK8963_CNTL, 0x12);
  delay(10);
}

uint8_t readIMUData(float * accel_X, float * accel_Y, float * accel_Z, float * gyro_X, float * gyro_Y, float * gyro_Z)
{
  float x = 0;
  float y = 0;
  float z = 0;
  result = 0;
  
  result = I2c.read(MPU_ADDRESS, INT_STATUS, 1);
  data = I2c.receive();

  if(data == 0x01)
  {
    result = I2c.read(MPU_ADDRESS, ACCEL_XOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(MPU_ADDRESS, ACCEL_XOUT_L, 1);
    data |= I2c.receive();

    x = (float)data*aRes;

    result = I2c.read(MPU_ADDRESS, ACCEL_YOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(MPU_ADDRESS, ACCEL_YOUT_L, 1);
    data |= I2c.receive();

    y = (float)data*aRes;

    result = I2c.read(MPU_ADDRESS, ACCEL_ZOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(MPU_ADDRESS, ACCEL_ZOUT_L, 1);
    data |= I2c.receive();

    z = (float)data*aRes;

    *accel_X = x;
    *accel_Y = y;
    *accel_Z = z;

    result = I2c.read(MPU_ADDRESS, GYRO_XOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(MPU_ADDRESS, GYRO_XOUT_L, 1);
    data |= I2c.receive();

    x = (float)data*gRes - gyroOffsetX;

    result = I2c.read(MPU_ADDRESS, GYRO_YOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(MPU_ADDRESS, GYRO_YOUT_L, 1);
    data |= I2c.receive();

    y = (float)data*gRes - gyroOffsetY;

    result = I2c.read(MPU_ADDRESS, GYRO_ZOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(MPU_ADDRESS, GYRO_ZOUT_L, 1);
    data |= I2c.receive();

    z = (float)data*gRes - gyroOffsetZ;

    *gyro_X = x;
    *gyro_Y = y;
    *gyro_Z = z;
    result = 1;
  }

  return result;
}

//Get x,y,z readings from compass
uint8_t readCompassData(float * mag_X, float * mag_Y, float * mag_Z)
{
  float x = 0;
  float y = 0;
  float z = 0;
  result = 0;

  //Check compass status for new data
  result = I2c.read(AK8963_ADDRESS, AK8963_ST1, 1);
  data = I2c.receive();

  //If new data is present, print out new data
  if(data & 0x01)
  {
    //Read compass data then convert to milliGauss
    result = I2c.read(AK8963_ADDRESS, AK8963_XOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(AK8963_ADDRESS, AK8963_XOUT_L, 1);
    data |= I2c.receive();

    x = (float)data*mRes*mAdj_X + mUserX;

    result = I2c.read(AK8963_ADDRESS, AK8963_YOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(AK8963_ADDRESS, AK8963_YOUT_L, 1);
    data |= I2c.receive();

    y = (float)data*mRes*mAdj_Y + mUserY;

    result = I2c.read(AK8963_ADDRESS, AK8963_ZOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(AK8963_ADDRESS, AK8963_ZOUT_L, 1);
    data |= I2c.receive();

    z = (float)data*mRes*mAdj_Z + mUserZ;

    result = I2c.read(AK8963_ADDRESS, AK8963_ST2, 1);
    data = I2c.receive();

    //Check if magnetic sensor overflow set, if not then report data
    if(!(data & 0x08)) 
    { 
      *mag_X = x;
      *mag_Y = y;
      *mag_Z = z;
      result = 1;
    }
  }

  return result;
}

void getGyroOffsets(float * gyro_OffsetX, float * gyro_OffsetY, float * gyro_OffsetZ)
{
  float sumX = 0;
  float sumY = 0;
  float sumZ = 0;

  for (int i = 0; i < 100; i++)
  {
    result = I2c.read(MPU_ADDRESS, GYRO_XOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(MPU_ADDRESS, GYRO_XOUT_L, 1);
    data |= I2c.receive();

    sumX += (float)data*gRes;

    result = I2c.read(MPU_ADDRESS, GYRO_YOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(MPU_ADDRESS, GYRO_YOUT_L, 1);
    data |= I2c.receive();

    sumY += (float)data*gRes;

    result = I2c.read(MPU_ADDRESS, GYRO_ZOUT_H, 1);
    data = I2c.receive() << 8;

    result = I2c.read(MPU_ADDRESS, GYRO_ZOUT_L, 1);
    data |= I2c.receive();

    sumZ += (float)data*gRes;
  }

  *gyro_OffsetX = sumX/100.;
  *gyro_OffsetY = sumY/100.;
  *gyro_OffsetZ = sumZ/100.;
}

// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
    float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
    float norm;
    float hx, hy, _2bx, _2bz;
    float s1, s2, s3, s4;
    float qDot1, qDot2, qDot3, qDot4;

    // Auxiliary variables to avoid repeated arithmetic
    float _2q1mx;
    float _2q1my;
    float _2q1mz;
    float _2q2mx;
    float _4bx;
    float _4bz;
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _2q1q3 = 2.0f * q1 * q3;
    float _2q3q4 = 2.0f * q3 * q4;
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    // Normalise accelerometer measurement
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = sqrt(mx * mx + my * my + mz * mz);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    _2q1mx = 2.0f * q1 * mx;
    _2q1my = 2.0f * q1 * my;
    _2q1mz = 2.0f * q1 * mz;
    _2q2mx = 2.0f * q2 * mx;
    hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
    hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
    norm = 1.0f/norm;
    s1 *= norm;
    s2 *= norm;
    s3 *= norm;
    s4 *= norm;

    // Compute rate of change of quaternion
    qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
    qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
    qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
    qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

    // Integrate to yield quaternion
    q1 += qDot1 * deltat;
    q2 += qDot2 * deltat;
    q3 += qDot3 * deltat;
    q4 += qDot4 * deltat;
    norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
    norm = 1.0f/norm;
    q[0] = q1 * norm;
    q[1] = q2 * norm;
    q[2] = q3 * norm;
    q[3] = q4 * norm;
}

// Similar to Madgwick scheme but uses proportional and integral filtering on the error between estimated reference vectors and
// measured ones. 
void MahonyQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
  float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
  float norm;
  float hx, hy, bx, bz;
  float vx, vy, vz, wx, wy, wz;
  float ex, ey, ez;
  float pa, pb, pc;

  // Auxiliary variables to avoid repeated arithmetic
  float q1q1 = q1 * q1;
  float q1q2 = q1 * q2;
  float q1q3 = q1 * q3;
  float q1q4 = q1 * q4;
  float q2q2 = q2 * q2;
  float q2q3 = q2 * q3;
  float q2q4 = q2 * q4;
  float q3q3 = q3 * q3;
  float q3q4 = q3 * q4;
  float q4q4 = q4 * q4;   

  // Normalise accelerometer measurement
  norm = sqrt(ax * ax + ay * ay + az * az);
  if (norm == 0.0f) return; // handle NaN
  norm = 1.0f / norm;        // use reciprocal for division
  ax *= norm;
  ay *= norm;
  az *= norm;

  // Normalise magnetometer measurement
  norm = sqrt(mx * mx + my * my + mz * mz);
  if (norm == 0.0f) return; // handle NaN
  norm = 1.0f / norm;        // use reciprocal for division
  mx *= norm;
  my *= norm;
  mz *= norm;

  // Reference direction of Earth's magnetic field
  hx = 2.0f * mx * (0.5f - q3q3 - q4q4) + 2.0f * my * (q2q3 - q1q4) + 2.0f * mz * (q2q4 + q1q3);
  hy = 2.0f * mx * (q2q3 + q1q4) + 2.0f * my * (0.5f - q2q2 - q4q4) + 2.0f * mz * (q3q4 - q1q2);
  bx = sqrt((hx * hx) + (hy * hy));
  bz = 2.0f * mx * (q2q4 - q1q3) + 2.0f * my * (q3q4 + q1q2) + 2.0f * mz * (0.5f - q2q2 - q3q3);

  // Estimated direction of gravity and magnetic field
  vx = 2.0f * (q2q4 - q1q3);
  vy = 2.0f * (q1q2 + q3q4);
  vz = q1q1 - q2q2 - q3q3 + q4q4;
  wx = 2.0f * bx * (0.5f - q3q3 - q4q4) + 2.0f * bz * (q2q4 - q1q3);
  wy = 2.0f * bx * (q2q3 - q1q4) + 2.0f * bz * (q1q2 + q3q4);
  wz = 2.0f * bx * (q1q3 + q2q4) + 2.0f * bz * (0.5f - q2q2 - q3q3);  

  // Error is cross product between estimated direction and measured direction of gravity
  ex = (ay * vz - az * vy) + (my * wz - mz * wy);
  ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
  ez = (ax * vy - ay * vx) + (mx * wy - my * wx);
  if (Ki > 0.0f)
  {
      eInt[0] += ex;      // accumulate integral error
      eInt[1] += ey;
      eInt[2] += ez;
  }
  else
  {
      eInt[0] = 0.0f;     // prevent integral wind up
      eInt[1] = 0.0f;
      eInt[2] = 0.0f;
  }

  // Apply feedback terms
  gx = gx + Kp * ex + Ki * eInt[0];
  gy = gy + Kp * ey + Ki * eInt[1];
  gz = gz + Kp * ez + Ki * eInt[2];

  // Integrate rate of change of quaternion
  pa = q2;
  pb = q3;
  pc = q4;
  q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * deltat);
  q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * deltat);
  q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * deltat);
  q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * deltat);

  // Normalise quaternion
  norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
  norm = 1.0f / norm;
  q[0] = q1 * norm;
  q[1] = q2 * norm;
  q[2] = q3 * norm;
  q[3] = q4 * norm;
}

void setup()
{
  //Init LED pin
  pinMode(13, OUTPUT);

  //Setup comms
  Serial.begin(9600);
  delay(100);
  I2c.begin();
  delay(100);
  I2c.setSpeed(1);
  delay(100);


  //Startup IMU
  initMPU();
  initCompass();

  //Run calibration for sensors
  getGyroOffsets(&gyroOffsetX,&gyroOffsetY,&gyroOffsetZ);

  //Turn on LED to signal end of calibration
  digitalWrite(13, HIGH);
}

void loop()
{
  result = readIMUData(&accelX,&accelY,&accelZ,&gyroX,&gyroY,&gyroZ);
  result = readCompassData(&magX,&magY,&magZ);

  now = micros();
  deltat = ((now - lastUpdate)/1000000.0f);
  lastUpdate = now;

  MadgwickQuaternionUpdate(accelX, accelY, accelZ, gyroX*PI/180.0f, gyroY*PI/180.0f, gyroZ*PI/180.0f, magY, magX, magZ);

  //only print every 0.5s
  delt_t = millis() - count;

  if(delt_t > 500)
  {
    // Serial.println("Accelerometer Data");
    // Serial.print("x: ");
    // Serial.println(accelX);
    // Serial.print("y: ");
    // Serial.println(accelY);
    // Serial.print("z: ");
    // Serial.println(accelZ);

    // Serial.println("Gyroscope Data");
    // Serial.print("x: ");
    // Serial.println(gyroX);
    // Serial.print("y: ");
    // Serial.println(gyroY);
    // Serial.print("z: ");
    // Serial.println(gyroZ);

    // Serial.println("Compass Data");
    // Serial.print("x: ");
    // Serial.println(magX);
    // Serial.print("y: ");
    // Serial.println(magY);
    // Serial.print("z: ");
    // Serial.println(magZ);

    // Serial.println("Quaternion Data");
    // Serial.print("q0: ");
    // Serial.println(q[0]);
    // Serial.print("qx: "); 
    // Serial.println(q[1]); 
    // Serial.print("qy: "); 
    // Serial.println(q[2]); 
    // Serial.print("qz: "); 
    // Serial.println(q[3]); 

    // Convert quaternion orientation to euler angles in aircraft orientation.
    // Positive z-axis is down toward Earth. 
    // Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North if corrected for local declination, looking down on the sensor positive yaw is counterclockwise.
    // Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive, up toward the sky is negative.
    // Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
    // The correct order which for this configuration is yaw, pitch, and then roll.

    yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);   
    pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
    roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
    pitch *= 180.0f / PI;
    yaw   *= 180.0f / PI;// + 9.4; // Waterloo, ON magnetic declination: 9, 38.58 W
    roll  *= 180.0f / PI;

    // Serial.println("Orientation Estimates");
    // Serial.print("Yaw: ");
    // Serial.println(yaw);
    // Serial.print("Pitch: ");
    // Serial.println(pitch);
    // Serial.print("Roll: ");
    // Serial.println(roll);

    // Serial.print("Update Rate (Hz) = ");
    // Serial.println((float)1.0f/deltat, 2);

#ifdef MATLAB_OUTPUT
    Serial.print(yaw); Serial.print("\n");
    Serial.print(pitch); Serial.print("\n");
    Serial.print(roll); Serial.print("\n");
#endif

    count = millis();
  }
}