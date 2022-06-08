import sys
sys.path.append('../build')
import pyrobonTL

driver=pyrobonTL.STLDriver()
s="signal x\nphi:=alw_[0, 5] x[t]>0"
driver.parse_string(s)
