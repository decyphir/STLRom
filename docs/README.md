# STLRom

STLRom is a C++11 library with Python bindings for **robust online monitoring of Signal Temporal Logic (STL)**.

You write STL formulas in a small text language and feed the library timed samples of your signals. It returns the **robustness** of each formula: a number whose sign says whether the formula holds (positive) or fails (negative) and whose size says by how much. It works on complete traces (offline) and on traces that are still arriving (online). Online, it also returns lower and upper bounds that hold whatever the future samples turn out to be.

The online bounds follow the interval robustness of Deshmukh et al., *Robust online monitoring of signal temporal logic* (Formal Methods in System Design, 2017). The algorithm differs from the one in the paper.

**Documentation**

| File | What it covers |
|---|---|
| this file | install, quickstart, concepts, contributing |
| [stl_language.md](stl_language.md) | the formula language: every keyword, operator and rule |
| [api.md](api.md) | the Python classes and functions |
| [CHANGELOG.md](CHANGELOG.md) | what changed in each release |
| [tutorial notebook](../test/notebook_test.ipynb) | a guided tour with plots ([Colab version](https://colab.research.google.com/drive/1RUkK4bsXKHawvb10N11oO3u-7CdiQBXT?usp=sharing)) |

## Install

STLRom is compiled when you install it, on Linux and macOS. You need:

- a C++ compiler and CMake 3.15 or later
- recent **Bison** and **Flex**:
  - macOS: `brew install bison flex`
  - Ubuntu: `sudo apt install bison flex`
- for the Python package, the Python headers (Ubuntu: `sudo apt install python-dev-is-python3`)

### Python, from PyPI

```bash
pip install stlrom
```

### Python, from a clone

Run this from the repository root:

```bash
pip install -e .
```

Or with [uv](https://docs.astral.sh/uv/):

```bash
uv sync                      # creates .venv and builds stlrom into it
source .venv/bin/activate
```

### C++ only

```bash
mkdir build && cd build
cmake ..
make
```

This builds the shared library `stlromlib` and a command-line parser called `STLRomParser` (see [Command-line parser](#command-line-parser)).

## Quickstart (Python)

```python
import stlrom

driver = stlrom.STLDriver()
driver.parse_string("""
signal x, y                   # must come first, declared once
param a = 1, T = 2
mu  := x[t] > a
phi := alw_[0, T] (mu and y[t] < 5)
""")

# one sample = [t, x, y], in declaration order, with increasing t
for sample in [[0, 2, 0], [1, 3, 1], [2, 4, 2], [3, 0.5, 4]]:
    driver.add_sample(sample)

print(driver.get_rob("phi"))          # robustness at t = 0
print(driver.get_rob("phi", 1.0))     # robustness at t = 1

rob = driver.get_rob_signal("mu", 0, 3)   # robustness of mu over [0, 3]
print(rob.value_at(2.5))
```

Output:

```
1.0
-0.5
1.25
```

`phi` holds at time 0 with margin 1, because `x - a` stays at least 1 over `[0, 2]`. It fails at time 1, because `x` drops to 0.5 at `t = 3`. You can also put the formulas in a file and call `driver.parse_file("spec.stl")`.

### Online monitoring

Ask for robustness before the data is complete, and you get `[rob, lower, upper]`. The true robustness at time 0 is guaranteed to lie in `[lower, upper]`:

```python
driver = stlrom.STLDriver()
driver.parse_string("""
signal x
phi := ev_[0, 5] x[t] > 3
""")

for t, x in [(0, 0), (1, 1), (2, 2), (3, 4)]:
    driver.add_sample([t, x])
    rob, lower, upper = driver.get_online_rob("phi")
    print(f"t={t}: phi(0) in [{lower}, {upper}]")
```

```
t=0: phi(0) in [-3.0, 10000.0]
t=1: phi(0) in [-2.0, 10000.0]
t=2: phi(0) in [-1.0, 10000.0]
t=3: phi(0) in [1.0, 10000.0]
```

At `t = 3`, `x` reaches 4, so `phi` is satisfied whatever comes next and the lower bound turns positive. The upper bound stays at "unknown" (`BigM`, see below) until the window `[0, 5]` is fully covered by data.

### Quickstart (C++)

```cpp
#include <iostream>
#include "stl_driver.h"
using namespace STLRom;

int main() {
    STLDriver driver;
    driver.parse_string("signal x, y\n"
                        "phi := alw_[0, 2] (x[t] > 0 and y[t] < 5)");
    driver.add_sample({0.0, 1.0, 2.0});   // {t, x, y}
    driver.add_sample({1.0, 2.0, 3.0});
    driver.add_sample({2.0, 0.5, 4.0});

    std::cout << driver.get_rob("phi", 0.) << std::endl;              // 0.5
    std::vector<double> online = driver.get_online_rob("phi", 0.);   // {rob, lower, upper}
}
```

To compile it, link against `stlromlib`. Put `include/` on the include path, plus `src/` (the generated parser headers live there after a build) and Flex's include directory.

## Concepts

**Signals** are piecewise-linear. Each sample is a time, a value and a slope. By default, `add_sample` joins consecutive samples with straight lines. Pass `add_sample(sample, False)` to hold each value until the next sample. Samples must arrive with strictly increasing times: a sample at or before the last time is **silently ignored**.

**Robustness** of a formula at time `t`:
- An atom `e1 > e2` has robustness `e1 - e2`, and `e1 < e2` has `e2 - e1`.
- `and` and `alw` take the minimum.
- `or` and `ev` take the maximum.
- `not` negates.

Values come back as a single number (`get_rob`) or as a `Signal` over a time range (`get_rob_signal`).

**Data coverage.** A formula is only defined where the data covers all of its time windows. For example, `alw_[0, 2] x[t] > 0` at `t = 1` needs data on `[1, 3]`. `get_rob` raises `ValueError` at times the data does not cover. `get_rob_signal` returns only the covered part, or an empty signal.

**Online bounds.** `get_online_rob` and `get_online_rob_signal` work with partial data. They return the robustness computed so far plus a lower and an upper bound. Where nothing is known yet, the bounds are `-BigM` and `+BigM`.

**Semantics.** `driver.set_semantics(name)` changes what robustness measures:

| Name | Meaning |
|---|---|
| `SPACE` (default) | how far the signal values are from violating the formula |
| `LEFT_TIME` | how long the atom has kept its truth value, looking back |
| `RIGHT_TIME` | how long the atom will keep its truth value, looking forward |
| `BOOLEAN` | 1 if true, 0 if false |

The two time semantics only give correct values with `get_rob_signal` over the whole data range. `get_rob(phi, t)` cuts the computation to the single instant `t` and returns about 0. See [Known limitations](#known-limitations).

**Global settings**, shared by the whole process:
- `stlrom.Signal.set_BigM(v)`, default 10000: the value that stands for +∞ in robustness and bounds.
- `stlrom.Signal.set_Eps(v)`, default 1e-10: the margin used when a signal crosses an atom's threshold.

**`dt`.** Products of signals (`x[t] * y[t]`) are not linear between samples. Passing `dt` to `get_rob`, `get_rob_signal`, `get_online_rob` or `get_online_rob_signal` adds evaluation points at least every `dt` time units inside products. The error shrinks like `dt²`. The default, `inf`, uses the data points only.

**Driver and monitors.** An `STLDriver` holds the declarations, the formulas, the parameters and one data set. `driver.get_monitor("phi")` returns an `STLMonitor` for one formula with **its own copy of the data**. Samples added to the driver afterwards do not reach the monitor, and samples added to the monitor do not reach the driver.

## Known limitations

- `parse_string` and `parse_file` print syntax errors to stderr and return `False`. Check the return value.
- With `LEFT_TIME` or `RIGHT_TIME`, `get_rob(phi, t)` and short `get_rob_signal` ranges give truncated values. Use `get_rob_signal` over the whole data.

## Command-line parser

`STLRomParser` parses a specification and prints what it defined. It is handy for checking syntax:

```bash
./STLRomParser -f spec.stl -v
```

Options: `-f/--file <file>` reads from a file instead of standard input, `-v/--verbose` explains each definition, `-d/--debug` traces the scanner and parser, and `-h/--help` prints usage.

## Contributing

### Build and test

From a clone with the Python package installed (see [Install](#install)) and its virtual environment active:

```bash
mkdir build && cd build
cmake ..
make
make test          # all tests; or make test-cpp / make test-python
```

The test suite is the list registered in [test/CMakeLists.txt](../test/CMakeLists.txt): 6 C++ programs and 10 Python scripts. The other scripts and the notebooks in `test/` are exploratory and are not run by `make test`.

Set `STLROM_DEBUG=ON` when installing the Python package, or `-DDEBUG=ON` with CMake, to compile in the debug tracing.

### How the code is organised

| Path | Contents |
|---|---|
| `src/scanner.l`, `src/parser.y` | the Flex scanner and Bison grammar for the formula language. Generated `parser.cpp`, `scanner.cpp` and `include/parser.hpp` are produced at build time and are not committed |
| `include/stl_driver.h`, `src/stl_driver.cpp` | `STLDriver`: parses specifications, holds the formulas and the data |
| `include/stl_monitor.h`, `src/stl_monitor.cpp` | `STLMonitor`: computes robustness of one formula over a time range |
| `include/stl_data.h` | `STLData`: the declared signals and their samples |
| `include/signal.h`, `src/signal.cpp`, `src/robustness.cpp` | `Signal` (a deque of samples) and the operations on signals: min, max, sliding-window max, until, time robustness |
| `include/transducer.h`, `include/signal_expr.h`, `src/*transducer*.cpp`, `src/stl_atom.cpp`, `src/signal_expr.cpp` | one class per operator |
| `src/pybind11/main.cpp` | the Python bindings |
| `src/stlrom/*.py` | pure-Python additions: plotting and test-signal generators |

A parsed formula is a **tree of transducers**, one node per operator. Each node has:
- a horizon `[start_time, end_time]`: the times it must compute, pushed down from its parent
- a result `z` (a `Signal`)
- in online mode, bounds `z_low` and `z_up`

Computing a formula means setting the horizon at the root and evaluating the tree bottom-up. The past operators (`hist`, `once`, `since`) reverse time and reuse the future algorithms.

### Docstrings and the type stub

Docstrings live next to the code:
- C++ classes: in [src/pybind11/main.cpp](../src/pybind11/main.cpp), as the last argument of each `.def`. Give every argument a `py::arg("name")`.
- Python helpers: in [src/stlrom/](../src/stlrom/), with type hints.

IDEs can't look inside the compiled module, so they read [src/stlrom/_stlrom.pyi](../src/stlrom/_stlrom.pyi) instead. That file is generated: **regenerate it whenever you change the bindings or a helper's signature**. With the changed package installed in the active environment, run this from the repository root:

```bash
pip install pybind11-stubgen   # once
pybind11-stubgen stlrom._stlrom -o src
```

It should print no `ERROR` lines. Two common causes of errors:
- A class used in a signature is registered *after* the code that uses it. Register it earlier in `main.cpp`.
- A helper's type annotation names something the generator can't import. Use full module paths, e.g. `matplotlib.axes.Axes`.

### When you change something users can see

- Add a line under **Unreleased** in [CHANGELOG.md](CHANGELOG.md).
- If you changed the language, update [stl_language.md](stl_language.md). If you changed the Python API, update [api.md](api.md), the docstrings and the stub.

## License

BSD 3-clause style, copyright Decyphir SAS. See [LICENSE](../LICENSE). The scanner and parser skeleton come from an MIT-licensed Flex/Bison example by Krzysztof Narkiewicz.
