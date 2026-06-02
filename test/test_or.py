import sys
sys.path.append('../build')
import stlrom

driver=stlrom.STLDriver()
s="signal x,y\nphi:=x[t]>0 or y[t]<0"
driver.parse_string(s)

driver.add_sample([0, 1, -3])
driver.add_sample([0.5, -1, 2])

robs = driver.get_online_rob("phi")
assert(robs[0]==3) #, "Robustness is wrong.")
assert(robs[1]==3)  # "Lower robustness is wrong.")
assert(robs[2]==3)  # "Upper robustness is wrong.")


