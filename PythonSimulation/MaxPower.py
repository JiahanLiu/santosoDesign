import pandas as pd
import numpy as np
from matplotlib import pyplot
import spidev
import time

spi = spidev.SpiDev()
spi.open(0, 0)
spi.max_speed_hz = 976000

def write_pot(Pot):
    msb = Pot >> 8
    lsb = Pot & 0xFF
    spi.xfer([msb, lsb])

for i in range(0, 1000):
    print(int(round(0)))
    write_pot(int(round(0)))
    time.sleep(1)

pyplot.show()
