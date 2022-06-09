import sys
sys.path.append('../build')
import pyrobonTL

s=pyrobonTL.Signal();
s.set_BigM(1000);

assert(s.get_BigM()==1000)