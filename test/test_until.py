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
s="""
signal x, y

workspace := (x[t] >= 0 and x[t] <= 5 and y[t] >= 0 and y[t] <= 5)
puddle := (x[t] >= 2 and x[t] <= 3 and y[t] <= 5)
carpet := (y[t] <= 1 and x[t] >= 4)
goal := (y[t] >= 4 and x[t] >= 4)

# safety := (not puddle) or (ev_[0,3] carpet) # test 1
# safety := (not puddle) or (workspace until_[0, 3] carpet) # test 2
 safety := (not puddle) or ((not goal) until_[0, 3] carpet) # test 3
# safety := puddle => ((not goal) until_[0,3] carpet) # test 4

phi := (workspace and safety) until_[0,10] goal
"""
driver.parse_string(s)

driver.add_sample([0,  2 , 1 ])
driver.add_sample([0.5, -1, 2])
robs = driver.get_online_rob("phi", 0)

print(robs)
