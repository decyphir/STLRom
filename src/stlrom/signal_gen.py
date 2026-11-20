"""Generators of test signals."""
from __future__ import annotations

from typing import Callable, Optional, Sequence

from ._stlrom import Signal
import numpy as np

__all__ = ["Signal", "get_time_array", "SignalGen", "OscillSignalGen", "PWCSignalGen", "PWLSignalGen", "RandSignalGen"]

def get_time_array(t0: float = 0, tf: float = 10, dt: float = 0.1) -> np.ndarray:
    """Times from t0 to tf (included) every dt: ``numpy.arange(t0, tf + dt, dt)``."""
    return np.arange(t0, tf+dt, dt) # added +dt to include tf

# WIP SignalGen should probably be interface only, below it interpolates a function fun
class SignalGen:
    """Base class of the generators: samples a function of time into a Signal.

    Use one of the subclasses; the base class cannot be used on its own.
    """
    def __init__(self, fun: Callable[[float], float] = lambda t:t):
        self.fun =  fun        
        self.interp = 'LINEAR' # 'PREVIOUS' or 'LINEAR'
        self.param_map={}

    def _update_fun(self):
       pass
    def _add_pwl_sample(self, t, v):
        self.sig.append_sample(t,v,0.,True)
    
    def _add_pwc_sample(self,t, v):
        self.sig.append_constant_sample(t,v)

    def set_param(self, **kargs: object) -> SignalGen:
        """Change generator parameters (see each class) and return the generator."""
        for item in kargs:
            self.param_map[item]= kargs[item]
        self._update_fun()
        return self

    def get_signal(self, t0: float, tf: float, dt: float = .1, time: Optional[Sequence[float]] = None,
                   **kargs: object) -> Signal:
        """Sample the function on [t0, tf] every dt (or at the given times) and return the Signal.

        Extra keyword arguments are passed to ``set_param`` first.
        """
        self.set_param(**kargs)
        self.sig = Signal()
            
        if time is None:
            time = get_time_array(t0, tf, dt)
           
        for t in time:
            self.add_sample(t, self.fun(t))
        self.sig.end_time = tf            
        return self.sig

class OscillSignalGen(SignalGen):
    """``exp(damp*t) * amplitude * sin(2*pi*t/period) + base``, sampled every dt by ``get_signal(t0, tf, dt)``.

    Example: ``OscillSignalGen(period=2).get_signal(0, 10, dt=0.1)``
    """
    def __init__(self, t0: float = 0, tf: float = 0, period: float = 1, amplitude: float = 1,
                 base: float = 0, damp: float = 0):
        super().__init__()
        self.param_map= {'period':period,'amplitude':amplitude, 'base':base, 'damp':damp}        
        self._update_fun()        

    def _update_fun(self):
        T = self.param_map['period']
        A = self.param_map['amplitude']
        d = self.param_map['damp']
        base =  self.param_map['base']
        self.fun = lambda t: np.exp(d*t)*A*np.sin(2*np.pi*t/T)+base
        if self.interp=='LINEAR':
            self.add_sample = self._add_pwl_sample
        else:
            self.add_sample = self._add_pwc_sample
 

class PWCSignalGen(SignalGen):
    """Piecewise-constant signal: each value is held until the next time.

    Example: ``PWCSignalGen(times=[0, 1, 3], values=[0, 2, 1]).get_signal(0, 4)``
    """
    def __init__(self, t0: float = 0, tf: float = 0, times: Sequence[float] = [0, 1.],
                 values: Sequence[float] = [0., 1.]):
        super().__init__()
        self.interp='PREVIOUS'
        self.param_map= {'times':times,'values':values}        
        self._update_fun()        

    def _update_fun(self):
        s = Signal()
        for t,v in zip(self.param_map['times'], self.param_map['values']):            
            s.append_constant_sample(t,v)
        self.sig = s
        self.fun = lambda t: s.value_at(t)

    def get_signal(self, t0: float, tf: float) -> Signal:
        """The signal restricted or extended to [t0, tf]."""
        s = self.sig
        s.resize(t0, tf)
        return s


class PWLSignalGen(SignalGen):
    """Piecewise-linear signal through the points (times[i], values[i]).

    Example: ``PWLSignalGen(times=[0, 1, 3], values=[0, 2, 1]).get_signal(0, 4)``
    """
    def __init__(self, t0: float = 0, tf: float = 0, times: Sequence[float] = [0, 1.],
                 values: Sequence[float] = [0., 1.], derivatives: Optional[Sequence[float]] = None):
        super().__init__()
        self.interp = 'LINEAR'
        self.param_map= {'times':times,'values':values}        
        self._update_fun()        

    def _update_fun(self):
        s = Signal()
        for t,v in zip(self.param_map['times'], self.param_map['values']):            
            s.append_linear_sample(t,v)
        self.sig = s
        self.fun = lambda t: s.value_at(t)

    def get_signal(self, t0: float, tf: float) -> Signal:
        """The signal restricted or extended to [t0, tf]."""
        s = self.sig
        s.resize(t0, tf)
        return s

class RandSignalGen(SignalGen):
    """Random walk: steps of random duration in [dt_min, dt_max] and random change in [-dv_max, dv_max],
    clipped to [v_min, v_max]. ``interp`` is 'PREVIOUS' (piecewise-constant) or 'LINEAR'.

    Example: ``RandSignalGen(seed=1).get_signal(0, 5)``
    """
    def __init__(self, seed: int = 0, t0: float = 0, tf: float = 0, dt_min: float = 0.1, dt_max: float = 1,
                 v_min: float = -5, v_max: float = 5, dv_max: float = 1, interp: str = 'PREVIOUS'):
        super().__init__()
        self.param_map={'seed':seed, 'dt_min':dt_min,'dt_max': dt_max, 'v_min': v_min, 'v_max': v_max, 'dv_max': dv_max }
        self.interp = interp        
        self._update_fun()
        self.get_signal(t0,tf) 

    def _fun(self, t):
        dt_min = self.param_map['dt_min']
        dt_max = self.param_map['dt_max']
        v_min = self.param_map['v_min']
        v_max = self.param_map['v_max']
        dv_max = self.param_map['dv_max']
        t_end = self.sig.end_time
            
        while t>t_end:            
            dv = self.rng.uniform(-dv_max,dv_max) 
            dt = self.rng.uniform(dt_min,dt_max)            
            v_prev = self.last_v
            v = max(v_prev+dv, v_min)
            v = min(v, v_max)
            self.add_sample(t_end+dt, v)
            t_end= self.sig.end_time    
            self.last_v = v
        
        return self.sig.value_at(t)

    def _update_fun(self):
        self.rng = np.random.default_rng(self.param_map['seed'])
        self.fun = lambda t: self._fun(t)
        self.sig = Signal()
        if self.interp=='LINEAR':
            self.add_sample = self._add_pwl_sample
        else:
            self.add_sample = self._add_pwc_sample
        
        v_min = self.param_map['v_min']
        v_max = self.param_map['v_max']        
        v = self.rng.uniform(v_min, v_max)                  
        self.last_v = v
        self.add_sample(0.,v)

    def get_signal(self, t0: float, tf: float) -> Signal:
        """The random signal on [t0, tf] (generated as far as needed)."""
        self._fun(tf)
        s = self.sig
        s.resize(t0, tf)
        return s
