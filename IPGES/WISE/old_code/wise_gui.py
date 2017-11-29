import sys
import RPi.GPIO as GPIO
from Tkinter import *
from PIL import Image, ImageTk

class Zone:

    def __init__(self, gridCanvas, zone, x_cord, y_cord, pin):
        self.var = IntVar()
        self.zone=zone
        self.x=x_cord
        self.y=y_cord
        self.c = Checkbutton(gridCanvas,variable=self.var,command=self.cb,bg='white',activebackground='white')
        self.c.place(x=x_cord, y=y_cord)
        self.pin = pin
        self.safeIDs = []
        self.unsafeIDs = []
        GPIO.setup(pin, GPIO.OUT)

    def addImage(self, x_cord, y_cord, state):
        if state == "safe":
            self.safeIDs.append(gridCanvas.create_image(x_cord, y_cord, image=cb_safe))
        else
            self.unsafeIDs.append(gridCanvas.create_image(x_cord, y_cord, image=cb_unsafe))
        

    def cb(self):

        if self.var.get()==1:
            GPIO.output(self.pin,GPIO.HIGH)
            for i in safeIDs
                
            gridCanvas.lower(self.imgIDs[0])
            gridCanvas.lift(self.imgIDs[1])
        elif self.var.get()==0:
            GPIO.output(self.pin,GPIO.LOW)
            gridCanvas.lower(self.imgIDs[1])
            gridCanvas.lift(self.imgIDs[0])

#initiate
GUI = Tk()
GUI.geometry('1000x800+100+100')
GUI.title("Grid Control System")
width=800
height=600
safe="safe"
unsafe="unsafe"
GPIO.setmode(GPIO.BOARD)


#create Canvas
gridCanvas = Canvas(GUI,width=800,height=600,bg='white')
gridCanvas.pack()


#Lay Grid Image and misc
image = Image.open("grid.jpg")
grid = ImageTk.PhotoImage(image)
gridCanvas.create_image(403,300, image=grid)
red = Image.open("unsafe.jpg")
cb_unsafe = ImageTk.PhotoImage(red)
green = Image.open("safe.jpg")
cb_safe = ImageTk.PhotoImage(green)


#Create Zone Buttons
G1=Zone(gridCanvas,'G1',width*0.23,height*0.575,32)
G1.addImage(width*0.175, height*0.723, safe)
G1.addImage(width*0.175, height*0.723, unsafe)

#G2=Zone(gridCanvas,'G2',width*0.455,height*0.31)
#T1=Zone(gridCanvas,'T1',width*0.425,height*0.54)
#T2=Zone(gridCanvas,'T2',width*0.635,height*0.545)
#T3=Zone(gridCanvas,'T3',width*0.545,height*0.83)
#DT1=Zone(gridCanvas,'DT1',width*0.674,height*0.73)
#DT2=Zone(gridCanvas,'DT2',width*0.935,height*0.73)
#D1=Zone(gridCanvas,'D1',width*0.85,height*0.515)
#D3=Zone(gridCanvas,'D3',width*0.93,height*0.515)
#D2=Zone(gridCanvas,'D2',width*0.85,height*0.88)
#D4=Zone(gridCanvas,'D4',width*0.935,height*0.88)


GUI.mainloop()
