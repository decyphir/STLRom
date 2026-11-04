from ._stlrom import Signal
from .signal import plot

def get_signal_idx(self, sig):    
    return self.data.get_signal_idx(sig)

def get_signame_from_idx(self,idx):
    return self.data.get_signame_from_idx(idx)

def get_signal(self, sig):
    return self.data.get_signal(sig)

def add_bounds(self, sample): # sample = [t, [sig1_lb, sig1_ub], [sig2_lb, sig2_ub], ...]
    t, sigs = sample[0], sample[1:]
    self.add_lower_bound([t] + [s[0] for s in sigs])
    self.add_upper_bound([t] + [s[1] for s in sigs])

def plot_signal(self, sig, label=None, ax=None, draw_samples=False, draw_tube=False, **kwargs):
    S = self.get_signal(sig)
    
    if S is not None and label is None: 
        label = "Signal " + str(sig) #FIXME: dummy if sig is integer
    return plot(S, label=label, ax=ax, draw_samples=draw_samples, draw_tube=draw_tube, **kwargs)
    
def plot_signals(self, sig_names=None, same_ax=True, ax=None, draw_samples=False, draw_tube=False):
    if sig_names is None:
        sig_names= list(self.data.signal_map.keys())

    if len(sig_names)>0:    
        if ax is None:
            ax = self.plot_signal(sig_names[0], draw_samples=draw_samples, draw_tube=draw_tube)
        else:
            self.plot_signal(sig_names[0], ax=ax, draw_samples=draw_samples, draw_tube=draw_tube)
        if same_ax:
            for sig in sig_names[1:]:
                self.plot_signal(sig, ax=ax,draw_samples=draw_samples, draw_tube=draw_tube)
        else:
            for sig in sig_names[1:]:
                self.plot_signal(sig, draw_samples=draw_samples, draw_tube=draw_tube)
    return ax

def plot_rob(self, phi, t0, tf, label=None, ax=None, **kwargs):
    S = self.get_rob_signal(phi, t0, tf)
    
    if S is not None and label is None: 
        label = str(phi)
    return plot(S, label=label, ax=ax, **kwargs)

def plot_online_rob(self, phi, t0, tf, label=None, ax=None, **kwargs):
    S = self.get_online_rob_signal(phi, t0, tf)
    
    if S is not None and label is None: 
        label = str(phi)
    return plot(S, label=label, ax=ax, draw_tube=True, **kwargs)

def plot_sat(self, phi, t0, tf, label=None, ax=None, **kwargs):
    S = self.get_rob_signal(phi, t0, tf)
    
    if S is not None and label is None: 
        label = str(phi)
    return plot(S, label=label, ax=ax, plot_rob=False, plot_sat=True, **kwargs)
