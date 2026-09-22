import sys
sys.path.append('../build')
import stlrom


driver=stlrom.STLDriver()
s="signal x\nphi:=x[t]<2"
driver.parse_string(s)

driver.add_sample([0, 1])
driver.add_sample([0.5, -1])
driver.data.inflate_signal("x", 0.2)

robs = driver.get_online_rob("phi")
assert(robs[0]==1) #, "Robustness is wrong.")
assert(round(robs[1],15)==0.8)  # "Lower robustness is wrong.")
assert(round(robs[2],15)==1.2)  # "Upper robustness is wrong.")