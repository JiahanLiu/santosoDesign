import numpy as np
import MySQLdb
import matplotlib.pyplot as plt
import time


def removeDC(arr):
	max = np.amax(arr)
	min = np.amin(arr)
	offset = (max+min)/2

	for i in range(0, len(arr)):
		arr[i] = arr[i] - offset 


def getSamples(id):
	db = MySQLdb.connect("localhost", "wise", "wisepassword", "wisedb")

	dbcursor = db.cursor()
	dbcursor.execute("select * from data_samples where id = " + str(id))
	rec = dbcursor.fetchone()
	arr = []
	for i in range(6,134):
		arr.append(rec[i])
	removeDC(arr)
	return arr


def performFFT(samples):

	N=128
	T=1.0/7680.0
	fft  = np.fft.rfft(samples,N)
	return fft	


def initPlot():

	plt.show()


###############################################
# Main Loop
###############################################

# plot initialization
fig, ax = plt.subplots()
ind = np.arange(14)
print ind	
width = 0.20         # the width of the bars
ax.set_ylabel('Magnitude')
ax.set_title('Spectral Plot')
ax.set_xticks(ind+width/2)
ax.set_ylim(0,50)
ax.set_xticklabels(['0','60','120','180','300','360','420','480','540','600','660','720','780'])
rect1 = ax.bar(np.arange(14), np.arange(14), width, color='b')
plt.show(block=False)

id = 500

while True:

	samples = getSamples(id)	
	fft_data = performFFT(samples)
	y_data = np.array(abs(fft_data[0:13])/100).astype(int) 
	for rect, h in zip(rect1, y_data):
		rect.set_height(h)
	plt.draw()
	id = id + 1
	time.sleep(.1)

