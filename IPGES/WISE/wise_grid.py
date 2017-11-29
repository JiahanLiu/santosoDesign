import sys
from Tkinter import *
from PIL import Image, ImageTk
import RPi.GPIO as GPIO
import MySQLdb


class Zone:    
    
	def __init__(self, canvas , zone, x_cord, y_cord, pin, lbl_loc, status):
		self.canvas = canvas
		self.zone_status = status 
		self.zone=zone
		self.unsafes = []
		self.safes = []
		self.pin = pin

		self.frm = Frame(canvas, height=30, width=40)
		self.frm.pack_propagate(0)
		self.frm.place(x=x_cord, y=y_cord)
		self.btn = Button(self.frm, text=zone, command=self.cb, activebackground='grey') 
		self.btn.pack(fill=BOTH, expand=1)
       	 
		self.red = Image.open("unsafe.jpg")
		self.cb_unsafe = ImageTk.PhotoImage(self.red)
		self.green = Image.open("safe.jpg")
		self.cb_safe = ImageTk.PhotoImage(self.green)


	def set_zone_status(self, status):

		if status == 1:
			for id in self.unsafes:
				self.canvas.lift(id)
		else:
			for id in self.safes:
				self.canvas.lift(id)

		GPIO.output(self.pin, status)
		self.zone_status = status


	def refresh_zone_status(self):
		self.set_zone_status(self.zone_status)	

	def cb(self):
		
		if self.zone_status == 0:
			self.set_zone_status(1)	
		else:
			self.set_zone_status(0)	

	def addSwitch(self, x_cord, y_cord):
		self.unsafes.append(self.canvas.create_image(x_cord, y_cord, image=self.cb_unsafe))
		self.safes.append(self.canvas.create_image(x_cord, y_cord, image=self.cb_safe))


zones = dict()
##################################
# Create Emergency Shutdown Button
##################################
def Emergency_Shutdown():
	global zones
	for zone in zones:
		zones[zone].set_zone_status(0)

def create_grid_canvas(root):

	# Create Grid Canvas
	canvas = Canvas(root, bg='white', height=544, width=1011)
	image = Image.open("grid.bmp")
	grid = ImageTk.PhotoImage(image)
	canvas.copy_image = grid;
	canvas.create_image(0, 0, image=grid, anchor=N+W)
	
	global zones

	
	##################################
	# Create Color Dictionary 
	##################################
	colordict = dict()
	px = image.load()
	for w in range(image.size[0]):
    		for h in range(image.size[1]):
        		if px[w,h][0] == 255 and px[w,h][1] == 0:
            			colorid = str(px[w,h][0])+str(px[w,h][1])+str(px[w,h][2])
            			colordict[colorid] = [w,h]
	
	##################################
	# Create Zones
	##################################

	db = MySQLdb.connect("localhost", "wise", "wisepassword", "wisedb")
        dbcursor = db.cursor()
        dbcursor.execute("select id, label, label_loc, rgb_id, checked, gpio_pin from zones")
	for rec in dbcursor.fetchall():
       		zones[rec[0]] = Zone(canvas, rec[1], colordict[str(rec[3])][0], colordict[str(rec[3])][1], int(rec[5]), rec[2], rec[4])
		GPIO.setup(rec[5], GPIO.OUT)

	dbcursor.execute("select zones.id, contactors.rgb_id from zones, contactors where zones.id = contactors.zone_id")
	for rec in dbcursor.fetchall():
		zones[rec[0]].addSwitch(colordict[str(rec[1])][0], colordict[str(rec[1])][1])

	for zone in zones:
   		zones[zone].refresh_zone_status()

		
	frm = Frame(canvas, height=60, width=60)
	frm.pack_propagate(0)
	frm.place(x=10, y=10)
	btn = Button(frm, text="OFF!", command=Emergency_Shutdown, activebackground='grey') 
	btn.pack(fill=BOTH, expand=1)

	return canvas

