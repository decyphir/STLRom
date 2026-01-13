import sys
sys.path.append('../build')
import stlrom

stl_driver =stlrom.STLDriver()

s="""
signal x, y    # signal names
mux := x[t]>0  # simple predicate 

param a=1, b=2, p = -3   
muy := a*y[t] + b > p   # operation and parameter in predicate

phi1 := alw_[0, 5] mux               # always (G works too) 
phi2 := ev_[3, 4] (mux) or phi1      # eventually (or F)
phi_until := phi1 until_[0, 1] phi2  # until 
"""

# parse the formulas
stl_driver.parse_string(s)  # or write the above in spec.stl, and use parse_file('spec.stl')

# add data as timed samples
stl_driver.add_sample([0,  2 , 1 ])  # must be of the format [t, x_val, y_val]
stl_driver.add_sample([0.5, -3, 2])  # i.e., contain signal value with same order as declared
stl_driver.add_sample([2.1, 10, 20])


# create monitor for phi 
phi1 = stl_driver.get_monitor("phi1")
print('Robustness of phi1 at time 0.: ', phi1.eval_rob())
print('Robustness of phi1 at time 1.: ', phi1.eval_rob(1.))

