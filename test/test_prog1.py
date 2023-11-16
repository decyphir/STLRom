import sys
sys.path.append('../build')
import STLRom

nb_sample=15
nb_signal=1

driver=STLRom.STLDriver()
s="signal x\nphi:=x[t]>2"
driver.parse_string(s)
sig_names= driver.get_signals_names()
signal=STLRom.rand_trace_data(nb_sample+2,nb_signal+1)

for i in range (nb_sample) :
	driver.add_sample(signal[i]);

robs = driver.get_online_rob("phi")
print(robs)
assert(robs[0]<=robs[1]) #, "Robustness is wrong.")
assert(robs[1]<= robs[2])  # "Lower robustness is wrong.")
