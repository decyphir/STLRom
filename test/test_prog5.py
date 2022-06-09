import sys
sys.path.append('../build')
import pyrobonTL

driver=pyrobonTL.STLDriver()
s="signal x\nphi:=ev_[3, 4] x[t]<5"
driver.parse_string(s)
sig_names= driver.get_signals_names()

driver.add_sample([0, 1])
#driver.add_sample([0.5, -1])

robs = driver.get_online_rob("phi")
assert(robs[0]==4) #, "Robustness is wrong.")
assert(robs[1]==-10000)  # "Lower robustness is wrong.")
assert(robs[2]==10000)  # "Upper robustness is wrong.")


