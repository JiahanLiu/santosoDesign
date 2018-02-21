
import os
import pandas as pd
import numpy as np
from matplotlib import pyplot
#import spidev
import time
import math

#Set entries per day -- we only simulate one day (could change if you increase this)
#WAS       144 entries per day @ 1 per 10 minutes
#Currently 289 Entries per day @ 1 per 5  minutes
entries_per_day = 289
m = input("Enter Month (1-12): ")
d = input("Enter Day (1-30): ")

#Set excel -- df = data file
#file = '/Users/bstotmeister/Documents/Sp18/Senior Design/Arduino/Solar_Insolation_1_test.xlsx'
# Use m in the filename so we don't load everything -- just the month
file = '/Users/bstotmeister/Documents/Sp18/Senior Design/Arduino/Full_2006_Wind_Solar_Data/' + str(m) +  '_Data.xlsx'
df = pd.read_excel(file)

#Set up arrays from excel file
month = df["Month"]
day = df["Day"]
year = df["Year"]
hour = df["Hour"]
minute = df["Minute"]
wind_MW = df["Wind Output (MW)"]
solar_Output = df["Solar Output"]
solar_SPI = df["SPI"]



#Calculate wind scale factor for duty cycle in the selected day
maxOutput = 0
maxSOutput = 0

for i in range(0, entries_per_day):
    if maxOutput < wind_MW[i]:
       maxOutput = wind_MW[i]
    if maxSOutput < solar_Output[i] :
        maxSOutput = solar_SPI[i]

scale_factor = maxOutput * 1.05
print("Scale factor: ", math.floor(100*(maxOutput/scale_factor)))

for i in range(0, entries_per_day):
    print("Duty cycle at ", i, " : ", math.floor(100*(wind_MW[i]/scale_factor)))
print(len(month))
print(maxOutput)
print(maxSOutput)
print(len(df))


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













