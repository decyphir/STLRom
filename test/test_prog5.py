import sys
sys.path.append('../build')
import pyrobonTL

driver=pyrobonTL.STLDriver()
s="signal x\nphi:=ev_[3, 4] x[t]<5"
driver.parse_string(s)
