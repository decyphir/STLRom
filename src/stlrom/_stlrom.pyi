"""
STLRom: robust online monitoring of Signal Temporal Logic (compiled core).
"""
from __future__ import annotations
import collections.abc
import matplotlib.axes
import typing
__all__: list[str] = ['Point', 'STLData', 'STLDriver', 'STLMonitor', 'Sample', 'Signal', 'print_monitor', 'read_point', 'transducer']
class Point:
    """
    A (time, value) pair.
    """
    def __copy__(self) -> Point:
        ...
    def __deepcopy__(self, memo: dict) -> Point:
        ...
    def __del__(self) -> None:
        ...
    @typing.overload
    def __init__(self, time: typing.SupportsFloat | typing.SupportsIndex, value: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @typing.overload
    def __init__(self, other: Point) -> None:
        """
        Copy constructor.
        """
    def copy(self) -> Point:
        """
        Return a copy.
        """
    def print_point(self) -> None:
        """
        Print the point.
        """
    @property
    def time(self) -> float:
        ...
    @time.setter
    def time(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def value(self) -> float:
        ...
    @value.setter
    def value(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class Sample:
    """
    One piece of a piecewise-linear Signal: from `time`, the value is value + derivative * (t - time).
    """
    def __copy__(self) -> Sample:
        ...
    def __deepcopy__(self, memo: dict) -> Sample:
        ...
    def __del__(self) -> None:
        ...
    def __init__(self, time: typing.SupportsFloat | typing.SupportsIndex, value: typing.SupportsFloat | typing.SupportsIndex, derivative: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    def copy(self) -> Sample:
        """
        Return a copy.
        """
    def print_sample(self) -> None:
        """
        Print the sample.
        """
    def time_intersect(self, other: Sample) -> float:
        """
        Time at which this sample's line meets the other sample's line.
        """
    def value_at(self, t: typing.SupportsFloat | typing.SupportsIndex) -> float:
        """
        Value of this sample's line at time t.
        """
    @property
    def derivative(self) -> float:
        """
        Slope of the piece starting at this sample.
        """
    @derivative.setter
    def derivative(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def time(self) -> float:
        ...
    @time.setter
    def time(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def value(self) -> float:
        ...
    @value.setter
    def value(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class Signal:
    """
    A piecewise-linear signal: a sequence of samples (time, value, slope).
    
    Between two samples the value follows the first sample's slope, so a jump happens where the
    next sample's value differs. Robustness results are Signals.
    """
    @staticmethod
    def get_BigM() -> int:
        """
        Return BigM, rounded down to an integer.
        """
    @staticmethod
    def get_Eps() -> float:
        """
        Return Eps.
        """
    @staticmethod
    def get_semantics() -> str:
        """
        Return the global semantics.
        """
    @staticmethod
    def set_BigM(value: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Set the value that stands for +infinity in robustness and online bounds (default 10000).
        """
    @staticmethod
    def set_Eps(value: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Set the margin used where a signal crosses an atom's threshold (default 1e-10).
        """
    @staticmethod
    def set_semantics(semantics: str) -> None:
        """
        Set the global semantics. Drivers and monitors overwrite it with their own setting at each
        computation: use STLDriver.set_semantics instead.
        """
    def __copy__(self) -> Signal:
        ...
    def __deepcopy__(self, memo: dict) -> Signal:
        ...
    def __del__(self) -> None:
        ...
    def __init__(self) -> None:
        """
        Create an empty signal.
        """
    def __repr__(self):
        ...
    def __str__(self) -> str:
        ...
    def append_constant_sample(self, t: typing.SupportsFloat | typing.SupportsIndex, v: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Append a sample; the previous value is held until t.
        """
    @typing.overload
    def append_linear_sample(self, t: typing.SupportsFloat | typing.SupportsIndex, v: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Append a sample joined to the previous one by a straight line.
        """
    @typing.overload
    def append_linear_sample(self, t: typing.SupportsFloat | typing.SupportsIndex, v: typing.SupportsFloat | typing.SupportsIndex, slope: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Append a sample joined to the previous one by a straight line; `slope` is the slope after t.
        """
    @typing.overload
    def append_sample(self, t: typing.SupportsFloat | typing.SupportsIndex, v: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Append a sample with slope 0 (the value is held until the next sample).
        
        Ignored if t is not after the last sample's time.
        """
    @typing.overload
    def append_sample(self, t: typing.SupportsFloat | typing.SupportsIndex, v: typing.SupportsFloat | typing.SupportsIndex, slope: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Append a sample with an explicit slope.
        
        Ignored if t is not after the last sample's time.
        """
    @typing.overload
    def append_sample(self, t: typing.SupportsFloat | typing.SupportsIndex, v: typing.SupportsFloat | typing.SupportsIndex, slope: typing.SupportsFloat | typing.SupportsIndex, interp: bool) -> None:
        """
        Append a sample. With interp=True, the previous sample's slope is replaced so that the two
        are joined by a straight line.
        
        Ignored if t is not after the last sample's time.
        """
    def compute_and(self, a: Signal, b: Signal) -> None:
        """
        Replace this signal with min(a, b).
        """
    def compute_boolean(self, s: Signal) -> None:
        """
        Replace this signal with 1 where s > 0 and 0 elsewhere.
        """
    def compute_left_time_rob(self, s: Signal) -> None:
        """
        Replace this signal with the left time robustness of s (how long s has kept its sign).
        """
    def compute_not(self, s: Signal) -> None:
        """
        Replace this signal with -s.
        """
    def compute_or(self, a: Signal, b: Signal) -> None:
        """
        Replace this signal with max(a, b).
        """
    def compute_right_time_rob(self, s: Signal) -> None:
        """
        Replace this signal with the right time robustness of s (how long s will keep its sign).
        """
    def copy(self) -> Signal:
        """
        Return a copy.
        """
    def empty(self) -> bool:
        """
        True if the signal has no samples.
        """
    def get_samples_list(self) -> list[Sample]:
        """
        Return the samples as a list.
        """
    def plot(self, label: str | None = None, ax: matplotlib.axes.Axes | None = None, title: str = 'Signal Plot', **kwargs: typing.Any) -> matplotlib.axes.Axes:
        """
        Plot the signal (available as ``Signal.plot``).
        
            Args:
                label: legend label; defaults to "Signal <n>".
                ax: axes to draw on; a new figure is created if None.
                title: axes title.
                **kwargs: ``draw_samples`` (bool, mark the samples, default False),
                    ``plot_sat`` (bool, also plot the Boolean satisfaction on a second axis, default False),
                    ``legend`` (bool, default True), ``draw_canvas`` (bool, default True),
                    ``plot_rob`` (bool, draw the signal itself, default True);
                    anything else is passed to ``matplotlib.axes.Axes.plot``.
        
            Returns:
                The axes drawn on.
            
        """
    def read_from_file(self, filename: str) -> None:
        """
        Replace the samples with those of a CSV file with rows 't,value' or 't,value,slope'.
        
        Raises:
            ValueError: if the file is missing or malformed.
        """
    def resize(self, t_start: typing.SupportsFloat | typing.SupportsIndex, t_end: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Restrict or extend the signal to [t_start, t_end], holding the end values where it is extended.
        """
    def reverse(self) -> None:
        """
        Reverse the signal in time over [begin_time, end_time].
        """
    def shift(self, delta: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Move the signal in time by delta.
        """
    def value_at(self, t: typing.SupportsFloat | typing.SupportsIndex) -> float:
        """
        Value at time t. Before the first sample, the first piece is extended backwards.
        An empty signal prints a message and returns 0.
        """
    def write_to_file(self, filename: str) -> None:
        """
        Write the samples to a CSV file with rows 't,value,slope'.
        """
    @property
    def begin_time(self) -> float:
        """
        Start of the time range the signal covers.
        """
    @begin_time.setter
    def begin_time(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def end_time(self) -> float:
        """
        End of the time range the signal covers (can be after the last sample).
        """
    @end_time.setter
    def end_time(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class STLData:
    """
    Declared signals and their samples (STLDriver.data, STLMonitor.data).
    """
    @typing.overload
    def __init__(self) -> None:
        """
        Create empty data with no signals.
        """
    @typing.overload
    def __init__(self, nb_signals: typing.SupportsInt | typing.SupportsIndex) -> None:
        """
        Create data with nb_signals empty signals named x0, x1, ...
        """
    @typing.overload
    def __init__(self, signals: collections.abc.Sequence[Signal]) -> None:
        """
        Create data from a list of Signals, named x0, x1, ...
        """
    def __str__(self) -> str:
        ...
    def add_sample(self, sample: collections.abc.Sequence[typing.SupportsFloat | typing.SupportsIndex], interp: bool = True) -> None:
        """
        Append one sample for every declared signal.
        
        Args:
            sample: [t, v1, v2, ...], values in signal declaration order.
            interp: True (default) joins the sample to the previous one with a straight line;
                False holds the previous value until t (piecewise-constant).
        
        A sample whose time is not after the last sample's time is ignored.
        
        Raises:
            ValueError: if len(sample) is not 1 + the number of signals.
        """
    @typing.overload
    def add_signal_sample(self, name: str, t: typing.SupportsFloat | typing.SupportsIndex, v: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Append a sample to one signal only. Unknown names are ignored.
        """
    @typing.overload
    def add_signal_sample(self, name: str, t: typing.SupportsFloat | typing.SupportsIndex, v: typing.SupportsFloat | typing.SupportsIndex, slope: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Append a sample with an explicit slope to one signal only. Unknown names are ignored.
        """
    @typing.overload
    def add_signal_sample(self, name: str, t: typing.SupportsFloat | typing.SupportsIndex, v: typing.SupportsFloat | typing.SupportsIndex, slope: typing.SupportsFloat | typing.SupportsIndex, interp: bool) -> None:
        """
        Append a sample to one signal only; interp=True joins it to the previous one by a line.
        Unknown names are ignored.
        """
    @typing.overload
    def get_signal(self, index: typing.SupportsInt | typing.SupportsIndex) -> Signal:
        """
        Return a copy of the signal at this index.
        """
    @typing.overload
    def get_signal(self, name: str) -> Signal:
        """
        Return a copy of the signal with this name.
        """
    def get_signal_idx(self, name: str) -> int:
        """
        Index of a signal, or -1 if there is no signal with this name.
        """
    def get_signame_from_idx(self, index: typing.SupportsInt | typing.SupportsIndex) -> str:
        """
        Name of the signal at this index.
        """
    @property
    def data_vector(self) -> list[Signal]:
        """
        The signals, in index order.
        """
    @data_vector.setter
    def data_vector(self, arg0: collections.abc.Sequence[Signal]) -> None:
        ...
    @property
    def signal_map(self) -> dict[str, int]:
        """
        {signal name: index}.
        """
    @signal_map.setter
    def signal_map(self, arg0: collections.abc.Mapping[str, typing.SupportsInt | typing.SupportsIndex]) -> None:
        ...
class transducer:
    """
    Internal: a node of a formula tree (one operator). Not part of the supported API.
    """
    def __init__(self) -> None:
        ...
    def __str__(self) -> str:
        ...
    def get_child(self) -> transducer:
        """
        Child of a unary operator, or None.
        """
    def get_childL(self) -> transducer:
        """
        Left child of a binary operator, or None.
        """
    def get_childR(self) -> transducer:
        """
        Right child of a binary operator, or None.
        """
    def get_formula_string(self) -> str:
        """
        The formula as text.
        """
    @property
    def z(self) -> Signal:
        """
        Robustness from the last computation.
        """
    @z.setter
    def z(self, arg0: Signal) -> None:
        ...
    @property
    def z_low(self) -> Signal:
        """
        Lower bound from the last online computation.
        """
    @z_low.setter
    def z_low(self, arg0: Signal) -> None:
        ...
    @property
    def z_up(self) -> Signal:
        """
        Upper bound from the last online computation.
        """
    @z_up.setter
    def z_up(self, arg0: Signal) -> None:
        ...
class STLMonitor:
    """
    One formula with its own copy of the data. Create it with STLDriver.get_monitor(name).
    
    Samples added to the monitor do not reach the driver, and the other way round.
    """
    def __copy__(self) -> STLMonitor:
        ...
    def __deepcopy__(self, memo: dict) -> STLMonitor:
        ...
    def __del__(self) -> None:
        ...
    def __init__(self) -> None:
        """
        Create an empty monitor (no formula). Use STLDriver.get_monitor instead.
        """
    def __str__(self) -> str:
        ...
    def add_sample(self, sample: collections.abc.Sequence[typing.SupportsFloat | typing.SupportsIndex], interp: bool = True) -> None:
        """
        Append one sample for every declared signal.
        
        Args:
            sample: [t, v1, v2, ...], values in signal declaration order.
            interp: True (default) joins the sample to the previous one with a straight line;
                False holds the previous value until t (piecewise-constant).
        
        A sample whose time is not after the last sample's time is ignored.
        
        Raises:
            ValueError: if len(sample) is not 1 + the number of signals.
        """
    def copy(self) -> STLMonitor:
        """
        Return an independent copy.
        """
    def display_formula(self) -> None:
        """
        Print the formula.
        """
    def get_lower_rob(self) -> float:
        """
        Lower bound at t_start from the last online computation.
        """
    def get_online_rob(self, t0: typing.SupportsFloat | typing.SupportsIndex = 0.0, dt: typing.SupportsFloat | typing.SupportsIndex = ...) -> list[float]:
        """
        Online robustness at time t0 from the data so far.
        
        Returns:
            [rob, lower, upper]: the robustness computed so far and bounds that hold for any future data.
            With no data at all: [nan, -BigM, BigM].
        
        Raises:
            ValueError: if dt <= 0.
            RuntimeError: if the monitor has no formula.
        """
    @typing.overload
    def get_online_rob_signal(self) -> list[Signal]:
        """
        [z, z_low, z_up] over the range set by set_eval_time: robustness so far, lower and upper bounds.
        """
    @typing.overload
    def get_online_rob_signal(self, t: typing.SupportsFloat | typing.SupportsIndex) -> list[Signal]:
        """
        [z, z_low, z_up] at time t: robustness so far, lower and upper bounds.
        """
    @typing.overload
    def get_online_rob_signal(self, t_start: typing.SupportsFloat | typing.SupportsIndex, t_end: typing.SupportsFloat | typing.SupportsIndex) -> list[Signal]:
        """
        [z, z_low, z_up] over [t_start, t_end]: robustness so far, lower and upper bounds.
        """
    @typing.overload
    def get_online_rob_signal(self, t_start: typing.SupportsFloat | typing.SupportsIndex, t_end: typing.SupportsFloat | typing.SupportsIndex, dt: typing.SupportsFloat | typing.SupportsIndex) -> list[Signal]:
        """
        [z, z_low, z_up] over [t_start, t_end]: robustness so far, lower and upper bounds.
        
        dt: largest time step used inside products of signals; must be > 0.
        """
    def get_online_robustness_map(self) -> dict:
        """
        Same as get_robustness_map, with the online bounds from the last online computation.
        """
    def get_param(self, name: str) -> float:
        """
        Return the value of a parameter.
        """
    def get_rob(self, t0: typing.SupportsFloat | typing.SupportsIndex = 0.0, dt: typing.SupportsFloat | typing.SupportsIndex = ...) -> float:
        """
        Robustness of the formula at time t0.
        
        Args:
            t0: evaluation time.
            dt: largest time step used inside products of signals; inf uses the data points only.
        
        Raises:
            ValueError: if the data does not cover the formula's time windows at t0, or dt <= 0.
            RuntimeError: if the monitor has no formula.
        """
    @typing.overload
    def get_rob_signal(self) -> Signal:
        """
        Robustness over the range set by set_eval_time (default [0, 0]).
        """
    @typing.overload
    def get_rob_signal(self, t: typing.SupportsFloat | typing.SupportsIndex) -> Signal:
        """
        Robustness at time t, as a one-sample Signal.
        """
    @typing.overload
    def get_rob_signal(self, t_start: typing.SupportsFloat | typing.SupportsIndex, t_end: typing.SupportsFloat | typing.SupportsIndex) -> Signal:
        """
        Robustness over [t_start, t_end], cut to the part covered by the data (empty if none).
        """
    @typing.overload
    def get_rob_signal(self, t_start: typing.SupportsFloat | typing.SupportsIndex, t_end: typing.SupportsFloat | typing.SupportsIndex, dt: typing.SupportsFloat | typing.SupportsIndex) -> Signal:
        """
        Robustness over [t_start, t_end], cut to the part covered by the data (empty if none).
        
        dt: largest time step used inside products of signals; must be > 0.
        """
    def get_robustness_map(self) -> dict:
        """
        Robustness of every subformula from the last computation:
        {depth: {subformula: {'z': Signal, 'z_low': Signal, 'z_up': Signal}}}, the root at depth 0.
        """
    def get_semantics(self) -> str:
        """
        Return the semantics name.
        """
    def get_signal(self, sig: str | int) -> Signal:
        """
        A copy of one data signal, by name or index.
        """
    def get_signal_idx(self, sig: str) -> int:
        """
        Index of a declared signal, or -1 if there is no signal with this name.
        """
    def get_signals_names(self) -> str:
        """
        The declared signal names, space-separated.
        """
    def get_upper_rob(self) -> float:
        """
        Upper bound at t_start from the last online computation.
        """
    def load_csv(self, files: collections.abc.Sequence[str]) -> None:
        """
        Load one CSV file per declared signal, in declaration order.
        
        Each row is 't,value' or 't,value,slope'.
        
        Raises:
            ValueError: if the number of files does not match the declaration, or a file is missing or malformed.
        """
    def plot_signal(self, sig: str | int, label: str | None = None, ax: matplotlib.axes.Axes | None = None, draw_samples: bool = False) -> matplotlib.axes.Axes:
        """
        Plot one data signal (by name or index); returns the axes drawn on.
        """
    def plot_signals(self, sig_names: list[str] | None = None, same_ax: bool = True, ax: matplotlib.axes.Axes | None = None, draw_samples: bool = False) -> matplotlib.axes.Axes | None:
        """
        Plot several data signals (all of them by default).
        
            Args:
                sig_names: names to plot; all declared signals if None.
                same_ax: draw them on one axes; otherwise one figure each.
                ax: axes to draw on; a new figure is created if None.
                draw_samples: mark the samples.
        
            Returns:
                The axes of the first signal, or ``ax`` if there is nothing to plot.
            
        """
    def set_eval_time(self, t_start: typing.SupportsFloat | typing.SupportsIndex, t_end: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Set the default range used by get_rob_signal() and get_online_rob_signal() with no arguments.
        """
    def set_param(self, name: str, value: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Change the value of a parameter for this monitor.
        """
    def set_semantics(self, semantics: str) -> None:
        """
        Set the robustness semantics: 'SPACE' (default), 'LEFT_TIME', 'RIGHT_TIME' or 'BOOLEAN'.
        
        Raises:
            ValueError: for any other name.
        """
    def set_signals(self, signals: collections.abc.Sequence[Signal]) -> None:
        """
        Replace all data with one Signal per declared signal, in declaration order.
        
        Raises:
            ValueError: if the number of signals does not match the declaration.
        """
    def write_csv(self, directory: str) -> None:
        """
        Write one '<signal name>.csv' per signal into an existing directory (rows 't,value,slope').
        """
    @property
    def data(self) -> STLData:
        """
        The monitor's data (STLData).
        """
    @data.setter
    def data(self, arg0: STLData) -> None:
        ...
    @property
    def end_time(self) -> float:
        """
        End of the last requested range.
        """
    @end_time.setter
    def end_time(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def formula(self) -> transducer:
        """
        Internal: the root of the formula tree.
        """
    @formula.setter
    def formula(self, arg0: transducer) -> None:
        ...
    @property
    def lower_rob(self) -> float:
        """
        Lower bound at t_start from the last online computation.
        """
    @lower_rob.setter
    def lower_rob(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def param_map(self) -> dict[str, float]:
        """
        Parameter values, {name: value}.
        """
    @param_map.setter
    def param_map(self, arg0: collections.abc.Mapping[str, typing.SupportsFloat | typing.SupportsIndex]) -> None:
        ...
    @property
    def rob(self) -> float:
        """
        Robustness at t_start from the last computation.
        """
    @rob.setter
    def rob(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def start_time(self) -> float:
        """
        Start of the last requested range.
        """
    @start_time.setter
    def start_time(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
    @property
    def up_to_date(self) -> bool:
        """
        Internal.
        """
    @up_to_date.setter
    def up_to_date(self, arg0: bool) -> None:
        ...
    @property
    def upper_rob(self) -> float:
        """
        Upper bound at t_start from the last online computation.
        """
    @upper_rob.setter
    def upper_rob(self, arg0: typing.SupportsFloat | typing.SupportsIndex) -> None:
        ...
class STLDriver:
    """
    Parses STL specifications, holds the formulas, the parameters and one data set, and computes robustness.
    
    Example:
        driver = STLDriver()
        driver.parse_string('signal x\\nphi := ev_[0, 5] x[t] > 3')
        driver.add_sample([0, 1.0])
        rob, lower, upper = driver.get_online_rob('phi')
    """
    def __copy__(self) -> STLDriver:
        ...
    def __deepcopy__(self, memo: dict) -> STLDriver:
        ...
    def __del__(self) -> None:
        ...
    def __init__(self) -> None:
        """
        Create an empty driver.
        """
    def __str__(self) -> str:
        ...
    def add_sample(self, sample: collections.abc.Sequence[typing.SupportsFloat | typing.SupportsIndex], interp: bool = True) -> None:
        """
        Append one sample for every declared signal.
        
        Args:
            sample: [t, v1, v2, ...], values in signal declaration order.
            interp: True (default) joins the sample to the previous one with a straight line;
                False holds the previous value until t (piecewise-constant).
        
        A sample whose time is not after the last sample's time is ignored.
        
        Raises:
            ValueError: if len(sample) is not 1 + the number of signals.
        """
    def copy(self) -> STLDriver:
        """
        Return an independent copy.
        """
    def disp(self) -> None:
        """
        Print the declarations, formulas and a summary of the data.
        """
    def get_monitor(self, name: str) -> STLMonitor:
        """
        Return a monitor for one formula, with a copy of the current data, parameters and semantics.
        
        Raises:
            ValueError: if no formula has this name.
        """
    def get_online_rob(self, phi_in: str = 'phi', t0: typing.SupportsFloat | typing.SupportsIndex = 0.0, dt: typing.SupportsFloat | typing.SupportsIndex = ...) -> list[float]:
        """
        Online robustness of a formula at time t0 from the data so far.
        
        Returns:
            [rob, lower, upper]: the robustness computed so far and bounds that hold for any future data.
            With no data at all: [nan, -BigM, BigM].
        
        Raises:
            ValueError: if the formula is not defined or dt <= 0.
        """
    @typing.overload
    def get_online_rob_signal(self, phi_in: str) -> list[Signal]:
        """
        [z, z_low, z_up] at time 0: robustness so far, lower and upper bounds.
        
        Raises:
            ValueError: if the formula is not defined.
        """
    @typing.overload
    def get_online_rob_signal(self, phi_in: str, t: typing.SupportsFloat | typing.SupportsIndex) -> list[Signal]:
        """
        [z, z_low, z_up] at time t: robustness so far, lower and upper bounds.
        """
    @typing.overload
    def get_online_rob_signal(self, phi_in: str, t_start: typing.SupportsFloat | typing.SupportsIndex, t_end: typing.SupportsFloat | typing.SupportsIndex) -> list[Signal]:
        """
        [z, z_low, z_up] over [t_start, t_end]: robustness so far, lower and upper bounds.
        """
    @typing.overload
    def get_online_rob_signal(self, phi_in: str, t_start: typing.SupportsFloat | typing.SupportsIndex, t_end: typing.SupportsFloat | typing.SupportsIndex, dt: typing.SupportsFloat | typing.SupportsIndex) -> list[Signal]:
        """
        [z, z_low, z_up] over [t_start, t_end]: robustness so far, lower and upper bounds.
        
        dt: largest time step used inside products of signals; must be > 0.
        """
    def get_online_robustness_map(self, phi_in: str = 'phi') -> dict:
        """
        Same as get_robustness_map, with the online bounds from the last online computation.
        
        Raises:
            ValueError: if the formula is not defined.
        """
    def get_param(self, name: str) -> float:
        """
        Return the value of a parameter.
        """
    def get_rob(self, phi_in: str = 'phi', t0: typing.SupportsFloat | typing.SupportsIndex = 0.0, dt: typing.SupportsFloat | typing.SupportsIndex = ...) -> float:
        """
        Robustness of a formula at time t0.
        
        Args:
            phi_in: formula name.
            t0: evaluation time.
            dt: largest time step used inside products of signals; inf uses the data points only.
        
        Raises:
            ValueError: if the formula is not defined, the data does not cover its time windows at t0,
                or dt <= 0.
        """
    @typing.overload
    def get_rob_signal(self, phi_in: str) -> Signal:
        """
        Robustness of a formula at time 0, as a one-sample Signal.
        
        Raises:
            ValueError: if the formula is not defined.
        """
    @typing.overload
    def get_rob_signal(self, phi_in: str, t: typing.SupportsFloat | typing.SupportsIndex) -> Signal:
        """
        Robustness of a formula at time t, as a one-sample Signal.
        """
    @typing.overload
    def get_rob_signal(self, phi_in: str, t_start: typing.SupportsFloat | typing.SupportsIndex, t_end: typing.SupportsFloat | typing.SupportsIndex) -> Signal:
        """
        Robustness of a formula over [t_start, t_end], cut to the part covered by the data (empty if none).
        """
    @typing.overload
    def get_rob_signal(self, phi_in: str, t_start: typing.SupportsFloat | typing.SupportsIndex, t_end: typing.SupportsFloat | typing.SupportsIndex, dt: typing.SupportsFloat | typing.SupportsIndex) -> Signal:
        """
        Robustness of a formula over [t_start, t_end], cut to the part covered by the data (empty if none).
        
        dt: largest time step used inside products of signals; must be > 0.
        """
    def get_robustness_map(self, phi_in: str = 'phi') -> dict:
        """
        Robustness of every subformula from the last computation of the formula:
        {depth: {subformula: {'z': Signal, 'z_low': Signal, 'z_up': Signal}}}, the root at depth 0.
        
        Raises:
            ValueError: if the formula is not defined.
        """
    def get_semantics(self) -> str:
        """
        Return the semantics name.
        """
    def get_signal(self, sig: str | int) -> Signal:
        """
        A copy of one data signal, by name or index.
        """
    def get_signal_idx(self, sig: str) -> int:
        """
        Index of a declared signal, or -1 if there is no signal with this name.
        """
    def get_signals_names(self) -> str:
        """
        The declared signal names, space-separated.
        """
    def get_signame_from_idx(self, idx: int) -> str:
        """
        Name of the signal at this index.
        """
    def load_csv(self, files: collections.abc.Sequence[str]) -> None:
        """
        Load one CSV file per declared signal, in declaration order.
        
        Each row is 't,value' or 't,value,slope'.
        
        Raises:
            ValueError: if the number of files does not match the declaration, or a file is missing or malformed.
        """
    def parse_file(self, filename: str, verbose: bool = False) -> bool:
        """
        Parse a specification file. With verbose=True, also print the file and each definition.
        
        Returns:
            False if the file cannot be opened or has a syntax error (the error is printed to stderr).
        """
    def parse_string(self, text: str) -> bool:
        """
        Parse a specification. Can be called several times to add definitions.
        
        Returns:
            False on a syntax error (the error is printed to stderr).
        """
    def plot_signal(self, sig: str | int, label: str | None = None, ax: matplotlib.axes.Axes | None = None, draw_samples: bool = False) -> matplotlib.axes.Axes:
        """
        Plot one data signal (by name or index); returns the axes drawn on.
        """
    def plot_signals(self, sig_names: list[str] | None = None, same_ax: bool = True, ax: matplotlib.axes.Axes | None = None, draw_samples: bool = False) -> matplotlib.axes.Axes | None:
        """
        Plot several data signals (all of them by default).
        
            Args:
                sig_names: names to plot; all declared signals if None.
                same_ax: draw them on one axes; otherwise one figure each.
                ax: axes to draw on; a new figure is created if None.
                draw_samples: mark the samples.
        
            Returns:
                The axes of the first signal, or ``ax`` if there is nothing to plot.
            
        """
    def set_param(self, name: str, value: typing.SupportsFloat | typing.SupportsIndex) -> None:
        """
        Change the value of a parameter; applies to the next computation.
        """
    def set_semantics(self, semantics: str) -> None:
        """
        Set the robustness semantics: 'SPACE' (default), 'LEFT_TIME', 'RIGHT_TIME' or 'BOOLEAN'.
        
        Raises:
            ValueError: for any other name.
        """
    def set_signals(self, signals: collections.abc.Sequence[Signal]) -> None:
        """
        Replace all data with one Signal per declared signal, in declaration order.
        
        Raises:
            ValueError: if the number of signals does not match the declaration.
        """
    def write_csv(self, directory: str) -> None:
        """
        Write one '<signal name>.csv' per signal into an existing directory (rows 't,value,slope').
        """
    @property
    def data(self) -> STLData:
        """
        The driver's data (STLData).
        """
    @data.setter
    def data(self, arg0: STLData) -> None:
        ...
def print_monitor(driver: STLDriver) -> None:
    """
    Internal test function: print the monitor of formula 'phi'.
    """
def read_point() -> int:
    """
    Internal test function: read a point from stdin and print it.
    """
