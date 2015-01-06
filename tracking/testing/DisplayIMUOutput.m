clear;
clc;
delete(instrfindall); 
arduino = serial('COM3', 'BaudRate', 9600);
fopen(arduino);

X = zeros(100,1);
Y = zeros(100,1);
Z = zeros(100,1);

for i=1:length(X) 
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

X(i) = temp;
Y(i) = fscanf(arduino,'%f');
Z(i) = fscanf(arduino,'%f');
end

fclose(arduino);
delete(arduino);
clear arduino;