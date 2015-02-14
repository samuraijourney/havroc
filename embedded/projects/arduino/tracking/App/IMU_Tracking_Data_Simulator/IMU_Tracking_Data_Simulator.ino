#include <I2C.h>
#include <Estimation.h>
#include <IMU_Driver.h>

#define DATA_POINTS_PER_TEST   1000  // Number of data points to collect per action test
#define WAIT_TIME              30    // Time to wait between action tests
#define NUM_OF_ACTIONS         5     // Number of actions to be completed like arm up, arm down, arm lateral, arm punch, etc...
#define NUM_OF_IMUS            6     // Number of available IMU's
#define CLEAN_TIME             5     // Time required to clean IMU readings before gathering data in seconds

// Testing variables
char* testQueries[NUM_OF_ACTIONS];
char* imuQueries[NUM_OF_IMUS];

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
  
  testQueries[0] = "Please place your arm forward, parallel to the ground and keep it straight";
  testQueries[1] = "Now bend your arm until your elbow has reached 90 degrees while your upper arm remains parallel to the ground";
  testQueries[2] = "Hold your arm in a natural boxing stance";
  testQueries[3] = "Hold your arm neutrally at your side";
  testQueries[4] = "Hold your arm laterally to the side";
  
  imuQueries[0] = "Prepare to use the IMU on your right shoulder";
  imuQueries[1] = "Prepare to use the IMU on your right elbow";
  imuQueries[2] = "Prepare to use the IMU on your right wrist";
  imuQueries[3] = "Prepare to use the IMU on your left shoulder";
  imuQueries[4] = "Prepare to use the IMU on your left elbow";
  imuQueries[5] = "Prepare to use the IMU on your left wrist";
}

void printCountdown(int timeInSeconds)
{
    Serial.print("Prepare to begin test in ");
    
    while(timeInSeconds > 0)
    {
      Serial.print(timeInSeconds);
      Serial.print("...");
      timeInSeconds--;
      delay(1000);
    }
    
    Serial.print("\n");
}

void grabIMUData(float* out_yaw, float* out_pitch, float* out_roll)
{
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
  uint32_t now = 0;
  uint32_t delt_t = 0;
  uint32_t count = 0;
  
  static uint32_t lastUpdate = 0;
  static float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};
  static float eInt[3] = {0.0f, 0.0f, 0.0f};

  result = readMPUData(&accelX,&accelY,&accelZ,&gyroX,&gyroY,&gyroZ);
  result = readCompassData(&magX,&magY,&magZ);

  now = micros();
  deltat = ((now - lastUpdate)/1000000.0f);
  lastUpdate = now;

  MadgwickQuaternionUpdate(accelX, accelY, accelZ, gyroX*PI/180.0f, gyroY*PI/180.0f, gyroZ*PI/180.0f, magY, magX, magZ, &q[0], deltat);

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
  
  *out_yaw = yaw;
  *out_pitch = pitch;
  *out_roll = roll;
}

void cleanIMUData()
{
  float yaw, pitch, roll;
  
  uint32_t now = 0;
  uint32_t then = 0;
  now = micros();
  
  while((then - now) < (CLEAN_TIME * 1000000.0))
  {
    grabIMUData(&yaw,&pitch,&roll);
  }
}

void loop()
{
  uint32_t imus_completed = 0;
  
  Serial.print("\n\n\n\n\n");
  
  while(imus_completed < NUM_OF_IMUS)
  {
    uint32_t actions_completed = 0;
      
    Serial.println("-----------------------------------------------------");
    Serial.print("Beginning tests for IMU: ");
    Serial.println(imus_completed + 1); 
    Serial.println(imuQueries[imus_completed]);
    Serial.println("-----------------------------------------------------");
    Serial.print("\n\n\n\n\n");
    
    while(actions_completed < NUM_OF_ACTIONS)
    {
      float yaw;
      float pitch;
      float roll;
      
      Serial.println(testQueries[actions_completed]);
      
      printCountdown(WAIT_TIME); 
      
      uint32_t samples_remaining = DATA_POINTS_PER_TEST;
      
      cleanIMUData();  // Filters out garbage data
            
      while(samples_remaining > 0)
      {
        grabIMUData(&yaw, &pitch, &roll);
        
        Serial.print(yaw);
        Serial.print(",");
        Serial.print(pitch);
        Serial.print(",");
        Serial.print(roll);
        Serial.print("\n");
        
        samples_remaining--;
      }
      
      actions_completed++;
      
      Serial.println("-----------------------------------------------------");
      Serial.print("\n\n\n\n\n");
    }    
    
    imus_completed++;
  }
  
  Serial.println("Test Finished!");
  
  while(true)
  { 
    delay(1000); 
  }
}
