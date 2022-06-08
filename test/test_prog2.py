import sys
sys.path.append('../build')
import pyrobonTL

driver=pyrobonTL.STLDriver()
s="signal x\nphi:=x[t]<2"
driver.parse_string(s)
