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
	spi.max_speed_hz = 500000 

	G.setwarnings(False)
	G.setmode(G.BOARD)
	G.setup(A0_PIN, G.OUT)
	G.setup(A1_PIN, G.OUT)
	G.setup(A2_PIN, G.OUT)
	G.setup(A3_PIN, G.OUT)
	getchannelsettings()

def getchannelsettings():
	global channelSettings

	channelSettings = []
	dbcursor = db.cursor()
	dbcursor.execute("select * from comm_cfg")
	for rec in dbcursor.fetchall():
		channelSettings.append(rec)

def setchannel(rec):
	G.output(A0_PIN, rec[3])
	G.output(A1_PIN, rec[4])
	G.output(A2_PIN, rec[5])
	G.output(A3_PIN, rec[6])

def loaddata(channel, samples):

	dt = datetime.datetime.now()
	str_channel = str(channel)
	insert_stmt = "" 
	type = ['voltage','voltage', 'voltage', 'current', 'current', 'current']
	phase = ['a', 'b', 'c', 'a', 'b', 'c']

	dbcursor = db.cursor()

	for i in range(0,6):
		insert_stmt = "" 
		insert_stmt += "insert into data_samples values (0,"
		insert_stmt += str_channel + ","
		insert_stmt += "'" + type[i] + "',"
		insert_stmt += "'" + phase[i] + "',"
		insert_stmt += "'" + dt.strftime("%Y-%m-%d %H:%M:%S") + "',"
		insert_stmt += "'" + str(dt.microsecond) + "',"
		for j in range(0, 128):
       			insert_stmt += str(samples[(j*6)+i])
       			if j != 127:
               			insert_stmt += ","
		insert_stmt += ")"
		dbcursor.execute(insert_stmt)
		print insert_stmt

	db.commit()


def plotsamples(samples):
	
	
	blah = []
	for i in range(0, 128):
		blah.append(samples[i*6])
	print blah
	plt.plot(blah,'ro')
	plt.show()


################################################
# Main Loop
################################################

initialize()

while True:
	for rec in channelSettings:
			
		if rec[1] == 0:
			continue
		print "Channel " + str(rec[0])
		setchannel(rec)
		resp = spi.readbytes(1)
		print resp
		if resp[0] == 255:
			start = time.time()	
			samples = spi.readbytes(rec[2])
			end = time.time()
			print end - start
			plotsamples(samples)
			loaddata(rec[0], samples)			
		elif resp[0] != 0:
			resp = spi.writebytes([9])
	break;
	
