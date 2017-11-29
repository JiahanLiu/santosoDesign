import sys

import matplotlib
matplotlib.use('TkAgg')
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

import numpy as np
import Tkinter as Tk
from PIL import Image, ImageTk

graph_h = 275
graph_w = 600

root = Tk.Tk()
root.title('WISE')

canvas = Tk.Canvas(root, bg='white')
image = Image.open("grid.bmp")
grid = ImageTk.PhotoImage(image)
canvas.create_image(0, 0, image=grid, anchor='nw')
canvas.grid(row=0, column=0, rowspan=2, sticky='news')

w = Tk.Frame(root, bg='orange', height=graph_h, width=954)
w.grid(row=2, column=0, sticky='nws')

plot1 = Tk.Frame(root, bg='blue', height=graph_h, width=graph_w)
plot1.grid(row=0, column=1, sticky='w')
plot2  = Tk.Frame(root, bg='green', height=graph_h, width=graph_w)
plot2.grid(row=1, column=1, sticky='w')
plot3 = Tk.Frame(root, bg='yellow', height=graph_h, width=graph_w)
plot3.grid(row=2, column=1, sticky='w')

# plot initialization
fig, ax = plt.subplots(figsize=(7,3))
ind = np.arange(14)
width = 0.20         # the width of the bars
ax.set_title('Spectral Plot')
ax.set_xticks(ind+width/2)
ax.set_ylim(0,50)
ax.set_xticklabels(['0','1','2','3','4','5','6','7','8','9','10','11','12'])
rect1 = ax.bar(np.arange(14), np.arange(14), width, color='b')

# a tk.DrawingArea
graph1 = FigureCanvasTkAgg(fig, master=plot1)
graph1.get_tk_widget().pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)
graph1.show()

# a tk.DrawingArea
graph2 = FigureCanvasTkAgg(fig, master=plot2)
graph2.get_tk_widget().pack()
graph2.show()

# a tk.DrawingArea
graph3 = FigureCanvasTkAgg(fig, master=plot3)
graph3.get_tk_widget().pack()
graph3.show()

root.mainloop()
