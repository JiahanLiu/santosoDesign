import sys

import wise_grid as wg
import wise_pom as pom
import wise_graph as graph

import matplotlib
#matplotlib.use('TkAgg')
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
from Tkinter import *
from PIL import Image, ImageTk
import xml.parsers.expat
import RPi.GPIO as GPIO


##############################
# Initialization
##############################

class WiseGui(Tk):

	def __init__(self):
		Tk.__init__(self)
	
		GPIO.setmode(GPIO.BOARD)

		self.title("Grid Control System")
		self.graph_pom = IntVar()
		self.graph_pom.set(1)

		# Maximize Window
		toplevel = self.winfo_toplevel()
		w = self.winfo_screenwidth()
		h = self.winfo_screenheight() - 60
		geom_string = "%dx%d+0+0" % (w,h)
		toplevel.wm_geometry(geom_string)


		##############################
		# Create One Line Grid
		##############################
		canvas = wg.create_grid_canvas(self)
		canvas.grid(row=0, column=0, sticky=N+W+S+E)


		##############################
		# Create POM Tables
		##############################
		tableFrame = Frame(self, bg='white', width=1011, height=h-544)
		tableFrame.grid(row=1, column=0, sticky='news')

		self.poms = []
		for i in range(1, 10):
			self.poms.append(pom.POM(i, tableFrame, self.graph_pom))

		##############################
		# Create Graphs 
		##############################
		graphFrame = Frame(self, bg='red', width=541, height=h)
		graphFrame.grid(row=0, column=1, rowspan=2, sticky='news')

		self.graphs = graph.GRAPHS(graphFrame)
		
		self.refreshStuff()


	def refreshStuff(self):

		for pom in self.poms:
			pom.refresh_data()

		self.graphs.refresh_data(self.graph_pom.get())

		self.after(1000, self.refreshStuff)



###############################
# Main Loop
###############################
root = WiseGui()
root.mainloop()
