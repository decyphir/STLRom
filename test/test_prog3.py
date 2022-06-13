import sys
sys.path.append('../build')
import pyrobonTL

driver=pyrobonTL.STLDriver()
s="signal x,y\nphi:=x[t]>0 or y[t]<0"
driver.parse_string(s)
sig_names= driver.get_signals_names()

nb_sample=15
nb_signal=1

driver.add_sample([0, 1])
#driver.add_sample([0.5, -1])

robs = driver.get_online_rob("phi")
assert(robs[0]==1) #, "Robustness is wrong.")
assert(robs[1]==1)  # "Lower robustness is wrong.")
assert(robs[2]==1)  # "Upper robustness is wrong.")


