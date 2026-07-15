from ._stlrom import *
from .signal import *
from .stl_driver import *
from .signal_gen import *

Signal.plot = plot
Signal.__repr__ = lambda self: (
    f"Signal(begin_time={self.begin_time}, end_time={self.end_time}, num_samples={self.get_samples_list().__len__()})"
)

STLDriver.get_signal_idx = get_signal_idx
STLDriver.get_signal = get_signal
STLDriver.plot_signal = plot_signal
STLDriver.plot_signals = plot_signals

STLMonitor.get_signal_idx = get_signal_idx
STLMonitor.get_signal = get_signal
STLMonitor.plot_signals = plot_signals


