import psutil
from datetime import datetime
import time

for x in xrange(1,1001):
	t1 = datetime.now()
	print t1, ',', psutil.cpu_percent(), ','
	time.sleep(1)