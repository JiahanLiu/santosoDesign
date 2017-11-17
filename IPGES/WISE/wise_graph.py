import MySQLdb
from Tkinter import *
import numpy as np
import matplotlib
matplotlib.use('TkAgg')
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

class GRAPHS:

	def __init__(self, root):

		self.xticks = np.arange(0.0,0.016,0.000125)
		self.fig = plt.figure(1, figsize=(6,10))
		self.fig.subplots_adjust(wspace=0.4, hspace=0.4)
		self.ax_volt = self.fig.add_subplot(311)
		self.ax_curr = self.fig.add_subplot(312)
		self.ax_harm = self.fig.add_subplot(313)
		self.graph = FigureCanvasTkAgg(self.fig, master=root)
		self.graph.get_tk_widget().grid(row=0, column=0, sticky='news')

		self.refresh_data(1)

	def refresh_data(self, pom):

		
		print "refreshing ", pom
		self.db = MySQLdb.connect("localhost", "wise", "wisepassword", "wisedb")
		self.dbcursor = self.db.cursor() 

		self.dbcursor.execute("select b.* from pom_data a, pom_data_samples b where a.id = b.pom_data_id and a.pom_id = " + str(pom) + " and sample_type = 'voltage' order by insert_date desc, insert_date_micro desc, phase limit 3")
		#self.dbcursor.execute("select b.* from pom_data a, pom_data_samples b where a.id = b.pom_data_id and a.id = 17552 and sample_type = 'voltage' order by insert_date desc, insert_date_micro desc, phase limit 3")
		voltages = self.dbcursor.fetchall()
		max = 0
		min = 0
		for voltage in voltages:
			blah = np.max(voltage[21:149])
			if blah > max:
				max = blah
			blah = np.min(voltage[21:149])
			if blah < min:
				min = blah

		self.ax_volt.clear()
		self.ax_volt.set_ylim(min-10,max+10)
		self.ax_volt.grid(True)
		self.ax_volt.set_title("3 Phase Voltage")
		self.ax_volt.plot(self.xticks, voltages[0][21:149],'k', self.xticks, voltages[1][21:149],'r', self.xticks, voltages[2][21:149],'b')
		
		self.dbcursor.execute("select b.* from pom_data a, pom_data_samples b where a.id = b.pom_data_id and a.pom_id = " + str(pom) + " and sample_type = 'current' order by insert_date desc, insert_date_micro desc, phase limit 3")
		#self.dbcursor.execute("select b.* from pom_data a, pom_data_samples b where a.id = b.pom_data_id and a.id = 10427  and sample_type = 'current' order by insert_date desc, insert_date_micro desc, phase limit 3")
		currents= self.dbcursor.fetchall()
		max = 0
		min = 0
		for current in currents:
			blah = np.max(current[21:149])
			if blah > max:
				max = blah
			blah = np.min(current[21:149])
			if blah < min:
				min = blah

		self.ax_curr.clear()
		self.ax_curr.set_ylim(min-10,max+10)
		self.ax_curr.grid(True)
		self.ax_curr.set_title("3 Phase Current")
		self.ax_curr.plot(self.xticks, currents[0][21:149],'k', self.xticks, currents[1][21:149],'r', self.xticks, currents[2][21:149],'b')

		self.ax_harm.clear()
		self.ax_harm.set_ylim(0,150)
		width = 0.1
		self.ax_harm.set_xticks(np.arange(14)+0.5/2)
		self.ax_harm.set_xticklabels(['0','1','2','3','4','5','6','7','8','9','10','11','12','13'])
		self.ax_harm.set_title("Spetral Plot")

		self.ax_harm.bar(np.arange(14), voltages[0][7:21], 0.1, color='k')
		self.ax_harm.bar(np.arange(14)+0.1, voltages[1][7:21], 0.1, color='r')
		self.ax_harm.bar(np.arange(14)+0.2, voltages[2][7:21], 0.1, color='b')
		self.ax_harm.bar(np.arange(14)+0.3, currents[0][7:21], 0.1, color='k')
		self.ax_harm.bar(np.arange(14)+0.4, currents[1][7:21], 0.1, color='r')
		self.ax_harm.bar(np.arange(14)+0.5, currents[2][7:21], 0.1, color='b')

		self.graph.show()




