import sys
sys.path.append('../build')
import pyrobonTL

driver=pyrobonTL.STLDriver()
s="signal x,y\nphi:=x[t]>0 or y[t]<0"
driver.parse_string(s)
