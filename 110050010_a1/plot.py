#!/usr/bin/env python
from datetime import datetime
import matplotlib.pyplot as plt

f = open('cpu_usage', 'r')

x = []
y = []
timestamps = []
for z in xrange(0,999):
	a = f.readline()

	x.append(z)
	timestamps.append(a.split(',')[0])
	y.append(float(a.split(',')[1]))
	
f.close()

plt.plot(x, y)
plt.show()

x = []
t0 = datetime.strptime(timestamps[0], "%Y-%m-%d %H:%M:%S.%f ")
for z in xrange(1,999):
	t1 = datetime.strptime(timestamps[z], "%Y-%m-%d %H:%M:%S.%f ")
	t2 = t1 - t0

	x.append(t2.microseconds/1000)
	pass

plt.hist(x,bins=30)
plt.show()