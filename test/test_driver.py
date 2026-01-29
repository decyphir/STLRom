import sys
sys.path.append('../build')
import stlrom
driver=stlrom.STLDriver()
s="signal x\nphi:= alw_[0., 3.] (x[t]<5)"
driver.parse_string(s)

driver.add_sample([0, 1]);
driver.add_sample([.1, 2]);
robs = driver.get_online_rob("phi")
print("Robs:")
print(robs)
driver.add_sample([.2, 6]);
robs = driver.get_online_rob("phi")
print(robs)
