import os
import datetime 
import time
import spidev
import RPi.GPIO as G
import MySQLdb
import signal, sys
import matplotlib.pyplot as plt
import numpy as np


################################################
# define global variables
################################################

A0_PIN = 11
A1_PIN = 7
A2_PIN = 5
A3_PIN = 3 

spi = spidev.SpiDev()
db = MySQLdb.connect("localhost", "wise", "wisepassword", "wisedb")

channelSettings = [] 


################################################
# define functions
################################################

def catch_sigusr1():
	getchannelsettings()

def catch_sigterm():
	G.cleanup()

signal.signal(signal.SIGUSR1, catch_sigusr1)
signal.signal(signal.SIGTERM, catch_sigterm)


def initialize():

	spi.open(0,0)
	spi.max_speed_hz = 100000 

	G.setwarnings(False)
	G.setmode(G.BOARD)
	G.setup(A0_PIN, G.OUT)
	G.setup(A1_PIN, G.OUT)
	G.setup(A2_PIN, G.OUT)
	G.setup(A3_PIN, G.OUT)

def getchannelsettings():

	global channelSettings

	channelSettings = []
	dbcursor = db.cursor()
	dbcursor.execute("select * from pom")
	for rec in dbcursor.fetchall():
		channelSettings.append(rec)

def setchannel(rec):
	print rec
	G.output(A0_PIN, rec[4])
	G.output(A1_PIN, rec[5])
	G.output(A2_PIN, rec[6])
	G.output(A3_PIN, rec[7])

def adjust_samples(arr, coeff):

	max = np.amax(arr)
	min = np.amin(arr)
	offset = (max+min)/2

	for i in range(len(arr)):
		arr[i] = (arr[i] - offset)*coeff



def performFFT(samples):

	blah = []
	N=128
	T=1.0/7680.0
	fft  = np.fft.rfft(samples,N)
	rfft = abs(fft)/10000
	for i in range(len(rfft)):
		blah.append(int(rfft[i])*169)
	return blah 

def performRMS(peak):
	
	fltpeak = float(peak)
	rms = fltpeak/1.41421
	return rms

def splitSamples(samples):

        va = []
        vb = []
        vc = []
        ia = []
        ib = []
        ic = []

        sample = [va, vb, vc, ia, ib, ic]

        ctr=0
        while ctr < len(samples):
                va.append(samples[ctr])
                ctr = ctr+1
                vb.append(samples[ctr])
                ctr = ctr+1
                vc.append(samples[ctr])
                ctr = ctr+1
                ia.append(samples[ctr])
                ctr = ctr+1
                ib.append(samples[ctr])
                ctr = ctr+1
                ic.append(samples[ctr])
                ctr = ctr+1

        return sample
	
def loaddata(pom, samples):


	dt = datetime.datetime.now()
	sample_arr = splitSamples(samples)
	max = []
	min = []
	fft = []
	rms = []
	type = ['voltage','voltage', 'voltage', 'current', 'current', 'current']
	phase = ['a', 'b', 'c', 'a', 'b', 'c']

	for i in range(len(sample_arr)):
		adjust_samples(sample_arr[i], pom[8+i])
		max.append(np.amax(sample_arr[i]))
		min.append(np.amin(sample_arr[i]))
		fft.append(performFFT(sample_arr[i]))
		rms.append(performRMS(max[len(max)-1]))

	str_pom = str(pom[0])

	dbcursor = db.cursor()

	insert_stmt = "insert into pom_data values (0, " + str_pom + ","  
	insert_stmt += "'" + dt.strftime("%Y-%m-%d %H:%M:%S") + "',"
        insert_stmt += str(dt.microsecond) + ")"
	
	print insert_stmt
	dbcursor.execute(insert_stmt)

	dbcursor.execute("select LAST_INSERT_ID()")
	pom_data_id = dbcursor.fetchone()
	
	insert_stmt = "" 


	for i in range(6):
		insert_stmt = "" 
		insert_stmt += "insert into pom_data_samples values (0,"
		insert_stmt += str(pom_data_id[0]) + ","
		insert_stmt += "'" + type[i] + "',"
		insert_stmt += "'" + phase[i] + "',"
		insert_stmt += str(rms[i]) + ","
		insert_stmt += str(min[i]) + ","
		insert_stmt += str(max[i]) + ","

		for j in range(14):
			insert_stmt += str(fft[i][j]) + "," 

		for k in range(128):
       			insert_stmt += str(sample_arr[i][k])
       			if k != 127:
               			insert_stmt += ","
		insert_stmt += ")"
		print insert_stmt
		dbcursor.execute(insert_stmt)

	db.commit()






################################################
# Main Loop
################################################

initialize()

while True:

	getchannelsettings()
	for rec in channelSettings:
			
		if rec[2] == 0:
			continue
		setchannel(rec)
		resp = spi.readbytes(1)
		print resp
		if resp[0] == 255:
			samples = spi.readbytes(rec[3])
			loaddata(rec, samples)			
		elif resp[0] != 0:
			print "bad status"
			resp = spi.writebytes([9])
		time.sleep(1)
	
