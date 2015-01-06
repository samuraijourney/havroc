clear;
clc;
delete(instrfindall); 
arduino = serial('COM3', 'BaudRate', 9600);
fopen(arduino);
yaw = 0;
pitch = 0;
roll = 0;

point = [0 0 0];
lineLength = 2;

while(1)
    figure(1);
    clf;
    hold on;
    
    yaw = fscanf(arduino,'%f')
    pitch = fscanf(arduino,'%f')
    roll = fscanf(arduino,'%f')
    
    x = lineLength*cosd(yaw)*cosd(pitch);
    y = lineLength*sind(yaw)*cosd(pitch);
    z = lineLength*sind(pitch);
    
    plot3([0 x],[0 y],[0 z]);
    plot3(0,0,0,'r.');
    plot3([0 x],[0 y],[-5 z],'r-');
    
    xlim([-5 5]);
    ylim([-5 5]);
    zlim([-5 5]);
    grid on;
    
    view([-37.5 30]);
end

fclose(arduino);
delete(arduino);
clear arduino;