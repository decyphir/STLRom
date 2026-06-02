import sys
sys.path.append('../build')
import stlrom


driver=stlrom.STLDriver()
s="signal x\nphi:=x[t]<2"
driver.parse_string(s)

driver.add_sample([0, 1])
driver.add_sample([0.5, -1])

robs = driver.get_online_rob("phi")
assert(robs[0]==1) #, "Robustness is wrong.")
assert(robs[1]==1)  # "Lower robustness is wrong.")
assert(robs[2]==1)  # "Upper robustness is wrong.")