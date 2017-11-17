import pandas as pd
import numpy as np
from matplotlib import pyplot
import spidev
import time

m = input("Enter Month (1-12): ")
d = input("Enter Day (1-30): ")

df = pd.read_excel("/home/pi/Documents/Solar Simulation/Solar_Insolation_1.xlsx")

a = df[df["Month"]==m]
a = a[a["Day"]==d]
a = a.reset_index(drop=True)

table_rows = len(a)

pyplot.figure()
pyplot.plot(a["Time"],a["Global Horizontal [W/m^2]"])
#pyplot.axis([4, 20, -200, 1400])
pyplot.ylabel('Global Horizontal [W/m^2]')
pyplot.xlabel('Time of day (hour)')
plot_title = 'Daily Sun Irradiance on {}/{}/2015'
pyplot.title(plot_title.format(m,d))
pyplot.grid()
#pyplot.show()


#for i in range(0, table_rows):
#    print(int(round(a['SPI Input'][i])))
#    time.sleep(0.01)

spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 976000

def write_pot(Pot):
    msb = Pot >> 8
    lsb = Pot & 0xFF
    spi.xfer([msb, lsb])

for i in range(0, table_rows):
    print(int(round(a['SPI Input'][i])))
    write_pot(int(round(a['SPI Input'][i])))
    time.sleep(1)

pyplot.show()
