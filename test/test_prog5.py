import sys
sys.path.append('../build')
import stlrom

driver=stlrom.STLDriver()
s="signal x\nphi:=ev_[3, 4] x[t]<5"
driver.parse_string(s)
sig_names= driver.get_signals_names()

nb_sample=15
nb_signal=1

driver.add_sample([0, 1])
driver.add_sample([3.5, -1])
driver.add_sample([5, -1])

robs = driver.get_online_rob("phi")
assert(robs[0]==6) #, "Robustness is wrong.")
assert(robs[1]==6)  # "Lower robustness is wrong.")
assert(robs[2]==6)  # "Upper robustness is wrong.")


