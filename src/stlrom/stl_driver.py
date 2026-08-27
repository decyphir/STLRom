from ._stlrom import Signal
from .signal import plot

def get_signal_idx(self, sig):    
    return self.data.get_signal_idx(sig)

def get_signame_from_idx(self,idx):
    return self.data.get_signame_from_idx(idx)

def get_signal(self, sig):
    return self.data.get_signal(sig)

def get_tube(self, tube):
    return self.data.get_tube(tube)

def plot_signal(self, sig, label=None, ax=None, draw_samples=False):
    S = self.get_signal(sig)
    
    if S is not None and label is None: 
        label = "Signal " + str(sig) #FIXME: dummy if sig is integer
    return plot(S, label=label, ax=ax, draw_samples=draw_samples)
    
def plot_signals(self, sig_names=None, same_ax=True, ax=None, draw_samples=False):
    if sig_names is None:
        sig_names= list(self.data.signal_map.keys())

    if len(sig_names)>0:    
        if ax is None:
            ax = self.plot_signal(sig_names[0], draw_samples=draw_samples)
        else:
            self.plot_signal(sig_names[0], ax=ax, draw_samples=draw_samples)
        if same_ax:
            for sig in sig_names[1:]:
                self.plot_signal(sig, ax=ax,draw_samples=draw_samples)
        else:
            for sig in sig_names[1:]:
                self.plot_signal(sig, draw_samples=draw_samples)
    return ax
            

def plot_tube(self, tube, label=None, ax=None, draw_samples=False):
    T = self.get_tube(tube)
    
    if T is not None and label is None: 
        label = "Tube " + str(tube) #FIXME: dummy if sig is integer
    ax = plot(T.lower_signal, ax=ax, draw_samples=draw_samples)
    ax.lines[-1].set_label(None)
    return plot(T.upper_signal, label=label, ax=ax, draw_samples=draw_samples, color=ax.lines[-1].get_color())