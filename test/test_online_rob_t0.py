import sys
sys.path.append('../build')
import STLRom
import matplotlib.pyplot as plt
import numpy as np
import time

nb_sample=15
nb_signal=1

driver=STLRom.STLDriver()
s="signal x\nphi:= alw_[0,3] x[t]>0"
driver.parse_string(s)

driver.add_sample([0, 1])
driver.add_sample([1.3, -2 ])
driver.add_sample([2.1, 2. ])


rob1 = driver.get_online_rob("phi")
print(rob1)

rob2 = driver.get_online_rob("phi", 1)
print(rob2)

rob3 = driver.get_online_rob("phi", 2)
print(rob3)

assert(rob1[0]<0) 
assert(rob3[0]> 0)
