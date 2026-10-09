import sys
sys.path.append('../build')
import stlrom


driver=stlrom.STLDriver()
s="signal x\nphi:=x[t]>2"
driver.parse_string(s)

driver.add_sample([0, 1])
driver.add_sample([0.5, -1])
driver.data.inflate_signal("x", 0.2)

rob_sig = driver.get_rob_signal("phi", 0, 0.5)
# expected_rob_sig = stlrom.Signal(0,-1) # TODO bug
expected_rob_sig = stlrom.Signal()
expected_rob_sig.append_linear_sample(0,-1)
expected_rob_sig.append_linear_sample(0.5,-3)
print(rob_sig)
print(expected_rob_sig)
assert(rob_sig == expected_rob_sig) # TODO should be True equality overload to check

robs = driver.get_online_rob("phi")
print(robs)
assert(robs[0]==-1) #, "Robustness is wrong.")
assert(robs[1]==-1.2)  # "Lower robustness is wrong.")
assert(robs[2]==-0.8)  # "Upper robustness is wrong.")