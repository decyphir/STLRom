import sys
sys.path.append('../build')
import stlrom


stl_monitor =stlrom.STLDriver()
s="""
signal x, y    # signal names
mux := x[t]>0  # simple predicate 

param a=1, b=2, p = -3   
muy := a*y[t] + b > p   # operation and parameter in predicate

phi1 := alw_[0, 5] mux            # always (G works too) 
phi2 := ev_[3, 4] (mux) or phi1   # eventually (or F)
phi_until := phi1 until_[0, 1] phi2  # dreaded until 
"""

# parse the formulas
stl_monitor.parse_string(s)  # or write the above in spec.stl, and use parse_file('spec.stl')

# add data as timed samples
stl_monitor.add_sample([0,  2 , 1 ])  # must be of the format [t, x_val, y_val]
stl_monitor.add_sample([0.5, -3, 2])  # i.e., contain signal value with same order as declared
stl_monitor.add_sample([2.1, 10, 20])

# get the robustness of the formulas at time t0
def test_rob(formula, t0):
    rob1 = stl_monitor.get_online_rob(formula, t0)
    print("Interval robustness of formula ", formula, " from time ", t0, " is ", rob1)

test_rob("phi1",0)
test_rob("phi2",1)
test_rob("phi_until",1.1)
