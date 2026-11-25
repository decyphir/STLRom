# Changelog

User-visible changes to STLRom. The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

## [Unreleased]

Changes on the `formula_signal` branch, which split from `main` before 0.4.0 was tagged.

### Added
- Past temporal operators: `hist_[a,b]` (`H`), `once_[a,b]` (`O`) and `since_[a,b]` (`S`).
- Time shifts in expressions: `x[t+c]` and `x[t-c]`, where `c` is a number or a parameter.
- Functions: `f := <expression>`, used later as `f[t]`.
- A formula's robustness used as a value: `phi[t]`, `phi[t-c]`.
- `t` as a value in expressions, e.g. `x[t] > 2*t`.
- An optional `dt` argument on `get_rob`, `get_rob_signal`, `get_online_rob` and `get_online_rob_signal`. It is the largest time step used inside products of signals; the default `inf` keeps the old behaviour.
- `STLMonitor.get_rob(t0=0.0, dt=inf)` and `STLMonitor.get_online_rob(t0=0.0, dt=inf)` in Python.
- Docstrings and argument names for the whole Python API, shown by `help()` and in Jupyter.

### Changed
- `get_rob` raises `ValueError` at times where the data does not cover the formula's time windows. It used to return a value anyway. Outside the data, signals now hold their end values.
- `get_rob_signal` returns only the part of the requested range that the data covers, or an empty signal.
- With no data, `get_online_rob` returns `[nan, -BigM, +BigM]`. Formulas that read no signal, such as `t > 0`, still get a value.
- An unknown formula name raises `ValueError` listing the defined formulas. This applies to `get_rob`, `get_online_rob`, `get_rob_signal`, `get_online_rob_signal`, the robustness maps and `get_monitor`. These calls used to print a message and return `0.0`, an empty result or an empty monitor.
- The computing methods of a monitor with no formula (`STLMonitor()`) raise `RuntimeError` instead of crashing.
- `add_sample(sample, interp=True)` and `parse_file(filename, verbose=False)` now take keyword arguments.
- `stlrom.np` (numpy, exported by accident) is no longer part of the package namespace.
- `Signal.value_at` holds the end values outside the signal, as `resize` does, and prints a warning there. It used to extend the first piece backwards. At a sample's time it now returns that sample's value: at a jump, the new value instead of the old one.
- Double unary minus is simplified away. Sequences such as `+-` without parentheses are rejected.

### Fixed
- Online bounds of `hist`, `once` and `until`. `until` returned nothing when asked for robustness before its data arrived.
- An atom crossing its threshold between the last sample and the end of the signal was missed.
- `abs` now handles a zero crossing, including one that falls on the next sample.
- Time reversal of signals with jumps, used by the past operators.
- Several bugs in `Signal.resize`, signal merging (`and`/`or`), simplification and the sliding-window min/max.
- `BOOLEAN` semantics.
- CSV output now keeps 15 significant digits.
- `transducer.get_child()`, `get_childL()` and `get_childR()` return references owned by the parent. Python used to take ownership of nodes the formula tree still owned.
- `x[t-c]` and `x[t+c]` on a range (`get_rob_signal`) lost their last piece when the range ended before the data did.
- `set_param` now changes a shift given by a parameter (`x[t+c]`). The shift used to keep the parameter's value at parse time.

## [0.4.0] - 2026-09-18

### Added
- `STLData` class holding the declared signals and their samples (`driver.data`), with `get_signal`, `get_signal_idx`, `get_signame_from_idx` and `add_signal_sample`.
- `set_signals`, `load_csv` and `write_csv` on drivers and monitors.
- `add_sample(sample, interp)`: `interp=False` gives piecewise-constant data.
- `get_rob_signal`, `get_online_rob_signal`, `get_robustness_map` and `get_online_robustness_map`.
- `BOOLEAN` semantics. The atoms `<=`, `>=` and `==`.
- Plotting: `Signal.plot`, `plot_signal(s)`, `plot_rob_map` and `plot_rob_map_widget`. matplotlib is now a dependency.
- Test-signal generators: `OscillSignalGen`, `PWLSignalGen`, `PWCSignalGen` and `RandSignalGen`.
- `parse_file(path, verbose)`.
- A build with scikit-build-core and uv. `STLROM_DEBUG` turns on debug tracing.
- Tutorial notebooks.

### Changed
- `STLDriver` computes through an internal `STLMonitor`.

## [0.3.0] - 2026-02-02

Not tagged in the repository.

### Changed
- New Flex/Bison parser for the formula language, in the `STLRom` C++ namespace, with the `STLRomParser` command-line tool.

## [0.2.1] - 2025-01-24

### Added
- `copy()`, `copy.copy` and `copy.deepcopy` for drivers, monitors and signals.

### Fixed
- A memory leak in `get_online_rob`.

## [0.2.0] - 2024-11-20

### Added
- `STLMonitor`, with its own semantics, interpolation and parameters. `STLDriver.get_monitor` copies the data into the monitor.
- First version of time robustness (`LEFT_TIME`, `RIGHT_TIME`).
- Tutorial notebook.

## [0.1.1] - 2024-03-18

### Fixed
- Packaging for pip.

## [0.1.0] - 2024-02-26

First release on PyPI.

### Added
- `STLDriver` with `parse_string`, `add_sample`, `get_rob` and `get_online_rob(t0)`.
- Parameters (`set_param`, `get_param`) and `BigM`.
