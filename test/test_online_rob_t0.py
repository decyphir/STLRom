import sys
sys.path.append('../build')
import stlrom

def test_rob_from(formula, t0):
    rob1 = driver.get_online_rob(formula, t0)
    # display both formula and robustness in one formated line
    print("Robustness of formula ", formula, " at time ", t0, " is ", rob1)
    return rob1

driver=stlrom.STLDriver()
s="""signal x,y
param p1=3,p2=-2
mu1 := x[t]>p1
mu2 := abs(y[t])>p2
mmu_or := mu1 or mu2 
mmu_and := mu1 and mu2 
phi:= alw_[0,3] mmu_or"""

driver.parse_string(s)

driver.add_sample([0, 0, 0])
driver.add_sample([1.5, -1.25, -2 ])
driver.add_sample([2., -2., 2. ])

# request user a value for t_in and loop until t_in is -1
t_in = 1.2
test_rob_from("phi", t_in)
test_rob_from("mu1", t_in)
test_rob_from("mu2", t_in)
test_rob_from("mmu_or", t_in)
test_rob_from("mmu_and", t_in)