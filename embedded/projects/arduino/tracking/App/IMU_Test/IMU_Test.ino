#include <I2C.h>
#include <Estimation.h>
#include <IMU_Driver.h>

//#define MATLAB_OUTPUT 

//Variable Init
int data = 0;
int result = 0;
float gyroX = 0;
float gyroY = 0;
float gyroZ = 0;
float accelX = 0;
float accelY = 0;
float accelZ = 0;
float magX = 0;
float magY = 0;
float magZ = 0;
float deltat = 0.0f;
float pitch, yaw, roll;
uint32_t lastUpdate = 0;
uint32_t now = 0;
uint32_t delt_t = 0;
uint32_t count = 0;
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};
float eInt[3] = {0.0f, 0.0f, 0.0f};


void setup()
{
  //Init LED pin
  pinMode(13, OUTPUT);

  //Setup comms
  Serial.begin(9600);
  delay(100);
  
  Serial.println("Start Application");  
  
  I2c.begin();
  delay(100);
  I2c.setSpeed(1);
  delay(100);
  
  Serial.println("I2C Started");

  //Startup IMU
  initMPU();
  initCompass();
  
  Serial.println("IMU Initialized");

  //Turn on LED to signal end of calibration
  digitalWrite(13, HIGH);
}

void loop()
{
  result = readMPUData(&accelX,&accelY,&accelZ,&gyroX,&gyroY,&gyroZ);
  result = readCompassData(&magX,&magY,&magZ);

  now = micros();
  deltat = ((now - lastUpdate)/1000000.0f);
  lastUpdate = now;

  MadgwickQuaternionUpdate(accelX, accelY, accelZ, gyroX*PI/180.0f, gyroY*PI/180.0f, gyroZ*PI/180.0f, magY, magX, magZ, &q[0], deltat);

  //only print every 0.5s
  delt_t = millis() - count;

  if(delt_t > 500)
  {
    Serial.println("Accelerometer Data");
    Serial.print("x: ");
    Serial.println(accelX);
    Serial.print("y: ");
    Serial.println(accelY);
    Serial.print("z: ");
    Serial.println(accelZ);

    Serial.println("Gyroscope Data");
    Serial.print("x: ");
    Serial.println(gyroX);
    Serial.print("y: ");
    Serial.println(gyroY);
    Serial.print("z: ");
    Serial.println(gyroZ);

    Serial.println("Compass Data");
    Serial.print("x: ");
    Serial.println(magX);
    Serial.print("y: ");
    Serial.println(magY);
    Serial.print("z: ");
    Serial.println(magZ);

    Serial.println("Quaternion Data");
    Serial.print("q0: ");
    Serial.println(q[0]);
    Serial.print("qx: "); 
    Serial.println(q[1]); 
    Serial.print("qy: "); 
    Serial.println(q[2]); 
    Serial.print("qz: "); 
    Serial.println(q[3]); 

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

    Serial.println("Orientation Estimates");
    Serial.print("Yaw: ");
    Serial.println(yaw);
    Serial.print("Pitch: ");
    Serial.println(pitch);
    Serial.print("Roll: ");
    Serial.println(roll);

    Serial.print("Update Rate (Hz) = ");
    Serial.println((float)1.0f/deltat, 2);

#ifdef MATLAB_OUTPUT
    Serial.print(yaw); Serial.print("\n");
    Serial.print(pitch); Serial.print("\n");
    Serial.print(roll); Serial.print("\n");
#endif

    count = millis();
  }
}
