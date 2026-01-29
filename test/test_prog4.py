import sys
sys.path.append('../build')
import stlrom

driver=stlrom.STLDriver()
s="signal x\nphi:=alw_[0, 5] x[t]>0"
driver.parse_string(s)
sig_names= driver.get_signals_names()

nb_sample=15
nb_signal=1

driver.add_sample([0, 1])
driver.add_sample([2, -1])
driver.add_sample([4, -3])

robs = driver.get_online_rob("phi")
assert(robs[0]==-3) #, "Robustness is wrong.")
assert(robs[1]==-10000)  # "Lower robustness is wrong.")
assert(robs[2]==-3)  # "Upper robustness is wrong.")


