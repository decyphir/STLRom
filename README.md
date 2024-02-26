# STLRom 

STLRom is a C++ library with python bindings for Robust online monitoring of Signal Temporal Logic. It computes interval robustness as defined in 'Robust online monitoring of signal temporal logic' by Deshmuk et al, although the algorithm is not exactly the one described in this paper. 

## Compilation

STLRom is written in C++ with Python bindings using pybind11. Python 3.10 or higher is recommended. To compile, run  
```
cd build
cmake ..
make
```

## Testing the C++ Library

In the test folder, `test_driver.cpp` is an example of using the C++ library. Go into build/test and run it with 
```
./test_driver
```

## Testing the python bindings

In the build folder, execute
```
make test
```
It will run the unit tests  python programs from the `test` folder.

## Example

```python
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
```