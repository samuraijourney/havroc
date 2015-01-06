clear;
clc;
delete(instrfindall); 
arduino = serial('COM3', 'BaudRate', 9600);
fopen(arduino);
yaw = 0;
pitch = 0;
roll = 0;

for i=1:1:10
    yaw = [yaw fscanf(arduino,'%f')];
    pitch = [pitch fscanf(arduino,'%f')];
    roll = [roll fscanf(arduino,'%f')];
end

fclose(arduino);
delete(arduino);
clear arduino;