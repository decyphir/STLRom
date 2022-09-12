import sys
sys.path.append('../build')
import STLRom

s=STLRom.Signal();
s.set_BigM(1000);

assert(s.get_BigM()==1000)