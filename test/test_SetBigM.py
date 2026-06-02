import sys
sys.path.append('../build')
import stlrom

s=stlrom.Signal();
s.set_BigM(1000);

assert(s.get_BigM()==1000)