import sys
sys.path.append('../build')
import pyrobonTL

driver=pyrobonTL.STLDriver()
s="signal x,y\nphi:=x[t]>0 => y[t]>2"
driver.parse_string(s)
