clear;
clc;
delete(instrfindall); 
arduino = serial('COM3', 'BaudRate', 9600);
fopen(arduino);

accX = zeros(100,1);
accY = zeros(100,1);
accZ = zeros(100,1);
gyroX = zeros(100,1);
gyroY = zeros(100,1);
gyroZ = zeros(100,1);

for i=1:length(accX) 
temp = fscanf(arduino,'%f');
j = 1;
while(j < (length(temp)))
    if(isempty(temp))
        temp = fscanf(arduino,'%f');
        j=1;
    end
    if(double(temp(j))>57 || double(temp(j))<45)
        temp(j)=[];
    end
    if(isempty(temp))
        temp = fscanf(arduino,'%f');
        j=1;
    end
    j = j+1;
end

accX(i) = temp;
accY(i) = fscanf(arduino,'%f');
accZ(i) = fscanf(arduino,'%f');
gyroX(i) = fscanf(arduino,'%f');
gyroY(i) = fscanf(arduino,'%f');
gyroZ(i) = fscanf(arduino,'%f');
end

fclose(arduino);
delete(arduino);
clear arduino;

biasaccX = sum(accX)/100;
biasaccY = sum(accY)/100;
biasaccZ = sum(accZ)/100;
biasgyroX = sum(gyroX)/100;
biasgyroY = sum(gyroY)/100;
biasgyroZ = sum(gyroZ)/100;