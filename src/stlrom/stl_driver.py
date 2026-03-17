from ._stlrom import Signal
from .signal import plot

def get_signal_idx(self, sig):    
    sig_list = self.get_signals_names().split()    
    return sig_list.index(sig) if sig in sig_list else -1

def get_signal(self, sig):
    if isinstance(sig, int):
        idx = sig
    else:
        idx = get_signal_idx(self, sig)
    
    sig = Signal()
    if idx is not -1:
        for sample in self.data:
            sig.append_sample(sample[0], sample[idx+1])
    return sig

def plot_signal(self, sig, label=None, ax=None):
    S = self.get_signal(sig)
    if label is None: 
        label = str(sig) #FIXME: dummy if sig is integer
    return plot(S, label=label, ax=ax)
    
