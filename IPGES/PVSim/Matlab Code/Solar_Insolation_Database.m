clear all
close all

prompt = 'Enter Month (1-12): '; %Input month and day number
m = input(prompt);
prompt = 'Enter Day (1-30): ';
d = input(prompt);

A = readtable('Solar_Insolation_1.csv'); %Filter data base
A(A{:,1}==m,:);
B=ans;
B(B{:,2}==d,:);
C=ans; %Final value

figure %Display the result
plot(C{:,4},C{:,5});
xlabel('Time of Day (Hour)')
ylabel('Global Irradiance (W/m^2)')
title(['Daily Sun Irradiance (' num2str(m) '/' num2str(d) '/2015)'])
grid on