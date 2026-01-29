# STLRom 

STLRom is a C++ library with python bindings for Robust online monitoring of Signal Temporal Logic. It computes interval robustness as defined in 'Robust online monitoring of signal temporal logic' by Deshmuk et al, although the algorithm is not exactly the one described in this paper. 

## Install

stlrom is only available on Linux based systems.

### Building the C++ library 

STLRom is primarily a C++ library. It comes with a standard cmake configuration file. Assuming cmake and a C++ development environment are installed, a library called stlromlib can be built with

```
mkdir build; cd build
cmake ..
make
```

### Python bindings

A python interface for stlrom is implemented with pybind11. To build and install it in a local virtual environment we recommend using [uv](https://docs.astral.sh/uv/getting-started/installation/).
To create a virtual environment, build and install stlrom, run:

```bash
uv venv
source .venv/bin/activate
uv pip install -e .
```
Note this requires compiling the C++ library, so cmake and a C++ compiler must be available.

It is also possible to install the Python interface by simply running the following command in the project's root directory:

```bash
uv sync
```

Note, however, that this does not activate the virtual environment. You can run Python files that use the created `stlrom` library by running:

```bash
uv run python filename.py
```

Or you can activate the virtual environment by running
```bash
source .venv/bin/activate
```
and then using Python normally inside the virtual enviornment.

### Testing the C++ Library

After running `cmake` in the `build` folder and building the C++ library using `make`, you can run:
```bash
make test-cpp
```
to run C++ test files in the `test` folder.

### Testing the python bindings

In the build folder, execute
```bash
make test-python
```
This will run the unit tests python programs from the `test` folder. This assumes having run `cmake`, having built the Python library, and having activated the `uv` virtual environment.

**Note**: you can also execute `make test` to run both C++ and Python test units.

## Usage

### Tutorial Notebook

The following [notebook](test/notebook_test.ipynb) is formated as a tutorial for stlrom. It can also be accessed on [colab](https://colab.research.google.com/drive/1RUkK4bsXKHawvb10N11oO3u-7CdiQBXT?usp=sharing).

###  A Complete Example

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


