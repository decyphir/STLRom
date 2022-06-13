import sys
sys.path.append('../build')
import pyrobonTL

driver=pyrobonTL.STLDriver()
s="signal x\nphi:=x[t]>2"
driver.parse_string(s)
sig_names= driver.get_signals_names()

signal=pyrobonTL.rand_trace_data(2,2)
for i in range (2) :
	driver.add_sample(signal[i]);

robs = driver.get_online_rob("phi")
print(robs)
assert(robs[0]<=robs[1]) #, "Robustness is wrong.")
assert(robs[1]<= robs[2])  # "Lower robustness is wrong.")
