#include <IMU_Driver.h>
#include <I2C.h>

float mAdj_X = 0;
float mAdj_Y = 0;
float mAdj_Z = 0;
float mUserX = 0;
float mUserY = 0;
float mUserZ = 0;
float aRes = 2.0/32768.0;
float gRes = 250.0/32768.0;
float mRes = 10.*4219./32760.;
float gyroOffsetX = 0;
float gyroOffsetY = 0;
float gyroOffsetZ = 0;
int data_I2C = 0;
int result_I2C = 0;

//Initialize gyro and accelerometer, check if device id is valid
void initMPU()
{
  //Check IMU and Compass device IDs
  result_I2C = I2c.read(MPU_ADDRESS, WHO_AM_I_MPU9250, 1);
  data_I2C = I2c.receive();

  if (data_I2C != MPU_WHOAMI)
  {
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
  result_I2C = I2c.read(MPU_ADDRESS, GYRO_CONFIG, 1);
  data_I2C = I2c.receive();

  //Clear Fchoice and full scale bits
  I2c.write(MPU_ADDRESS, GYRO_CONFIG, result_I2C & 0xE4);

  //Set full scale range for the gyro
  I2c.write(MPU_ADDRESS, GYRO_CONFIG, result_I2C | GFS_250DPS << 3);

  //Get accelerometer config register contents
  result_I2C = I2c.read(MPU_ADDRESS, ACCEL_CONFIG, 1);
  data_I2C = I2c.receive();

  //Clear full scale bits
  I2c.write(MPU_ADDRESS, ACCEL_CONFIG, result_I2C & 0xE7);

  //Set full scale range for the accelerometer
  I2c.write(MPU_ADDRESS, ACCEL_CONFIG, result_I2C | AFS_2G << 3);

  //Get accelerometer config2 register contents
  result_I2C = I2c.read(MPU_ADDRESS, ACCEL_CONFIG2, 1);
  data_I2C = I2c.receive();

  //Clear Fchoice and data_I2C rate bits
  I2c.write(MPU_ADDRESS, ACCEL_CONFIG2, result_I2C & ~0xF);

  //Set accelerometer bandwidth to 41 Hz
  I2c.write(MPU_ADDRESS, ACCEL_CONFIG2, result_I2C | 0x3);

  //Enable Bypass mode so compass is on I2C bus
  I2c.write(MPU_ADDRESS,INT_PIN_CFG,0x12);

  //Enable data_I2C ready interrupt
  I2c.write(MPU_ADDRESS, INT_ENABLE, 0x01);

  //Run calibration for sensors
  getGyroOffsets(&gyroOffsetX,&gyroOffsetY,&gyroOffsetZ);
}

//Initialize compass, check if device id is valid, get factory calibrated sensitivity values
void initCompass()
{
  result_I2C = I2c.read(AK8963_ADDRESS, WHO_AM_I_AK8963, 1);
  data_I2C = I2c.receive();

  if (data_I2C != AKM_WHOAMI)
  {
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
  result_I2C = I2c.read(AK8963_ADDRESS, AK8963_ASAX, 1);
  mAdj_X = (float)(I2c.receive()-128)/256. + 1.;

  result_I2C = I2c.read(AK8963_ADDRESS, AK8963_ASAY, 1);
  mAdj_Y = (float)(I2c.receive()-128)/256. + 1.;

  result_I2C = I2c.read(AK8963_ADDRESS, AK8963_ASAZ, 1);
  mAdj_Z = (float)(I2c.receive()-128)/256. + 1.;

  //Set compass to 8 Hz continuous read mode and to 16 bit resolution
  I2c.write(AK8963_ADDRESS, AK8963_CNTL, 0x12);
  delay(10);
}

int readMPUData(float * accel_X, float * accel_Y, float * accel_Z, float * gyro_X, float * gyro_Y, float * gyro_Z)
{
  float x = 0;
  float y = 0;
  float z = 0;
  result_I2C = 0;
  
  result_I2C = I2c.read(MPU_ADDRESS, INT_STATUS, 1);
  data_I2C = I2c.receive();

  if(data_I2C == 0x01)
  {
    result_I2C = I2c.read(MPU_ADDRESS, ACCEL_XOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(MPU_ADDRESS, ACCEL_XOUT_L, 1);
    data_I2C |= I2c.receive();

    x = (float)data_I2C*aRes;

    result_I2C = I2c.read(MPU_ADDRESS, ACCEL_YOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(MPU_ADDRESS, ACCEL_YOUT_L, 1);
    data_I2C |= I2c.receive();

    y = (float)data_I2C*aRes;

    result_I2C = I2c.read(MPU_ADDRESS, ACCEL_ZOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(MPU_ADDRESS, ACCEL_ZOUT_L, 1);
    data_I2C |= I2c.receive();

    z = (float)data_I2C*aRes;

    *accel_X = x;
    *accel_Y = y;
    *accel_Z = z;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_XOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_XOUT_L, 1);
    data_I2C |= I2c.receive();

    x = (float)data_I2C*gRes - gyroOffsetX;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_YOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_YOUT_L, 1);
    data_I2C |= I2c.receive();

    y = (float)data_I2C*gRes - gyroOffsetY;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_ZOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_ZOUT_L, 1);
    data_I2C |= I2c.receive();

    z = (float)data_I2C*gRes - gyroOffsetZ;

    *gyro_X = x;
    *gyro_Y = y;
    *gyro_Z = z;
    result_I2C = 1;
  }

  return result_I2C;
}

//Get x,y,z readings from compass
int readCompassData(float * mag_X, float * mag_Y, float * mag_Z)
{
  float x = 0;
  float y = 0;
  float z = 0;
  result_I2C = 0;

  //Check compass status for new data_I2C
  result_I2C = I2c.read(AK8963_ADDRESS, AK8963_ST1, 1);
  data_I2C = I2c.receive();

  //If new data_I2C is present, print out new data_I2C
  if(data_I2C & 0x01)
  {
    //Read compass data_I2C then convert to milliGauss
    result_I2C = I2c.read(AK8963_ADDRESS, AK8963_XOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(AK8963_ADDRESS, AK8963_XOUT_L, 1);
    data_I2C |= I2c.receive();

    x = (float)data_I2C*mRes*mAdj_X + mUserX;

    result_I2C = I2c.read(AK8963_ADDRESS, AK8963_YOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(AK8963_ADDRESS, AK8963_YOUT_L, 1);
    data_I2C |= I2c.receive();

    y = (float)data_I2C*mRes*mAdj_Y + mUserY;

    result_I2C = I2c.read(AK8963_ADDRESS, AK8963_ZOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(AK8963_ADDRESS, AK8963_ZOUT_L, 1);
    data_I2C |= I2c.receive();

    z = (float)data_I2C*mRes*mAdj_Z + mUserZ;

    result_I2C = I2c.read(AK8963_ADDRESS, AK8963_ST2, 1);
    data_I2C = I2c.receive();

    //Check if magnetic sensor overflow set, if not then report data_I2C
    if(!(data_I2C & 0x08)) 
    { 
      *mag_X = x;
      *mag_Y = y;
      *mag_Z = z;
      result_I2C = 1;
    }
  }

  return result_I2C;
}

void getGyroOffsets(float * gyro_OffsetX, float * gyro_OffsetY, float * gyro_OffsetZ)
{
  float sumX = 0;
  float sumY = 0;
  float sumZ = 0;

  for (int i = 0; i < 100; i++)
  {
    result_I2C = I2c.read(MPU_ADDRESS, GYRO_XOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_XOUT_L, 1);
    data_I2C |= I2c.receive();

    sumX += (float)data_I2C*gRes;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_YOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_YOUT_L, 1);
    data_I2C |= I2c.receive();

    sumY += (float)data_I2C*gRes;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_ZOUT_H, 1);
    data_I2C = I2c.receive() << 8;

    result_I2C = I2c.read(MPU_ADDRESS, GYRO_ZOUT_L, 1);
    data_I2C |= I2c.receive();

    sumZ += (float)data_I2C*gRes;
  }

  *gyro_OffsetX = sumX/100.;
  *gyro_OffsetY = sumY/100.;
  *gyro_OffsetZ = sumZ/100.;
}