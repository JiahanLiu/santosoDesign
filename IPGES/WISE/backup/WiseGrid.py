import sys
from Tkinter import *
from PIL import Image, ImageTk
import RPi.GPIO as GPIO
import MySQLdb


class Zone:    
    
    def __init__(self, canvas , zone, x_cord, y_cord, pin, lbl_loc, checked):
	self.canvas = canvas
        self.checked = IntVar()
        self.zone=zone
        self.unsafes = []
        self.safes = []
        self.pin = pin
        self.c = Checkbutton(canvas, variable=self.checked, command=self.cb, bg='white',
                             activebackground='white', highlightthickness=0, bd=0, padx=0)
        self.c.place(x=x_cord, y=y_cord)
        self.checked.set(int(checked))
        
        self.f = Frame(canvas, height=12, width = 40, bg='white')
        self.f.pack_propagate(0)
        self.l = Label(self.f, text=zone, fg='black', bg='white')

        if lbl_loc == 'w':
            self.f.place(x=x_cord - 42, y=y_cord + 3)
            self.l.pack(side='right', fill=BOTH, expand=0)  
        elif lbl_loc == 'e':
            self.f.place(x=x_cord + 20, y=y_cord + 3)
            self.l.pack(side='left', fill=BOTH, expand=0)  
        elif lbl_loc == 'n':
            self.f.place(x=x_cord - 9, y=y_cord - 17)
            self.l.pack(side='top', fill=BOTH, expand=0)  
        elif lbl_loc == 's':
            self.f.place(x=x_cord - 9, y=y_cord + 23)
            self.l.pack(side='top', fill=BOTH, expand=0)  

	#Lay Grid Image and misc
	self.red = Image.open("unsafe.jpg")
	self.cb_unsafe = ImageTk.PhotoImage(self.red)
	self.green = Image.open("safe.jpg")
	self.cb_safe = ImageTk.PhotoImage(self.green)


    def cb(self):
        GPIO.output(self.pin, self.checked.get())
        if self.checked.get()==1:
            for id in self.unsafes:
                self.canvas.lift(id)
        else:
            for id in self.safes:
                self.canvas.lift(id)

    def addSwitch(self, x_cord, y_cord):
        self.unsafes.append(self.canvas.create_image(x_cord, y_cord, image=self.cb_unsafe))
        self.safes.append(self.canvas.create_image(x_cord, y_cord, image=self.cb_safe))


def create_grid_canvas(root):

	# Create Grid Canvas
	canvas = Canvas(root, bg='white', height=544, width=1011)
	image = Image.open("grid.bmp")
	grid = ImageTk.PhotoImage(image)
	canvas.copy_image = grid;
	canvas.create_image(0, 0, image=grid, anchor=N+W)
	
	# create color dictionary for switch and checkbutton placement
	colordict = dict()
	px = image.load()
	for w in range(image.size[0]):
    		for h in range(image.size[1]):
        		if px[w,h][0] == 255 and px[w,h][1] == 0:
            			colorid = str(px[w,h][0])+str(px[w,h][1])+str(px[w,h][2])
            			colordict[colorid] = [w,h]
	
	#Create Zones
	zones = dict()

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
   		zones[zone].cb()

	return canvas

