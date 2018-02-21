
import os
import pandas as pd
import numpy as np
from matplotlib import pyplot
#import spidev
import time
import math

#Set entries per day -- we only simulate one day (could change if you increase this)
#Currently 144 entries per day @ 1 per 10 minutes
entries_per_day = 144
m = input("Enter Month (1-12): ")
d = input("Enter Day (1-30): ")

#Set excel -- df = data file
file = '/Users/bstotmeister/Documents/Sp18/Senior Design/Arduino/Solar_Insolation_1_test.xlsx'
df = pd.read_excel(file)

#Set up arrays from excel file
month = df["Month"]
day = df["Day"]
year = df["Year"]
hour = df["Hour"]
minute = df["Minute"]
wind_MW = df["Wind Output (MW)"]
solar_MW = df["SPI Input"]
#time = df["Time"]
m1 = df["M1"]
m2 = df["M2"]
m3 = df["M3"]
m4 = df["M4"]
m5 = df["M5"]
m6 = df["M6"]

res = [["mm"]["dd"]["yy"]["hh"]["min"]["wo"]

isFirst = 1
for i in range(0, len(month)):
    if isFirst:
        res["mm"][i] = month[i]
        res["dd"][i] = day[i]
        res["yy"][i] = year[i]
        res["hh"][i] = hour[i]
        res["min"][i] = minute[i]
        res["wo"][i] = wind_MW[i]
        isFirst = 0
    else:
        res["mm"][i] = m1[i]
        res["dd"][i] = m2[i]
        res["yy"][i] = m3[i]
        res["hh"][i] = m3[i]
        res["min"][i] = m4[i]
        res["wo"][i] = m5[i]
        isFirst = 0



#Calculate wind scale factor for duty cycle in the selected day
maxOutput = 0
for i in range(0, entries_per_day):
	if maxOutput < wind_MW[i]:
		maxOutput = wind_MW[i]

scale_factor = maxOutput * 1.05
print("Scale factor: ", math.floor(100*(maxOutput/scale_factor)))

for i in range(0, entries_per_day):
	print("Duty cycle at ", i, " : ", math.floor(100*(wind_MW[i]/scale_factor)))
print(len(month))

#Set up plot
pyplot.figure()
windplot = pyplot.plot(time, wind_MW, label='wind')
timeplot = pyplot.plot(time, time, label='time')
pyplot.axis([0,24,-2, maxOutput*1.1])
pyplot.ylabel('Megawatts')
pyplot.xlabel('Time of Day (Hours)')
pyplot.legend(handles=timeplot)
plot_title = 'Daily Renewables Output on {}/{}/2006'
pyplot.title(plot_title.format(m,d))
pyplot.grid()
pyplot.show()













