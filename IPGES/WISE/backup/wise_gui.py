import sys

import WiseGrid as wg
import POM as pom

import matplotlib
matplotlib.use('TkAgg')
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
from Tkinter import *
from PIL import Image, ImageTk
import xml.parsers.expat
import RPi.GPIO as GPIO


##############################
# Initialization
##############################

GPIO.setmode(GPIO.BOARD)

root = Tk()
root.title("Grid Control System")

# Maximize Window
toplevel = root.winfo_toplevel()
w = root.winfo_screenwidth()
h = root.winfo_screenheight() - 60
geom_string = "%dx%d+0+0" % (w,h)
print geom_string
toplevel.wm_geometry(geom_string)


##############################
# Create One Line Grid
##############################
canvas = wg.create_grid_canvas(root)
canvas.grid(row=0, column=0, sticky=N+W+S+E)


##############################
# Create Graphs 
##############################
graphFrame = Frame(root, bg='red', width=541, height=h)
graphFrame.grid(row=0, column=1, rowspan=2, sticky='news')


##############################
# Create POM Tables
##############################
tableFrame = Frame(root, bg='white', width=1011, height=h-544)
tableFrame.grid(row=1, column=0, sticky='news')

poms = []
for i in range(1, 10):
	poms.append(pom.POM(i, tableFrame))



###############################
# Main Loop
###############################
root.mainloop()
