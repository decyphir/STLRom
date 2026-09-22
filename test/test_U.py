import sys
sys.path.append('../build')
import stlrom

driver=stlrom.STLDriver()
s="signal x,y\nphi:= not(x[t]>3) => (x[t]>0 until_[0, 3] y[t]<0)"
driver.parse_string(s)

driver.add_sample([0,  2 , 1 ])
driver.add_sample([0.5, -1, 2])

x = driver.get_signal("x")
x.append_lower_sample(0,-1)
x.append_upper_sample(0, 2)
y = driver.get_signal("y")
y.append_upper_sample(0, 2)
driver.set_signals([x,y])

robs = driver.get_online_rob("phi")
assert(robs[0]==-1) #, "Robustness is wrong.")
assert(robs[1]==-x.get_BigM())  # "Lower robustness is wrong.")
assert(robs[2]==-1)  # "Upper robustness is wrong.")