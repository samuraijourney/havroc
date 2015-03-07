#include <I2C.h>
#include <Estimation.h>
#include <IMU_Driver.h>

#define DATA_POINTS_PER_TEST   1000                      // Number of data points to collect per action test
#define DATA_POINTS_PER_PRINT  DATA_POINTS_PER_TEST / 2
#define CALIBRATION_POINTS     1000                      // Number of data points to collect in order to set 0,0,0 position of yaw, pitch, roll
#define READ_DELAY             10
#define WAIT_TIME              20                        // Time to wait between action tests
#define NUM_OF_ACTIONS         6                         // Number of actions to be completed like arm up, arm down, arm lateral, arm punch, etc...
#define NUM_OF_IMUS            3                         // Number of available IMU's

// Testing variables
char* testQueries[NUM_OF_ACTIONS];
char* imuQueries[NUM_OF_IMUS];

// Data variables
float yaw_offset = 0.0f;
float pitch_offset = 0.0f;
float roll_offset = 0.0f;

float yaw_list[DATA_POINTS_PER_PRINT];
float pitch_list[DATA_POINTS_PER_PRINT];
float roll_list[DATA_POINTS_PER_PRINT];

void setup()
{
  //Init LED pin
  //pinMode(13, OUTPUT);

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
  //digitalWrite(13, HIGH);
  
  testQueries[0] = "Place your arm forward, parallel to the ground and keep it straight";
  testQueries[1] = "Now bend your arm until your elbow has reached 90 degrees while your upper arm remains parallel to the ground";
  testQueries[2] = "Hold your arm in a natural boxing stance";
  testQueries[3] = "Hold your arm neutrally at your side";
  testQueries[4] = "Hold your arm laterally to the side";
  testQueries[5] = "Punch your ass off, dog.";
  
  imuQueries[0] = "Prepare to use the IMU on your shoulder";
  imuQueries[1] = "Prepare to use the IMU on your elbow";
  imuQueries[2] = "Prepare to use the IMU on your wrist";
  //imuQueries[3] = "Prepare to use the IMU on your left shoulder";
  //imuQueries[4] = "Prepare to use the IMU on your left elbow";
  //imuQueries[5] = "Prepare to use the IMU on your left wrist";
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
  yaw   *= 180.0f / PI; // + 9.4; // Waterloo, ON magnetic declination: 9, 38.58 W
  roll  *= 180.0f / PI;
  
  *out_yaw = yaw - yaw_offset;
  *out_pitch = pitch - pitch_offset;
  *out_roll = roll - roll_offset;
}

void printCountdown(char* msg, int timeInSeconds)
{
    float yaw, pitch, roll;
    
    Serial.print(msg);
    
    while(timeInSeconds > 0)
    {
      Serial.print(timeInSeconds);
      Serial.print("...");
      timeInSeconds--;
      
      uint32_t endTime = micros() + 1000000.0;
      while(micros() < endTime)
      {
        grabIMUData(&yaw,&pitch,&roll); // Don't stop querying data from the IMU so that update can stabilize properly as you position yourself during countdown
        
        delay(17);
      }
    }
    
    Serial.print("\n");
}

void zero_yaw_pitch_roll(int sample_size)
{
  float yaw_accum = 0.0f;
  float pitch_accum = 0.0f;
  float roll_accum = 0.0f;
  
  float yaw;
  float pitch;
  float roll;
  
  for(int i = 0; i < sample_size; i++)
  {  
    grabIMUData(&yaw, &pitch, &roll);
    delay(READ_DELAY);
    
    yaw_accum += yaw;
    pitch_accum += pitch;
    roll_accum += roll;
  }
  
  yaw_offset = yaw_accum / sample_size;
  pitch_offset = pitch_accum / sample_size;
  roll_offset = roll_accum / sample_size;
}

void loop()
{
  uint32_t imus_completed = 0;
  
  Serial.print("\n\n\n\n\n");
  
  Serial.println("-----------------------------------------------------");
  Serial.println("Hold your arms neutrally at your side to zero yaw, pitch, roll");
  printCountdown("Calibrating in ", 5);
  zero_yaw_pitch_roll(CALIBRATION_POINTS);
  Serial.println("-----------------------------------------------------");
  Serial.print("\n\n\n\n\n");
  
  while(imus_completed < NUM_OF_IMUS)
  {
    uint32_t actions_completed = 0;
      
    Serial.println("-----------------------------------------------------");
    Serial.print("Beginning tests for IMU: ");
    Serial.println(imus_completed + 1); 
    Serial.println(imuQueries[imus_completed]);
    printCountdown("Tests for new IMU configuration will continue in ", WAIT_TIME); 
    Serial.println("-----------------------------------------------------");
    Serial.print("\n\n\n\n\n");
    
    while(actions_completed < NUM_OF_ACTIONS)
    {
      float yaw;
      float pitch;
      float roll;
      
      Serial.println(testQueries[actions_completed]);
      
      printCountdown("Prepare to begin test in ", WAIT_TIME); 
      
      uint32_t samples_done = 0;
      uint32_t sample_index = 0;
            
      while(samples_done < DATA_POINTS_PER_TEST)
      {
        grabIMUData(&yaw, &pitch, &roll);
        delay(READ_DELAY);
        
        sample_index = samples_done % DATA_POINTS_PER_PRINT;
        
        yaw_list[sample_index] = yaw;
        pitch_list[sample_index] = pitch;
        roll_list[sample_index] = roll;
        
        if(sample_index == DATA_POINTS_PER_PRINT - 1)
        {
          for(int i = 0; i < DATA_POINTS_PER_PRINT; i++)
          {
            Serial.print(yaw_list[i]);
            Serial.print(",");
            Serial.print(pitch_list[i]);
            Serial.print(",");
            Serial.print(roll_list[i]);
            Serial.print("\n");
          }
        }
        
        samples_done++;
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
