import sys
sys.path.append('../build')
import stlrom

driver=stlrom.STLDriver()
s="signal x,y\nphi:=x[t]>0 => y[t]>2"
driver.parse_string(s)

sig_names= driver.get_signals_names()
print(sig_names)

driver.add_sample([0, 1, 1])
driver.add_sample([0.5, -1, 2])
print(driver.data)
driver.disp()

robs = driver.get_online_rob("phi")
print(robs)
assert(robs[0]==-1) #, "Robustness is wrong.")
assert(robs[1]==-1)  # "Lower robustness is wrong.")
assert(robs[2]==-1)  # "Upper robustness is wrong.")


