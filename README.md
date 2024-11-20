# STLRom 

STLRom is a C++ library with python bindings for Robust online monitoring of Signal Temporal Logic. It computes interval robustness as defined in 'Robust online monitoring of signal temporal logic' by Deshmuk et al, although the algorithm is not exactly the one described in this paper. 

## Install

The simplest way to install and use STLRom is to use pip:
```
$ pip install stlrom
``` 

## Usage

### Tutorial Notebook

The following [notebook](test/tutorial.ipynb) is formated as a tutorial for stlrom. It can also be accessed on [colab](https://colab.research.google.com/drive/1RUkK4bsXKHawvb10N11oO3u-7CdiQBXT?usp=sharing).

###  A complete Example

```python
import stlrom


# define a "driver", which can parse and store a context of STL formulas, as well as data
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
print('Robustness of phi1 from time 1.: ', phi1.eval_rob(1.))
```



## Compiling from source

STLRom is written in C++ with Python bindings using pybind11. Python 3.10 or higher is recommended. To compile, run  
```
cd build
cmake ..
make
```

### Testing the C++ Library

In the test folder, `test_driver.cpp` is an example of using the C++ library. Go into build/test and run it with 
```
./test_driver
```

### Testing the python bindings

In the build folder, execute
```
make test
```
It will run the unit tests  python programs from the `test` folder.


