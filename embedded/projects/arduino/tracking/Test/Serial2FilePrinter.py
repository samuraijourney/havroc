## import the serial library
import serial
import sys

## Boolean variable that will represent 
## whether or not the arduino is connected
connected = False

## establish connection to the serial port that your arduino 
## is connected to.

locations=['COM1','COM2','COM3','COM4','COM5','COM6','COM7','COM8']

for device in locations:
    try:
        print "Trying...",device
        ser = serial.Serial(device, 9600)
        break
    except:
        print "Failed to connect on",device

## loop until the arduino tells us it is ready
while not connected:
    serin = ser.read()
    connected = True

## open text file to store the current 
##gps co-ordinates received from the rover    
text_file = open("IMU_Test_Data.txt", 'w');
## read serial data from arduino and 
## write it to the text file 'position.txt'
while 1:
    if ser.inWaiting():
        x=ser.read(1)
        sys.stdout.write(x)
        text_file.write(x)
        text_file.flush()

## close the serial connection and text file
text_file.close()
ser.close()