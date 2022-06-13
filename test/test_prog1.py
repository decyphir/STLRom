import sys
sys.path.append('../build')
import pyrobonTL
import matplotlib.pyplot as plt
import numpy as np

nb_sample=15
nb_signal=1

driver=pyrobonTL.STLDriver()
ts="signal x\nphi:=x[t]>2"
driver.parse_string(s)
sig_names= driver.get_signals_names()
signal=pyrobonTL.rand_trace_data(nb_sample+2,nb_signal+1)

#####Plot function#####
signal=np.array(signal)
x=signal.T[0]
y=signal.T[1]
plt.plot(x,y)
plt.show()
#######################

for i in range (nb_sample) :
	driver.add_sample(signal[i]);

robs = driver.get_online_rob("phi")
quit()