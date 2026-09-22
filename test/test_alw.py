import sys
sys.path.append('../build')
import stlrom

driver=stlrom.STLDriver()
s="signal x\nphi:=alw_[0, 5] x[t]>0"
driver.parse_string(s)

driver.add_sample([0, 1])
driver.add_sample([2, -1])
driver.add_sample([4, -3])

x = driver.get_signal("x")
x.append_lower_sample(0,-10)
x.append_lower_sample(3,-12)
x.append_upper_sample(0, 1.1)
x.append_upper_sample(1, 0)
x.append_upper_sample(4,-2)
driver.set_signals([x])

robs = driver.get_online_rob("phi")
assert(robs[0]==-3) #, "Robustness is wrong.")
assert(robs[1]==-x.get_BigM())  # "Lower robustness is wrong.")
assert(robs[2]==-2)  # "Upper robustness is wrong.")


