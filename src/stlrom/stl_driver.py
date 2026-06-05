from ._stlrom import Signal
from .signal import plot

def get_signal_idx(self, sig):    
    if sig in self.signal_map:
        idx= self.signal_map[sig]
    else:
        idx=-1
    return idx

def get_signame_from_idx(self,idx):
    signame=''
    for s in self.signal_map:
        if self.signal_map[s]==idx:
            signame= s
            break
    return signame

def get_signal(self, sig):

    if isinstance(sig, int):
        idx = sig
    else:
        idx = get_signal_idx(self, sig)
    
    sig = None
    if idx!=-1:
        sig= self.data[idx]

    return sig

def plot_signal(self, sig, label=None, ax=None):
    S = self.get_signal(sig)
    
    if S is not None and label is None: 
        label = str(sig) #FIXME: dummy if sig is integer
    return plot(S, label=label, ax=ax)
    
