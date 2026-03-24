import stlrom as sr
import numpy as np

d1 = sr.STLDriver()
d1.parse_string(
"""
signal x
mu:= x[t]>0
phi := ev_[0, 2.] ( x[t]>0 )
""")
d1.add_sample([0., 0.1])  
d1.add_sample([0.5, 1])
d1.add_sample([1.5, -1])
#d1.add_sample([4, .5])
#d1.add_sample([5, -1.])
#print("Online rob at 0:")
#print(d1.get_online_rob('mu'))

#print("Online rob at 3:")
#print(d1.get_online_rob('mu',3))

#mu = d1.get_monitor('mu')
#mu.eval_rob(0,3)
#print('z:',mu.formula.z)
#print('z_low:',mu.formula.z_low)
#print('z_up:',mu.formula.z_up)

phi = d1.get_monitor('phi')
phi.eval_rob(0,3)
#print(phi.formula.z)