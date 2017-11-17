import sys
from Tkinter import *
from PIL import Image, ImageTk
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BOARD)
GPIO.setup(32,GPIO.OUT)



class Zone:    
    
    def __init__(self, gridCanvas,zone,x_cord,y_cord):
        self.var = IntVar()
        self.zone=zone
        self.x=x_cord
        self.y=y_cord
        self.c = Checkbutton(gridCanvas,variable=self.var,command=self.cb,bg='white',activebackground='white')
        self.c.place(x=x_cord, y=y_cord)
        if zone=='G1':
            gridCanvas.create_image(width*0.175,height*0.723,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.175,height*0.723,image=cb_safe,tag=self.zone+"safe")
            gridCanvas.create_image(width*0.31,height*0.723,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.31,height*0.723,image=cb_safe,tag=self.zone+"safe")
        elif zone=='G2':
            gridCanvas.create_image(width*0.56,height*0.3,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.56,height*0.3,image=cb_safe,tag=self.zone+"safe")
            gridCanvas.create_image(width*0.56,height*0.43,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.56,height*0.43,image=cb_safe,tag=self.zone+"safe")
        elif zone=='T1':
            gridCanvas.create_image(width*0.39,height*0.656,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.39,height*0.656,image=cb_safe,tag=self.zone+"safe")
            gridCanvas.create_image(width*0.513,height*0.522,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.513,height*0.522,image=cb_safe,tag=self.zone+"safe")
        elif zone=='T2':
            gridCanvas.create_image(width*0.715,height*0.656,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.715,height*0.656,image=cb_safe,tag=self.zone+"safe")
            gridCanvas.create_image(width*0.61,height*0.522,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.61,height*0.522,image=cb_safe,tag=self.zone+"safe")
        elif zone=='T3':
            gridCanvas.create_image(width*0.39,height*0.785,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.39,height*0.785,image=cb_safe,tag=self.zone+"safe")
            gridCanvas.create_image(width*0.715,height*0.785,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.715,height*0.785,image=cb_safe,tag=self.zone+"safe")
        elif zone=='DT1':
            gridCanvas.create_image(width*0.746,height*0.723,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.746,height*0.723,image=cb_safe,tag=self.zone+"safe")
        elif zone=='DT2':
            gridCanvas.create_image(width*0.99,height*0.723,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.99,height*0.723,image=cb_safe,tag=self.zone+"safe")
        elif zone=='D1':
            gridCanvas.create_image(width*0.82,height*0.656,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.82,height*0.656,image=cb_safe,tag=self.zone+"safe")
            gridCanvas.create_image(width*0.915,height*0.656,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.915,height*0.656,image=cb_safe,tag=self.zone+"safe")
        elif zone=='D2':
            gridCanvas.create_image(width*0.82,height*0.785,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.82,height*0.785,image=cb_safe,tag=self.zone+"safe")
            gridCanvas.create_image(width*0.915,height*0.785,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.915,height*0.785,image=cb_safe,tag=self.zone+"safe")
        elif zone=='D3':
            gridCanvas.create_image(width*0.97,height*0.656,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.97,height*0.656,image=cb_safe,tag=self.zone+"safe")
        elif zone=='D4':
            gridCanvas.create_image(width*0.97,height*0.785,image=cb_unsafe,tag=self.zone+"unsafe")
            gridCanvas.create_image(width*0.97,height*0.785,image=cb_safe,tag=self.zone+"safe")
    

    def cb(self):

        #print self.zone+" variable is", self.var.get()
        if self.var.get()==1:
            GPIO.output(32,GPIO.HIGH)
            gridCanvas.lower(self.zone+"safe")
            gridCanvas.lift(self.zone+"unsafe")
        elif self.var.get()==0:
            GPIO.output(32,GPIO.LOW)
            gridCanvas.lower(self.zone+"unsafe")
            gridCanvas.lift(self.zone+"safe")



    
#initiate
GUI = Tk()
GUI.geometry('1000x800+100+100')
GUI.title("Grid Control System")
width=800
height=600



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

#grid for positioning stuff
#for x in range(1,10):
#    gridCanvas.create_line(x*80,0,x*80,600,fill='black',dash=(4,4))
#    gridCanvas.create_line(0,x*60,800,x*60,fill='black',dash=(4,4))
    

#Create Zone Buttons
G1=Zone(gridCanvas,'G1',width*0.23,height*0.575)
G2=Zone(gridCanvas,'G2',width*0.455,height*0.31)
T1=Zone(gridCanvas,'T1',width*0.425,height*0.54)
T2=Zone(gridCanvas,'T2',width*0.635,height*0.545)
T3=Zone(gridCanvas,'T3',width*0.545,height*0.83)
DT1=Zone(gridCanvas,'DT1',width*0.674,height*0.73)
DT2=Zone(gridCanvas,'DT2',width*0.935,height*0.73)
D1=Zone(gridCanvas,'D1',width*0.85,height*0.515)
D3=Zone(gridCanvas,'D3',width*0.93,height*0.515)
D2=Zone(gridCanvas,'D2',width*0.85,height*0.88)
D4=Zone(gridCanvas,'D4',width*0.935,height*0.88)


GUI.mainloop()
