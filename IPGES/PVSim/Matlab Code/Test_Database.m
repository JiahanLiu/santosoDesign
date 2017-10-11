clear all
close all

prompt = 'Enter Month (1-12): ';
m = input(prompt);
prompt = 'Enter Day (1-30): ';
d = input(prompt);

A = readtable('Solar_Insolation_1.csv');
list1= A(:,1);
list2= A(:,2);
list3= A(:,4);
list4= A(:,5);
B = [list1 list2 list3 list4];
B(B{:,1}==m,:);
C=ans;
C(C{:,2}==d,:);
D=ans;
%x = D{:,3};
%y = D{:,4};

figure
plot(D{:,3},D{:,4});
xlabel('Time of Day (Hour)')
ylabel('Global Irradiance (W/m^2)')
title('Daily Sun Irradiance')
grid on