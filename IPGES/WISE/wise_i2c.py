import os
import datetime 
import time
import smbus
import RPi.GPIO as G
import MySQLdb
import signal, sys
import matplotlib.pyplot as plt
import numpy as np


################################################
# define global variables
################################################

db = MySQLdb.connect("localhost", "wise", "wisepassword", "wisedb")
channelSettings = [] 
bus = smbus.SMBus(1)


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

	G.setwarnings(False)
	G.setmode(G.BOARD)

def getchannelsettings():

	global channelSettings

	channelSettings = []
	dbcursor = db.cursor()
	dbcursor.execute("select * from pom")
	for rec in dbcursor.fetchall():
		channelSettings.append(rec)

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
	rfft = abs(fft)
	for i in range(len(rfft)):
		blah.append(int(rfft[i])/90.5097)
	return blah 

def performRMS(peak):
	
	fltpeak = float(peak)
	rms = fltpeak/1.41421
	return rms

def splitSamples(samples):

        a = []
        b = []
        c = []

        sample = [a, b, c]

        ctr=0
        while ctr < len(samples):
                a.append(samples[ctr])
                ctr = ctr+1
                b.append(samples[ctr])
                ctr = ctr+1
                c.append(samples[ctr])
                ctr = ctr+1

        return sample
	
def loaddata(pom, samples, type):


	dt = datetime.datetime.now()
	sample_arr = splitSamples(samples)
	max = []
	min = []
	fft = []
	rms = []
	phase = ['a', 'b', 'c']

	if type == "voltage":
		coeff_offset = 4;
	else:
		coeff_offset = 7;

	for i in range(len(sample_arr)):
		adjust_samples(sample_arr[i], pom[coeff_offset+i])
		max.append(np.amax(sample_arr[i]))
		min.append(np.amin(sample_arr[i]))
		fft.append(performFFT(sample_arr[i]))
		rms.append(performRMS(max[len(max)-1]))

	str_pom = str(pom[0])

	dbcursor = db.cursor()

	insert_stmt = "insert into pom_data values (0, " + str_pom + ","  
	insert_stmt += "'" + dt.strftime("%Y-%m-%d %H:%M:%S") + "',"
        insert_stmt += str(dt.microsecond) + ")"
	
	dbcursor.execute(insert_stmt)

	dbcursor.execute("select LAST_INSERT_ID()")
	pom_data_id = dbcursor.fetchone()
	
	insert_stmt = "" 


	for i in range(len(sample_arr)):
		insert_stmt = "" 
		insert_stmt += "insert into pom_data_samples values (0,"
		insert_stmt += str(pom_data_id[0]) + ","
		insert_stmt += "'" + type + "',"
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

		volt_addr = rec[10]
		resp = bus.read_byte(volt_addr) 
		if resp != 255:
			samples = []
			samples.append(resp)
			for i in range(rec[3]-1):
				samples.append(bus.read_byte(volt_addr))
			loaddata(rec, samples, "voltage")			

		curr_addr = rec[11]
		resp = bus.read_byte(curr_addr) 
		if resp != 255:
			samples = []
			samples.append(resp)
			for i in range(rec[3]-1):
				samples.append(bus.read_byte(curr_addr))
			loaddata(rec, samples, "current")			

		time.sleep(.1)
	
