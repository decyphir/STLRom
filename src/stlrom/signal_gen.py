from ._stlrom import Signal
import numpy as np

def get_time(t0=0, tf=10, dt=0.1):
    return np.arange(t0, tf+dt, dt) # added +dt to include tf

# WIP SignalGen should probably be interface only, below it interpolates a function fun
class SignalGen:
    def __init__(self, t0=0, tf=0, fun = lambda t:t):
        self.fun =  fun        
        self.interp =  'LINEAR' # 'PREVIOUS' or 'LINEAR'
        self.param_map={}

    def _update_fun(self):
       pass
    def _add_pwl_sample(self, t, v):
        self.sig.append_sample(t,v,0.,True)
    
    def _add_pwc_sample(self,t, v):
        self.sig.append_constant_sample(t,v)

    def set_param(self, **kargs):
        for item in kargs:
            self.param_map[item]= kargs[item]
        self._update_fun()
        return self

    def get_signal(self, time=None, t0=0, tf=10, dt=.1,**kargs):
        self.set_param(**kargs)
        self.sig = Signal()
            
        if time is None:
            time = get_time(t0, tf, dt)
        for t in time:
            self.add_sample(t, self.fun(t))
        self.sig.end_time = tf            
        return self.sig

class OscillSignalGen(SignalGen):
    def __init__(self, t0=0, tf=0, period=1, amplitude=1, base=0, damp=0):        
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
            self.add_sample= self._add_pwl_sample
        else:
            self.add_sample= self._add_pwc_sample
 

class PWCSignalGen(SignalGen):
    def __init__(self, t0=0, tf=0,times=[0, 1.], values=[0., 1.]):        
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

    def get_signal(self, t0=0, tf=0):        
        s = self.sig
        s.resize(t0, tf)
        return s


class PWLSignalGen(SignalGen):
    def __init__(self, t0=0, tf=0,times=[0, 1.], values=[0., 1.], derivatives=None):        
        super().__init__()
        self.interp = 'LINEAR'
        self.param_map= {'times':times,'values':values}        
        self._update_fun()        

    def _update_fun(self):
        s = Signal()
        for t,v in zip(self.param_map['times'], self.param_map['values']):            
            s.append_sample(t,v)
        self.sig = s
        self.fun = lambda t: s.value_at(t)

    def get_signal(self, t0=0, tf=10):        
        s = self.sig
        s.resize(t0, tf)
        return s

class RandSignalGen(SignalGen):
    def __init__(self, seed=0, t0=0, tf=0, dt_min=0.1, dt_max=1, v_min=-5, v_max=5, dv_max=1, interp='PREVIOUS'):
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

    def get_signal(self, t0=0, tf=10):        
        self._fun(tf)
        s = self.sig
        s.resize(t0, tf)
        return s
