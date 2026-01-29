import sys
sys.path.append('../build')
import stlrom
import matplotlib.pyplot as plt
import numpy as np
import time

def test_rob_from(formula, t0):
    rob1 = driver.get_online_rob(formula, t0)
    # display both formula and robustness in one formated line
    print("Robustness of formula ", formula, " at time ", t0, " is ", rob1)
    return rob1

driver=stlrom.STLDriver()
s="""signal x,y
mu1 := x[t]>0
mu2 := abs(y[t])>0
mmu_and := mu1 and mu2"""

driver.parse_string(s)

driver.add_sample([0, 0, 0])
driver.add_sample([1.5, -1.25, -2 ])
driver.add_sample([2., -2., 2. ])

test_rob_from("mmu_and", 2.)
