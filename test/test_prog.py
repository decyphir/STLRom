import sys
sys.path.append('../build')
import pyrobonTL

driver=pyrobonTL.STLDriver()
phi=pyrobonTL.STLMonitor()
driver.parse_file("spec.stl")
phi=driver.get_monitor("phi")
phi.add_sample([1,2,3])