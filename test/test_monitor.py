import os
import sys
import importlib
import matplotlib.pyplot as plt


# Get the absolute path to the directory containing the local module
stlrom_path = os.path.abspath('../build')

# Insert the local module path at the beginning of sys.path
sys.path.insert(0, stlrom_path)

import stlrom

# Reload the module to ensure it is loaded from the new path
importlib.reload(stlrom)

import stlrom as sr
import numpy as np

stl_parser = stlrom.STLDriver()
stl_parser.parse_string("signal x, y")       # declaring signals named 'x' and 'y'
stl_parser.parse_string("param p=0.2, tau=5") # declaring parameters named 'p' and 'q'
# Let's add some formulas
stl_parser.parse_string("mu_x := x[t] > 0 ") # a atomic proposition
stl_parser.parse_string("mu_y := y[t] < p ") # another atomic proposition
stl_parser.parse_string("mu := mu_x and mu_y") # a compound proposition
stl_parser.parse_string("phi := ev_[0, tau] mu") # a temporal proposition
stl_parser.disp()

phi = stl_parser.get_monitor("phi")
phi.add_sample([0., 0., 0.])  # time, x, y
phi.add_sample([0.5, 1.1, 2.2])
#phi.set_interpol('PREVIOUS')

#phi.set_eval_time(0.,.001)
phi.eval_robustness() # should be -2.

print(phi.rob)
