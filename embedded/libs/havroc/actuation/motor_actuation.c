#include <Wire.h>

// I2C SRF10 or SRF08 Devantech Ultrasonic Ranger Finder 
// by Nicholas Zambetti <http://www.zambetti.com>
// and James Tichenor <http://www.jamestichenor.net> 

// Demonstrates use of the Wire library reading data from the 
// Devantech Utrasonic Rangers SFR08 and SFR10

// Created 29 April 2006

// This example code is in the public domain.
#define DRV2604L_ADDR 0x5A
#define COUNT 1

unsigned long time;
unsigned int LRA_PERIOD;
unsigned int VBAT;

void erm_setup();

void  register_write(unsigned int reg_addr, unsigned int data);

unsigned int register_read(unsigned int reg_addr);

float voltage_monitor();

void setup()
{
  Wire.begin();                // join i2c bus (address optional for master)
  Serial.begin(9600);          // start serial communication at 9600bps
  erm_setup();
}

void loop()
{
  delay(50);
//    register_write(0x1D,0x49);
//    delay(50);
//    //set resonant period of lra
//    register_write(0x20,0x3A);
//    delay(50);
  // select the RTP control mode
  register_write(0x01,0x05);
  Serial.println("Entered RTP mode");
  delay(50);

  // brake the motor for 1 s
  register_write(0x02,0x81);
  delay(1000);

  // 100% motor speed time test
  time=millis();
  register_write(0x02,0x7F);
  Serial.print("100 percent speed for 3 seconds");
  while ((millis()-time)<3000){}
  Serial.println("Run complete");

  time=millis();
  // brake the motor for 1 s
  Serial.println("Now braking");

  // brake the motor for 1 s
  register_write(0x02,0x81);
  delay(1000);
  Serial.println("Program complete");
  while (1);
}

void erm_setup(){
  // initial reading to see if driver is accessible
  delay(70);                   
  
  Serial.println(register_read(0x00));

  delay(250);
  // testing complete, should output 192
  
  // put in standby mode to begin programming
  register_write(0x01,0x07);
  
  //delay(50);
  // change DATA_FORMAT_RTP from 0 to 1 for unsigned input format for RTP control
  //register_write(0x1D,0x48);
  
  delay(50);
  // specify actuator specific data, this is pg 26 step 3a, b and c
  register_write(0x1A,0x2A);
  
  delay(50);
  // pg 26 step 3d-motor voltage set at 3V here
  register_write(0x16,0x8E);
  delay(50);
  // pg 26 step 3e- max motor voltage set at 3.5 V here
  register_write(0x17,0xA2);
  delay(50);
  // pg 26 step 3g- setting drive time, 2.4 ms for ERM
  register_write(0x1B,0x93);
  delay(50);
  // pg 26 step 3h, i and j
  register_write(0x1C,0xF5);
  delay(50);
  // pg 26 step 3f and k
  register_write(0x1E,0x30);

  delay(50);
  // pg 26 step 4, setting the go bit to 1 to start auto calibration
  register_write(0x0C,0x01);

  delay(70);
  // pg 26 step 5, check results of auto calibration
  Serial.println(register_read(0x00));
}

unsigned int register_read(unsigned int reg_addr){
  Wire.beginTransmission(DRV2604L_ADDR);
  Wire.write(byte(reg_addr));  
  Wire.endTransmission(); 
  
  Wire.requestFrom(DRV2604L_ADDR, COUNT); 
  if(COUNT <= Wire.available()){
    return Wire.read();
  }
}

void  register_write(unsigned int reg_addr, unsigned int data){
  Wire.beginTransmission(DRV2604L_ADDR);
  Wire.write(byte(reg_addr)); 
  Wire.write(byte(data));  
  Wire.endTransmission(); 
}

float voltage_monitor(){
  VBAT=register_read(0x21);
  float vdd;
  vdd=VBAT*5.6/255;
  return vdd;
}
