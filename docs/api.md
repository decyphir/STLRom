# Python API reference

Everything is imported from the top-level package: `import stlrom`. The classes are C++ classes exposed through pybind11, plus a few pure-Python helpers.

Every function has a docstring: `help(stlrom.STLDriver.get_rob)` shows it, with the argument names and defaults. Jupyter shows it too.

Times and values are floats. A `Signal` is piecewise-linear: a list of samples, each with a time, a value and a slope (see [`Signal`](#signal)).

Contents:
- [STLDriver](#stldriver): parse formulas, hold data, compute robustness
- [STLMonitor](#stlmonitor): one formula with its own copy of the data
- [Signal, Sample, Point](#signal)
- [STLData](#stldata)
- [Robustness maps](#robustness-maps)
- [Plotting](#plotting)
- [Signal generators](#signal-generators)
- [Internal names](#internal-names)

---

## STLDriver

The main entry point. A driver holds the signal and parameter declarations, the formulas, one data set and the current semantics.

```python
driver = stlrom.STLDriver()
driver.parse_string("signal x\nphi := ev_[0, 5] x[t] > 3")
driver.add_sample([0, 1.0])
```

### Parsing

| Method | Description |
|---|---|
| `parse_string(text) -> bool` | Parse a specification in the [formula language](stl_language.md). Returns `False` on a syntax error and prints the error to stderr. Can be called several times to add definitions. |
| `parse_file(filename, verbose=False) -> bool` | The same, from a file. With `verbose=True`, also prints the file and each definition. Returns `False` if the file cannot be opened. |
| `disp()` | Print the declarations, formulas and a summary of the data. `str(driver)` returns the same text. |

### Data

| Method | Description |
|---|---|
| `add_sample([t, v1, v2, ...], interp=True)` | Append one sample for every signal, in declaration order. With `interp=True`, the sample is joined to the previous one by a straight line. With `interp=False`, the previous value is held until this sample (piecewise-constant). Raises `ValueError` if the length is not 1 + the number of signals. **A sample at or before the last time is silently ignored.** |
| `set_signals([sig1, sig2, ...])` | Replace all data with a list of `Signal`s, one per declared signal, in declaration order. Raises `ValueError` if the count is wrong. |
| `load_csv([path1, path2, ...])` | Load one CSV file per declared signal, in declaration order. Each row is `t,value` or `t,value,slope`. |
| `write_csv(directory)` | Write one `<signal name>.csv` per signal into an existing directory, with rows `t,value,slope`. |
| `get_signal(name_or_index) -> Signal` | A copy of one signal's data. |
| `get_signal_idx(name) -> int`, `get_signame_from_idx(i) -> str` | Convert between a signal's name and its position. |
| `get_signals_names() -> str` | The declared names, space-separated, e.g. `"x y"`. |
| `data` | The underlying [`STLData`](#stldata). |

### Parameters and semantics

| Method | Description |
|---|---|
| `set_param(name, value)`, `get_param(name) -> float` | Change or read a parameter declared with `param`. The change applies to the next computation. |
| `set_semantics(name)`, `get_semantics() -> str` | `"SPACE"` (default), `"LEFT_TIME"`, `"RIGHT_TIME"` or `"BOOLEAN"`. An invalid name raises `ValueError`. See [Concepts](README.md#concepts). |

### Robustness

All of these take a formula name as their first argument (`phi_in`). An unknown name raises `ValueError`, and the message lists the formulas that are defined.

`dt` (default `inf`) is the largest time step used inside products of signals. It must be greater than 0, otherwise `ValueError` is raised.

| Method | Returns |
|---|---|
| `get_rob(phi_in="phi", t0=0.0, dt=inf) -> float` | Robustness at time `t0`. Raises `ValueError` if the data does not cover the formula's windows at `t0`. |
| `get_online_rob(phi_in="phi", t0=0.0, dt=inf) -> [rob, lower, upper]` | Robustness at `t0` from the data so far, with bounds valid for any future data. With no data at all: `[nan, -BigM, +BigM]`. |
| `get_rob_signal(phi_in)` | Robustness at time 0, as a one-sample `Signal`. |
| `get_rob_signal(phi_in, t)` | The same at time `t`. |
| `get_rob_signal(phi_in, t_start, t_end[, dt]) -> Signal` | Robustness over `[t_start, t_end]`, cut to the part the data covers. Empty if no part is covered. |
| `get_online_rob_signal(phi_in[, t_start[, t_end[, dt]]]) -> [z, z_low, z_up]` | Three `Signal`s: robustness so far, lower bound, upper bound. The argument forms are the same as for `get_rob_signal`. |
| `get_robustness_map(phi_in="phi") -> dict` | The robustness of every subformula from the last computation of `phi_in`. See [Robustness maps](#robustness-maps). |
| `get_online_robustness_map(phi_in="phi") -> dict` | The same, with the online bounds filled in. |

### Monitors

| Method | Description |
|---|---|
| `get_monitor(name) -> STLMonitor` | A monitor for one formula, with a **copy** of the current data, parameters and semantics. Raises `ValueError` if the name is unknown. |
| `copy()` | An independent copy of the driver. `copy.copy` and `copy.deepcopy` work too. |

---

## STLMonitor

One formula with its own data, obtained from `driver.get_monitor(name)`. Use it to run several formulas independently, or to keep monitoring one formula while the driver changes.

```python
mon = driver.get_monitor("phi")
mon.add_sample([4, 2.0])            # goes to the monitor's own data only
z, low, up = mon.get_online_rob_signal(0., 0.)
```

| Method / attribute | Description |
|---|---|
| `add_sample(sample, interp=True)`, `set_signals(signals)`, `load_csv(files)`, `write_csv(directory)` | As on the driver, applied to the monitor's data. |
| `get_signal`, `get_signal_idx`, `get_signals_names`, `plot_signal`, `plot_signals` | As on the driver. |
| `get_rob(t0=0.0, dt=inf) -> float` | As on the driver, without the formula name. |
| `get_online_rob(t0=0.0, dt=inf) -> [rob, lower, upper]` | As on the driver, without the formula name. |
| `get_rob_signal([t_start[, t_end[, dt]]]) -> Signal` | As on the driver, without the formula name. With no arguments, uses the range set by `set_eval_time`. |
| `get_online_rob_signal([t_start[, t_end[, dt]]]) -> [z, z_low, z_up]` | As on the driver. |
| `set_eval_time(t_start, t_end)` | The default range for the calls above. |
| `rob`, `lower_rob`, `upper_rob` | The values at `t_start` from the last computation. `get_lower_rob()` and `get_upper_rob()` return the last two. |
| `set_param`, `get_param`, `set_semantics`, `get_semantics`, `param_map` | As on the driver, for this monitor only. |
| `get_robustness_map()`, `get_online_robustness_map()` | See [Robustness maps](#robustness-maps). |
| `display_formula()` | Print the formula. |
| `copy()` | An independent copy. |

A monitor created directly with `STLMonitor()` has no formula. Its computing methods raise `RuntimeError`, so get monitors from `driver.get_monitor(name)`.

---

## Signal

A piecewise-linear signal: a sequence of samples `(time, value, slope)`. Between two samples the value follows the first sample's slope, so a jump happens where the next sample's value differs. Robustness results are `Signal`s, and you can build your own to pass to `set_signals`.

```python
s = stlrom.Signal()
s.append_linear_sample(0, 0.)
s.append_linear_sample(2, 4.)
s.value_at(1.)        # 2.0
```

| Method / attribute | Description |
|---|---|
| `append_linear_sample(t, v)` | Append, joined to the previous sample by a straight line. |
| `append_constant_sample(t, v)` | Append, with the previous value held until `t`. |
| `append_sample(t, v)`, `append_sample(t, v, slope)` | Append with an explicit slope (default 0), and no interpolation. |
| `append_sample(t, v, slope, interp)` | With `interp=True`, the previous sample's slope is replaced so the two are joined by a line. |
| `value_at(t) -> float` | The value at `t`. Before the first sample it extrapolates the first piece. An empty signal prints a message and returns 0. |
| `get_samples_list() -> list[Sample]` | The samples. |
| `begin_time`, `end_time` | The time range the signal covers. `end_time` can be later than the last sample. |
| `empty() -> bool` | `True` if there are no samples. |
| `resize(t0, t1)` | Restrict or extend to `[t0, t1]`. |
| `shift(dt)` | Move in time by `dt`. |
| `reverse()` | Reverse in time. |
| `read_from_file(path)`, `write_to_file(path)` | CSV with rows `t,value[,slope]`. |
| `compute_not(s)`, `compute_and(a, b)`, `compute_or(a, b)`, `compute_boolean(s)`, `compute_left_time_rob(s)`, `compute_right_time_rob(s)` | Replace this signal with the result of the operation on the arguments. These are the building blocks of the robustness computation. |
| `copy()` | An independent copy. |
| `plot(...)` | See [Plotting](#plotting). |

**Global settings** (static methods, shared by the whole process):

| Method | Default | Description |
|---|---|---|
| `Signal.set_BigM(v)`, `Signal.get_BigM()` | 10000 | The value standing for +∞ in robustness and online bounds. `get_BigM()` returns it **rounded down to an integer**. |
| `Signal.set_Eps(v)`, `Signal.get_Eps()` | 1e-10 | The margin used where a signal crosses an atom's threshold, and the robustness of a satisfied `==`. |
| `Signal.set_semantics(name)`, `Signal.get_semantics()` | `"SPACE"` | The global semantics. Drivers and monitors set it from their own setting at each computation, so use `driver.set_semantics` instead. |

### Sample and Point

`Sample(t, v, slope)` has the attributes `time`, `value` and `derivative`, plus `value_at(t)` (the value of its line at `t`). `Point(t, v)` has `time` and `value`.

---

## STLData

The data of a driver or monitor (`driver.data`): the declared signals and their samples. Most code uses the driver's methods instead.

| Method / attribute | Description |
|---|---|
| `signal_map` | `{name: index}` for the declared signals. |
| `data_vector` | The list of `Signal`s, in index order. |
| `add_sample(sample[, interp])` | As on the driver. |
| `add_signal_sample(name, t, v[, slope[, interp]])` | Append a sample to one signal only. Unknown names are ignored. |
| `get_signal(name_or_index)`, `get_signal_idx(name)`, `get_signame_from_idx(i)` | As on the driver. |

---

## Robustness maps

`get_robustness_map` and `get_online_robustness_map` return the robustness of every subformula, grouped by depth in the formula tree. The root is depth 0.

```python
{
  0: {"alw_[0,1] (x[t] > 3)": {"z": Signal, "z_low": Signal, "z_up": Signal}},
  1: {"x[t] > 3":             {...}},
  2: {"x":                    {...}},
}
```

The values come from the **last computation** of that formula, so call `get_rob_signal` or `get_online_rob_signal` first. Before any computation, the signals are empty. In the offline map, `z_low` and `z_up` can be `None`.

---

## Plotting

These need matplotlib, which is installed with stlrom. Each returns the matplotlib `Axes` it drew on.

| Function | Description |
|---|---|
| `Signal.plot(label=None, ax=None, title='Signal Plot', draw_samples=False, legend=True, ...)` | Plot a signal. Pass `ax` to draw on an existing axes. |
| `driver.plot_signal(name, label=None, ax=None, draw_samples=False)` | Plot one data signal. Also available on `STLMonitor`. |
| `driver.plot_signals(sig_names=None, same_ax=True, ax=None, draw_samples=False)` | Plot several data signals, by default all of them, on one axes. |
| `stlrom.plot_rob_map(rob_map, max_depth=None, to_plot="robustness", ax=None, same_figure=False, title=...)` | Plot a robustness map. `to_plot` is `"robustness"`, `"lower"` or `"upper"`. |
| `stlrom.plot_rob_map_widget(rob_map, title=...)` | An interactive version for Jupyter. Needs `ipywidgets` (`pip install stlrom[dev]`). |

---

## Signal generators

Helpers that make test signals. Each returns a `Signal` from `get_signal(t0, tf)`.

| Class | Signal | Example |
|---|---|---|
| `OscillSignalGen(period=1, amplitude=1, base=0, damp=0)` | `exp(damp*t) * amplitude * sin(2πt/period) + base`, sampled every `dt` | `OscillSignalGen(period=2).get_signal(0, 10, dt=0.1)` |
| `PWLSignalGen(times=[...], values=[...])` | piecewise-linear through the given points | `PWLSignalGen(times=[0,1,3], values=[0,2,1]).get_signal(0, 4)` |
| `PWCSignalGen(times=[...], values=[...])` | piecewise-constant, each value held until the next time | `PWCSignalGen(times=[0,1,3], values=[0,2,1]).get_signal(0, 4)` |
| `RandSignalGen(seed=0, dt_min=0.1, dt_max=1, v_min=-5, v_max=5, dv_max=1, interp='PREVIOUS')` | a random walk with random time steps. `interp='LINEAR'` joins the points with lines. | `RandSignalGen(seed=1).get_signal(0, 5)` |

`set_param(**kwargs)` changes a generator's parameters and returns the generator. The base class `SignalGen` is not usable on its own; use one of the subclasses. `get_time_array(t0, tf, dt)` returns `numpy.arange(t0, tf + dt, dt)`.

---

## Internal names

These are importable but are not part of the supported API and may change: `transducer` (a formula tree node, reachable as `monitor.formula`), `read_point`, `print_monitor`, and `STLMonitor.data`, `STLMonitor.formula`, `STLMonitor.up_to_date`.

## C++

The C++ API mirrors the Python one. `STLDriver` (`include/stl_driver.h`), `STLMonitor` (`include/stl_monitor.h`), `Signal` (`include/signal.h`) and `STLData` (`include/stl_data.h`) are in the `STLRom` namespace. Calls take `std::vector<double>` where Python takes a list.

The differences from Python:
- `STLDriver` also has `add_signal_sample` and `parse_stream`.
- A Python `ValueError` is a `std::invalid_argument` in C++, and a `RuntimeError` is a `std::runtime_error`.
