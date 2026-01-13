import sys
sys.path.append('../build')
import stlrom

driver=stlrom.STLDriver()
s="signal x,y\nphi:= not(x[t]>3) => (x[t]>0 until_[0, 3] y[t]<0)"
driver.parse_string(s)
sig_names= driver.get_signals_names()

driver.add_sample([0,  2 , 1 ])
driver.add_sample([0.5, -1, 2])

robs = driver.get_online_rob("phi")
assert(robs[0]<0) #, "Robustness is wrong.")
assert(robs[1]<0)  # "Lower robustness is wrong.")
assert(robs[2]<0)  # "Upper robustness is wrong.")