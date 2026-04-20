from ._stlrom import *
from .signal import *
from .stl_driver import *
from .signal_gen import *

Signal.plot = plot
STLDriver.get_signal_idx = get_signal_idx
STLDriver.get_signal = get_signal
STLDriver.plot_signal = plot_signal

STLMonitor.get_signal_idx = get_signal_idx
STLMonitor.get_signal = get_signal
STLMonitor.plot_signal = plot_signal

