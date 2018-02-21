
import os
import pandas as pd
import numpy as np
from matplotlib import pyplot
import spidev
import time
import math
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BOARD)
GPIO.setup(12, GPIO.OUT)

#Raspberry Pi PWM Pin setup
p = GPIO.PWM(12, 50)    #channel 12, frequency 50Hz
p.start(0)
dc = 0

#Set entries per day -- we only simulate one day (could change if you increase this)
#WAS       144 entries per day @ 1 per 10 minutes
#Currently 288 Entries per day @ 1 per 5  minutes
entries_per_day = 288
m = input("Enter Month (1-12): ")
d = input("Enter Day (1-30): ")

#Set excel -- df = data file
#file = '/Users/bstotmeister/Documents/Sp18/Senior Design/Arduino/Solar_Insolation_1_test.xlsx'
# Use m in the filename so we don't load everything -- just the month
# Hopefully this solution doesn't take minutes to read in
file = '/home/pi/Documents/Full_2006_Wind_Solar_Data/' + str(m) +  '_Data.xlsx'
df = pd.read_excel(file)

#Set up arrays from excel file
month = df["Month"]
day = df["Day"]
year = df["Year"]
hour = df["Hour"]
minute = df["Minute"]
wind_input = df["Wind Output (MW)"]
solar_input = df["Solar Output"]
solar_SPI = df["SPI"]

# We will only get data from [day, day + entries per day]
# Arrays start at 0 ;) (but these are actually lists)
# Set up arrays to feed to generators / plot
start_point = (d-1)*entries_per_day
wind_output = [1.0] * entries_per_day
solar_output = [1.0] * entries_per_day
#time_array = [0.0, 0.08333333333333333, 0.16666666666666666, 0.25, 0.3333333333333333, 0.4166666666666667, 0.5, 0.5833333333333334, 0.6666666666666666, 0.75, 0.8333333333333334, 0.9166666666666666, 1.0, 1.0833333333333333, 1.1666666666666667, 1.25, 1.3333333333333333, 1.4166666666666667, 1.5, 1.5833333333333335, 1.6666666666666665, 1.75, 1.8333333333333335, 1.9166666666666665, 2.0, 2.0833333333333335, 2.1666666666666665, 2.25, 2.3333333333333335, 2.4166666666666665, 2.5, 2.5833333333333335, 2.6666666666666665, 2.75, 2.8333333333333335, 2.9166666666666665, 3.0, 3.0833333333333335, 3.1666666666666665, 3.25, 3.3333333333333335, 3.4166666666666665, 3.5, 3.5833333333333335, 3.6666666666666665, 3.75, 3.8333333333333335, 3.9166666666666665, 4.0, 4.083333333333333, 4.166666666666667, 4.25, 4.333333333333333, 4.416666666666667, 4.5, 4.583333333333333, 4.666666666666667, 4.75, 4.833333333333333, 4.916666666666667, 5.0, 5.083333333333333, 5.166666666666667, 5.25, 5.333333333333333, 5.416666666666667, 5.5, 5.583333333333333, 5.666666666666667, 5.75, 5.833333333333333, 5.916666666666667, 6.0, 6.083333333333333, 6.166666666666667, 6.25, 6.333333333333333, 6.416666666666667, 6.5, 6.583333333333333, 6.666666666666667, 6.75, 6.833333333333333, 6.916666666666667, 7.0, 7.083333333333333, 7.166666666666667, 7.25, 7.333333333333333, 7.416666666666667, 7.5, 7.583333333333333, 7.666666666666667, 7.75, 7.833333333333333, 7.916666666666667, 8.0, 8.083333333333334, 8.166666666666666, 8.25, 8.333333333333334, 8.416666666666666, 8.5, 8.583333333333334, 8.666666666666666, 8.75, 8.833333333333334, 8.916666666666666, 9.0, 9.083333333333334, 9.166666666666666, 9.25, 9.333333333333334, 9.416666666666666, 9.5, 9.583333333333334, 9.666666666666666, 9.75, 9.833333333333334, 9.916666666666666, 10.0, 10.083333333333334, 10.166666666666666, 10.25, 10.333333333333334, 10.416666666666666, 10.5, 10.583333333333334, 10.666666666666666, 10.75, 10.833333333333334, 10.916666666666666, 11.0, 11.083333333333334, 11.166666666666666, 11.25, 11.333333333333334, 11.416666666666666, 11.5, 11.583333333333334, 11.666666666666666, 11.75, 11.833333333333334, 11.916666666666666, 12.0, 12.083333333333334, 12.166666666666666, 12.25, 12.333333333333334, 12.416666666666666, 12.5, 12.583333333333334, 12.666666666666666, 12.75, 12.833333333333334, 12.916666666666666, 13.0, 13.083333333333334, 13.166666666666666, 13.25, 13.333333333333334, 13.416666666666666, 13.5, 13.583333333333334, 13.666666666666666, 13.75, 13.833333333333334, 13.916666666666666, 14.0, 14.083333333333334, 14.166666666666666, 14.25, 14.333333333333334, 14.416666666666666, 14.5, 14.583333333333334, 14.666666666666666, 14.75, 14.833333333333334, 14.916666666666666, 15.0, 15.083333333333334, 15.166666666666666, 15.25, 15.333333333333334, 15.416666666666666, 15.5, 15.583333333333334, 15.666666666666666, 15.75, 15.833333333333334, 15.916666666666666, 16.0, 16.083333333333332, 16.166666666666668, 16.25, 16.333333333333332, 16.416666666666668, 16.5, 16.583333333333332, 16.666666666666668, 16.75, 16.833333333333332, 16.916666666666668, 17.0, 17.083333333333332, 17.166666666666668, 17.25, 17.333333333333332, 17.416666666666668, 17.5, 17.583333333333332, 17.666666666666668, 17.75, 17.833333333333332, 17.916666666666668, 18.0, 18.083333333333332, 18.166666666666668, 18.25, 18.333333333333332, 18.416666666666668, 18.5, 18.583333333333332, 18.666666666666668, 18.75, 18.833333333333332, 18.916666666666668, 19.0, 19.083333333333332, 19.166666666666668, 19.25, 19.333333333333332, 19.416666666666668, 19.5, 19.583333333333332, 19.666666666666668, 19.75, 19.833333333333332, 19.916666666666668, 20.0, 20.083333333333332, 20.166666666666668, 20.25, 20.333333333333332, 20.416666666666668, 20.5, 20.583333333333332, 20.666666666666668, 20.75, 20.833333333333332, 20.916666666666668, 21.0, 21.083333333333332, 21.166666666666668, 21.25, 21.333333333333332, 21.416666666666668, 21.5, 21.583333333333332, 21.666666666666668, 21.75, 21.833333333333332, 21.916666666666668, 22.0, 22.083333333333332, 22.166666666666668, 22.25, 22.333333333333332, 22.416666666666668, 22.5, 22.583333333333332, 22.666666666666668, 22.75, 22.833333333333332, 22.916666666666668, 23.0, 23.083333333333332, 23.166666666666668, 23.25, 23.333333333333332, 23.416666666666668, 23.5, 23.583333333333332, 23.666666666666668, 23.75, 23.833333333333332, 23.916666666666668]
time_array = [1.0] * entries_per_day

#SPI Setup
spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 976000

#Calculate wind scale factor for duty cycle in the selected day
#Caps the duty cycle to .95
scale_factor = 33
maxOutput = 0
maxSolar = 0
for i in range(0, entries_per_day):
    time_array[i] = float(hour[i]) + float(minute[i])/60
    wind_output[i] = wind_input[start_point + i]
    solar_output[i] = solar_input[start_point + i]
    #print(time_array[i])
    if maxOutput < wind_input[start_point + i]:
       maxOutput = wind_input[start_point + i]
    if maxSolar < solar_input[start_point + i]:
       maxSolar = solar_input[start_point + i]
       #print("Duty Cycle: ", math.floor(100*(maxOutput/scale_factor)))
scale_factor = maxOutput * 1.05
#print("Max Duty Cycle: ", math.floor(100*(maxOutput/scale_factor)))

print("Before plot")
# #Set up plot

pyplot.figure()
windplot = pyplot.plot(time_array, wind_output, label='wind')
solarplot = pyplot.plot(time_array, solar_output, label='solar')
#pyplot.axis([0.0,24.0,-2, maxOutput*1.1])
pyplot.axis([0.0,24.0,-2, maxSolar*1.1])
pyplot.ylabel('Megawatts')
pyplot.xlabel('Time of Day (Hours)')
pyplot.legend(handles=windplot)
plot_title = 'Daily Renewables Output on {}/{}/2006'
pyplot.title(plot_title.format(m,d))
pyplot.grid()
pyplot.ion()
pyplot.show()
#pyplot.axis([0,24,0,maxSolar])
#pyplot.ion()
print("After plot")

#Write to potentiometer for Solar Output
def write_pot(Pot):
    Pot = 128 - Pot
    msb = Pot >> 8
    lsb = Pot & 0xFF
    spi.xfer([msb, lsb])


#Increase/Decrease duty cycle one at a time
def adjust_dc(val):
    global dc
    increment = (1) if (val > dc) else (-1)     #this is python's nasty ternary if
    #Run from current duty cycle to desired duty cycle in increments of +-1
    for x in range(dc, val + increment, increment):
        p.ChangeDutyCycle(x)
        time.sleep(0.3)
    dc = val

#ready = input("Ready?")
#Main Loop
#if (ready == "y"):

try:
    for i in range(0, entries_per_day):
        print("wind" , wind_output[i])
        print("time" , time_array[i])
        pyplot.scatter(time_array[i], wind_input[i])
        pyplot.scatter(time_array[i], solar_input[i])
        pyplot.draw()
        pyplot.pause(0.01)
        next_dc = int(math.floor(100*(wind_output[i]/scale_factor)))    #gets duty cycle 
        adjust_dc(next_dc)
        print("Current duty cycle: " + str(dc))
        write_pot(int(round(solar_SPI[start_point + i])))
        print("Current SPI: " + str(int(round(solar_SPI[start_point + i]))))
        print("Current Time: ", time_array[i])
        print('')
        time.sleep(1)
except KeyboardInterrupt:
    pass

adjust_dc(0)
p.stop()
GPIO.cleanup()

# #Set up plot
# pyplot.figure()
# windplot = pyplot.plot(time, wind_MW, label='wind')
# timeplot = pyplot.plot(time, time, label='time')
# pyplot.axis([0,24,-2, maxOutput*1.1])
# pyplot.ylabel('Megawatts')
# pyplot.xlabel('Time of Day (Hours)')
# pyplot.legend(handles=timeplot)
# plot_title = 'Daily Renewables Output on {}/{}/2006'
# pyplot.title(plot_title.format(m,d))
# pyplot.grid()
# pyplot.show()






