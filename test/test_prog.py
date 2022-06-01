import sys
sys.path.append('../build')
import pyrobonTL

driver=pyrobonTL.STLDriver()
driver.parse_file("spec.stl")
driver.disp()
